package com.example

import android.graphics.Bitmap
import androidx.activity.compose.rememberLauncherForActivityResult
import androidx.activity.result.contract.ActivityResultContracts
import androidx.compose.foundation.Canvas
import androidx.compose.foundation.background
import androidx.compose.foundation.gestures.detectTapGestures
import androidx.compose.foundation.gestures.detectTransformGestures
import androidx.compose.foundation.horizontalScroll
import androidx.compose.foundation.layout.*
import androidx.compose.foundation.rememberScrollState
import androidx.compose.material.icons.Icons
import androidx.compose.material.icons.filled.*
import androidx.compose.material3.*
import androidx.compose.runtime.*
import androidx.compose.runtime.saveable.rememberSaveable
import androidx.compose.ui.Alignment
import androidx.compose.ui.Modifier
import androidx.compose.ui.draw.clipToBounds
import androidx.compose.ui.geometry.Offset
import androidx.compose.ui.geometry.Size
import androidx.compose.ui.graphics.Color
import androidx.compose.ui.graphics.drawscope.CanvasDrawScope
import androidx.compose.ui.graphics.drawscope.Stroke
import androidx.compose.ui.graphics.drawscope.withTransform
import androidx.compose.ui.graphics.toArgb
import androidx.compose.ui.input.pointer.pointerInput
import androidx.compose.ui.layout.onSizeChanged
import androidx.compose.ui.platform.LocalContext
import androidx.compose.ui.platform.LocalDensity
import androidx.compose.ui.platform.LocalFontFamilyResolver
import androidx.compose.ui.platform.testTag
import androidx.compose.ui.semantics.contentDescription
import androidx.compose.ui.semantics.semantics
import androidx.compose.ui.text.TextRange
import androidx.compose.ui.text.TextMeasurer
import androidx.compose.ui.text.input.TextFieldValue
import androidx.compose.ui.text.rememberTextMeasurer
import androidx.compose.ui.unit.Density
import androidx.compose.ui.unit.IntSize
import androidx.compose.ui.unit.LayoutDirection
import androidx.compose.ui.unit.dp
import androidx.compose.ui.unit.sp
import kotlinx.coroutines.Dispatchers
import kotlinx.coroutines.launch
import kotlinx.coroutines.withContext
import kotlin.math.min
import kotlin.math.roundToInt
import kotlin.math.sqrt

@Composable
fun DiagramView(
    code: String,
    onCodeChanged: (String) -> Unit = {},
    isNassiShneiderman: Boolean = false,
    onDiagramTypeChanged: (Boolean) -> Unit = {},
    profile: PSeIntProfile = PSeIntProfile.Flexible
) {
    val context = LocalContext.current
    val scope = rememberCoroutineScope()
    val snackbar = remember { SnackbarHostState() }
    var dark by rememberSaveable { mutableStateOf(false) }
    var toolbox by remember { mutableStateOf(false) }
    var viewport by remember { mutableStateOf(IntSize.Zero) }
    var zoom by remember { mutableFloatStateOf(1f) }
    var pan by remember { mutableStateOf(Offset.Zero) }
    var editing by remember { mutableStateOf<DiagramHitTarget?>(null) }
    var editingText by remember { mutableStateOf("") }
    var exportInProgress by remember { mutableStateOf(false) }
    var pendingExportCode by rememberSaveable { mutableStateOf<String?>(null) }
    var pendingExportDark by rememberSaveable { mutableStateOf(false) }
    var pendingExportNassi by rememberSaveable { mutableStateOf(false) }
    var pendingExportAlternativeIo by rememberSaveable { mutableStateOf(false) }
    val displayDensity = LocalDensity.current.density
    val fontResolver = LocalFontFamilyResolver.current
    val measurer = remember(fontResolver) { TextMeasurer(fontResolver, Density(1f, 1f), LayoutDirection.Ltr, 256) }
    val ast = remember(code) { DiagramParser.parse(code.lines()) }
    val layout = remember(ast, dark, isNassiShneiderman, measurer, profile.alternativeIoShapes) {
        DiagramRenderer.layoutDiagram(ast, measurer, dark, isNassiShneiderman, profile.alternativeIoShapes)
    }
    val background = if (dark) Color(0xFF333333) else Color(0xFFFAFAFA)
    fun fit() {
        if (viewport.width == 0 || viewport.height == 0) return
        val padding = 24f * displayDensity
        zoom = min((viewport.width - padding).coerceAtLeast(1f) / layout.width.coerceAtLeast(1f),
            (viewport.height - padding).coerceAtLeast(1f) / layout.height.coerceAtLeast(1f)).coerceAtMost(displayDensity * 1.5f).coerceAtLeast(.01f)
        pan = Offset((viewport.width - layout.width * zoom) / 2, (viewport.height - layout.height * zoom) / 2)
    }
    fun setZoom(target: Float) {
        val newZoom = target.coerceIn(.01f, displayDensity * 6f)
        val center = Offset(viewport.width / 2f, viewport.height / 2f)
        pan = center - (center - pan) * (newZoom / zoom)
        zoom = newZoom
    }
    LaunchedEffect(viewport, layout.width, layout.height, isNassiShneiderman) { fit() }

    val export = rememberLauncherForActivityResult(ActivityResultContracts.CreateDocument("image/png")) { uri ->
        val snapshotCode = pendingExportCode
        pendingExportCode = null
        if (uri != null && snapshotCode != null) scope.launch {
            exportInProgress = true
            var bitmap: Bitmap? = null
            try {
                val diagram = DiagramRenderer.layoutDiagram(DiagramParser.parse(snapshotCode.lines()), measurer, pendingExportDark, pendingExportNassi, pendingExportAlternativeIo)
                val exportScale = min(2f, min(8192f / maxOf(diagram.width, diagram.height), sqrt(16_000_000f / (diagram.width * diagram.height)))).coerceAtLeast(.001f)
                val width = (diagram.width * exportScale).roundToInt().coerceAtLeast(1)
                val height = (diagram.height * exportScale).roundToInt().coerceAtLeast(1)
                val rendered = Bitmap.createBitmap(width, height, Bitmap.Config.ARGB_8888)
                bitmap = rendered
                val canvas = android.graphics.Canvas(rendered)
                canvas.drawColor((if (pendingExportDark) Color(0xFF333333) else Color(0xFFFAFAFA)).toArgb())
                CanvasDrawScope().draw(Density(1f), LayoutDirection.Ltr, androidx.compose.ui.graphics.Canvas(canvas), Size(width.toFloat(), height.toFloat())) {
                    withTransform({ scale(exportScale, exportScale, Offset.Zero) }) { diagram.draw(this) }
                }
                withContext(Dispatchers.IO) {
                    val stream = context.contentResolver.openOutputStream(uri) ?: error("No se pudo abrir el destino")
                    stream.use { check(rendered.compress(Bitmap.CompressFormat.PNG, 100, it)) { "No se pudo crear la imagen" } }
                }
                snackbar.showSnackbar("Diagrama guardado como PNG")
            } catch (e: Exception) {
                snackbar.showSnackbar("No se pudo exportar: ${e.message}")
            } finally { bitmap?.recycle(); exportInProgress = false }
        }
    }

    Column(Modifier.fillMaxSize()) {
        Row(Modifier.fillMaxWidth().horizontalScroll(rememberScrollState()).padding(horizontal = 6.dp), verticalAlignment = Alignment.CenterVertically) {
            TextButton(onClick = { onDiagramTypeChanged(!isNassiShneiderman) }) {
                Text(if (isNassiShneiderman) "Nassi–Shneiderman" else "Clásico", fontSize = 12.sp)
                Icon(Icons.Default.SwapHoriz, "Cambiar tipo de diagrama", Modifier.size(18.dp))
            }
            VerticalDivider(Modifier.height(22.dp))
            TextButton(onClick = ::fit) { Icon(Icons.Default.FitScreen, null, Modifier.size(18.dp)); Text(" Ajustar", fontSize = 12.sp) }
            IconButton(onClick = { setZoom(zoom / 1.25f) }) { Icon(Icons.Default.Remove, "Alejar") }
            Text("${(zoom / displayDensity * 100).roundToInt()}%", fontSize = 11.sp)
            IconButton(onClick = { setZoom(zoom * 1.25f) }) { Icon(Icons.Default.Add, "Ampliar") }
            IconButton(onClick = { dark = !dark }) { Icon(if (dark) Icons.Default.LightMode else Icons.Default.DarkMode, "Cambiar fondo del diagrama") }
            IconButton(onClick = { toolbox = !toolbox }) { Icon(Icons.Default.Widgets, "Insertar figura") }
            IconButton(enabled = !exportInProgress, onClick = {
                pendingExportCode = code; pendingExportDark = dark; pendingExportNassi = isNassiShneiderman
                pendingExportAlternativeIo = profile.alternativeIoShapes
                export.launch("Diagrama_${if (isNassiShneiderman) "Nassi" else "Clasico"}.png")
            }) { Icon(Icons.Default.FileDownload, "Exportar PNG") }
        }
        HorizontalDivider()
        Box(Modifier.weight(1f).fillMaxWidth().background(background).clipToBounds()) {
            Canvas(Modifier.fillMaxSize().testTag("diagram-canvas").onSizeChanged { viewport = it }
                .semantics { contentDescription = "Diagrama ${if (isNassiShneiderman) "Nassi Shneiderman" else "clásico"}. Arrastra para desplazarte y pellizca para ampliar." }
                .pointerInput(layout) {
                    detectTransformGestures { centroid, drag, factor, _ ->
                        val target = (zoom * factor).coerceIn(.01f, displayDensity * 6f)
                        pan = centroid - (centroid - pan) * (target / zoom) + drag
                        zoom = target
                    }
                }
                .pointerInput(layout) {
                    detectTapGestures(onDoubleTap = { fit() }, onTap = { point ->
                        layout.hitTest((point - pan) / zoom)?.let { hit ->
                            if (hit.lineIndex in code.lines().indices) {
                                editing = hit
                                editingText = code.lines()[hit.lineIndex].trim()
                            }
                        }
                    })
                }) {
                withTransform({ translate(pan.x, pan.y); scale(zoom, zoom, Offset.Zero) }) {
                    layout.draw(this)
                    editing?.let { drawRect(Color(0xFF2684DB), it.bounds.topLeft, it.bounds.size, style = Stroke(2f / zoom)) }
                }
            }
            SnackbarHost(snackbar, Modifier.align(Alignment.BottomCenter))
            if (exportInProgress) CircularProgressIndicator(Modifier.align(Alignment.Center))
        }
        HorizontalDivider()
        Row(Modifier.fillMaxWidth().padding(horizontal = 12.dp, vertical = 7.dp), verticalAlignment = Alignment.CenterVertically) {
            Text("PSDraw", fontSize = 11.sp, color = MaterialTheme.colorScheme.primary)
            Spacer(Modifier.width(10.dp))
            Text("Toca una figura para editar · Pellizca para ampliar", fontSize = 10.sp, color = MaterialTheme.colorScheme.onSurfaceVariant)
        }
        if (toolbox) CommandPalette(profile) { command ->
            val index = Regex("(?im)^\\s*Fin(?:Algoritmo|Proceso)\\b").find(code)?.range?.first ?: code.length
            onCodeChanged(insertEditorCommand(TextFieldValue(code, TextRange(index)), command).text)
        }
    }
    if (editing != null) AlertDialog(
        onDismissRequest = { editing = null }, title = { Text("Editar instrucción · línea ${editing!!.lineIndex + 1}") },
        text = { OutlinedTextField(editingText, { editingText = it }, label = { Text("Pseudocódigo") }, modifier = Modifier.fillMaxWidth(), singleLine = true) },
        confirmButton = { TextButton(enabled = editingText.isNotBlank(), onClick = {
            val target = editing!!
            val lines = code.lines().toMutableList()
            if (target.lineIndex in lines.indices) {
                val indent = lines[target.lineIndex].takeWhile { it.isWhitespace() }
                lines[target.lineIndex] = indent + editingText.trim()
                onCodeChanged(lines.joinToString("\n"))
            }
            editing = null
        }) { Text("Aplicar") } },
        dismissButton = { TextButton(onClick = { editing = null }) { Text("Cancelar") } }
    )
}
