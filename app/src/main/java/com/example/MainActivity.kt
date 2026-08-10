package com.example

import android.content.Context
import android.content.Intent
import android.net.Uri
import android.os.Bundle
import android.widget.Toast
import androidx.activity.ComponentActivity
import androidx.activity.compose.rememberLauncherForActivityResult
import androidx.activity.compose.setContent
import androidx.activity.result.contract.ActivityResultContracts
import androidx.activity.enableEdgeToEdge
import androidx.compose.foundation.Image
import androidx.compose.foundation.background
import androidx.compose.foundation.border
import androidx.compose.foundation.clickable
import androidx.compose.foundation.horizontalScroll
import androidx.compose.foundation.layout.*
import androidx.compose.foundation.lazy.LazyColumn
import androidx.compose.foundation.lazy.items
import androidx.compose.foundation.rememberScrollState
import androidx.compose.foundation.shape.CircleShape
import androidx.compose.foundation.shape.RoundedCornerShape
import androidx.compose.foundation.text.BasicTextField
import androidx.compose.foundation.verticalScroll
import androidx.compose.material.icons.Icons
import androidx.compose.material.icons.filled.*
import androidx.compose.material.icons.outlined.*
import androidx.compose.material3.*
import androidx.compose.runtime.*
import androidx.compose.ui.Alignment
import androidx.compose.ui.Modifier
import androidx.compose.ui.draw.clip
import androidx.compose.ui.graphics.Color
import androidx.compose.ui.graphics.SolidColor
import androidx.compose.ui.graphics.vector.ImageVector
import androidx.compose.ui.platform.LocalClipboardManager
import androidx.compose.ui.platform.LocalContext
import androidx.compose.ui.res.painterResource
import androidx.compose.ui.text.AnnotatedString
import androidx.compose.ui.text.TextStyle
import androidx.compose.ui.text.font.FontFamily
import androidx.compose.ui.text.font.FontWeight
import androidx.compose.ui.text.style.TextAlign
import androidx.compose.ui.unit.dp
import androidx.compose.ui.unit.sp
import com.example.ui.theme.*
import kotlinx.coroutines.launch
import java.io.BufferedReader
import java.io.InputStreamReader

class MainActivity : ComponentActivity() {
    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        enableEdgeToEdge()
        setContent {
            MyApplicationTheme {
                PSeIntApp()
            }
        }
    }
}

enum class AppTab { Editor, Diagram, Terminal, Settings }
enum class AppTheme { Dark, LightClassic, HackerMatrix, OceanBlue }
enum class DiagramType { Classic, NassiShneiderman }

@OptIn(ExperimentalMaterial3Api::class)
@Composable
fun PSeIntApp() {
    val context = LocalContext.current
    val prefs = remember { context.getSharedPreferences("pseint_prefs", Context.MODE_PRIVATE) }
    var showOnboarding by remember { mutableStateOf(prefs.getBoolean("first_run", true)) }

    // Persistent State Setup
    var selectedTheme by remember {
        mutableStateOf(AppTheme.valueOf(prefs.getString("selected_theme", AppTheme.Dark.name) ?: AppTheme.Dark.name))
    }
    var fontSizeSp by remember { mutableStateOf(prefs.getInt("font_size", 14)) }
    var showLineNumbers by remember { mutableStateOf(prefs.getBoolean("show_line_numbers", true)) }
    var diagramType by remember {
        mutableStateOf(DiagramType.valueOf(prefs.getString("diagram_type", DiagramType.Classic.name) ?: DiagramType.Classic.name))
    }

    var code by remember {
        mutableStateOf(
            """Algoritmo HolaMundo
    Escribir "¡Hola Mundo!"
FinAlgoritmo"""
        )
    }

    var output by remember { mutableStateOf(listOf<String>()) }
    var isRunning by remember { mutableStateOf(false) }
    var isDebugRunning by remember { mutableStateOf(false) }
    var currentExecutingLine by remember { mutableStateOf(-1) }

    var awaitingInput by remember { mutableStateOf(false) }
    var inputPromptVar by remember { mutableStateOf("") }
    var currentInput by remember { mutableStateOf("") }

    var currentTab by remember { mutableStateOf(AppTab.Editor) }
    var showExportDialog by remember { mutableStateOf(false) }
    var showProfilePickerDialog by remember { mutableStateOf(false) }
    var showEditCustomProfileDialog by remember { mutableStateOf(false) }
    var showOperatorsDrawer by remember { mutableStateOf(false) }

    val coroutineScope = rememberCoroutineScope()
    val evaluator = remember { PSeIntEvaluator() }

    var customProfileObj by remember { mutableStateOf(PSeIntProfile.loadCustomProfile(context)) }
    val savedProfileName = prefs.getString("selected_profile", "Flexible") ?: "Flexible"

    var selectedProfile by remember {
        mutableStateOf(
            if (savedProfileName == "Personalizado") customProfileObj
            else PSeIntProfile(name = savedProfileName, description = "Perfil activo $savedProfileName")
        )
    }

    val clipboardManager = LocalClipboardManager.current

    // File Picker Launcher for Opening .psc files
    val filePickerLauncher = rememberLauncherForActivityResult(
        contract = ActivityResultContracts.GetContent()
    ) { uri: Uri? ->
        uri?.let {
            try {
                val inputStream = context.contentResolver.openInputStream(it)
                val reader = BufferedReader(InputStreamReader(inputStream))
                val sb = StringBuilder()
                var line: String?
                while (reader.readLine().also { l -> line = l } != null) {
                    sb.append(line).append("\n")
                }
                code = sb.toString()
                Toast.makeText(context, "Archivo .psc cargado con éxito", Toast.LENGTH_SHORT).show()
            } catch (e: Exception) {
                Toast.makeText(context, "Error al abrir el archivo: ${e.message}", Toast.LENGTH_LONG).show()
            }
        }
    }

    // Dynamic Theme Colors (Full Contrast Fix)
    val (editorBgColor, editorTextColor, lineNumBg, surfaceColor, primaryText, secondaryText) = when (selectedTheme) {
        AppTheme.Dark -> Sextuple(Color(0xFF1E1E28), Color(0xFFE0E0E0), Color(0xFF14141D), Color(0xFF14141D), Color.White, Color(0xFFA0A0B5))
        AppTheme.LightClassic -> Sextuple(Color(0xFFFFFFFF), Color(0xFF111111), Color(0xFFEEEEEE), Color(0xFFF5F5F5), Color(0xFF111111), Color(0xFF666666))
        AppTheme.HackerMatrix -> Sextuple(Color(0xFF030A04), Color(0xFF00FF66), Color(0xFF061408), Color(0xFF020703), Color(0xFF00FF66), Color(0xFF00AA44))
        AppTheme.OceanBlue -> Sextuple(Color(0xFF0D1B2A), Color(0xFFE0F2FE), Color(0xFF1B2A4A), Color(0xFF08101E), Color(0xFFE0F2FE), Color(0xFF7DD3FC))
    }

    // Syntax Status Result
    val syntaxResult = remember(code, selectedProfile) { evaluator.checkSyntax(code, selectedProfile) }

    val insertCommand: (String) -> Unit = { cmd ->
        code = "$code\n$cmd"
    }

    if (showOnboarding) {
        OnboardingWizard(
            selectedProfile = selectedProfile,
            onProfileSelected = {
                selectedProfile = it
                prefs.edit().putString("selected_profile", it.name).apply()
            },
            selectedTheme = selectedTheme,
            onThemeSelected = {
                selectedTheme = it
                prefs.edit().putString("selected_theme", it.name).apply()
            },
            onFinishOnboarding = {
                prefs.edit().putBoolean("first_run", false).apply()
                showOnboarding = false
            }
        )
    } else {
        Scaffold(
            containerColor = surfaceColor,
            bottomBar = {
                Column {
                    Divider(color = Color(0xFF333344))
                    Row(
                        modifier = Modifier
                            .fillMaxWidth()
                            .height(60.dp)
                            .background(surfaceColor)
                            .padding(horizontal = 16.dp),
                        horizontalArrangement = Arrangement.SpaceAround,
                        verticalAlignment = Alignment.CenterVertically
                    ) {
                        BottomNavButton("Editor", Icons.Outlined.Edit, currentTab == AppTab.Editor, primaryText, secondaryText) { currentTab = AppTab.Editor }
                        BottomNavButton("Diagrama", Icons.Outlined.AccountTree, currentTab == AppTab.Diagram, primaryText, secondaryText) { currentTab = AppTab.Diagram }
                        BottomNavButton("Terminal", Icons.Outlined.Terminal, currentTab == AppTab.Terminal, primaryText, secondaryText) { currentTab = AppTab.Terminal }
                        BottomNavButton("Ajustes", Icons.Outlined.Settings, currentTab == AppTab.Settings, primaryText, secondaryText) { currentTab = AppTab.Settings }
                    }
                }
            }
        ) { innerPadding ->
            Column(
                modifier = Modifier
                    .fillMaxSize()
                    .padding(innerPadding)
            ) {
                // Top Action Header Bar
                Row(
                    modifier = Modifier
                        .fillMaxWidth()
                        .height(56.dp)
                        .background(surfaceColor)
                        .padding(horizontal = 16.dp),
                    verticalAlignment = Alignment.CenterVertically,
                    horizontalArrangement = Arrangement.SpaceBetween
                ) {
                    Row(verticalAlignment = Alignment.CenterVertically, horizontalArrangement = Arrangement.spacedBy(10.dp)) {
                        Image(
                            painter = painterResource(id = R.drawable.logo_pseint),
                            contentDescription = "PSeInt Logo",
                            modifier = Modifier.height(30.dp)
                        )
                        Column {
                            Text("PSeInt Mobile", fontSize = 15.sp, fontWeight = FontWeight.Bold, color = primaryText)
                            Text("PERFIL: ${selectedProfile.name.uppercase()}", fontSize = 9.sp, fontWeight = FontWeight.Bold, color = Color(0xFF4CAF50), letterSpacing = 0.5.sp)
                        }
                    }
                    Row(verticalAlignment = Alignment.CenterVertically, horizontalArrangement = Arrangement.spacedBy(6.dp)) {
                        // Paso a Paso (Debug) Button
                        IconButton(
                            onClick = {
                                if (isRunning || isDebugRunning) {
                                    isRunning = false
                                    isDebugRunning = false
                                    awaitingInput = false
                                    currentExecutingLine = -1
                                } else {
                                    currentTab = AppTab.Terminal
                                    output = listOf("*** Ejecución Paso a Paso Iniciada (${selectedProfile.name}) ***")
                                    isDebugRunning = true
                                    coroutineScope.launch {
                                        evaluator.evaluate(
                                            code = code,
                                            profile = selectedProfile,
                                            isStepByStep = true,
                                            onStep = { lineNum -> currentExecutingLine = lineNum },
                                            onOutput = { msg -> output = output + msg },
                                            onRequestInput = { varName ->
                                                awaitingInput = true
                                                inputPromptVar = varName
                                                while (awaitingInput && isDebugRunning) { kotlinx.coroutines.delay(100) }
                                                val res = currentInput
                                                currentInput = ""
                                                output = output + "> $res"
                                                res
                                            },
                                            onFinish = {
                                                isDebugRunning = false
                                                awaitingInput = false
                                                currentExecutingLine = -1
                                                output = output + "*** Ejecución Paso a Paso Finalizada. ***"
                                            }
                                        )
                                    }
                                }
                            },
                            modifier = Modifier
                                .size(36.dp)
                                .background(Color(0xFFFFF176), CircleShape)
                        ) {
                            Icon(Icons.Filled.DirectionsWalk, contentDescription = "Paso a Paso", tint = Color.Black)
                        }

                        // Normal Run Button
                        IconButton(
                            onClick = {
                                if (isRunning || isDebugRunning) {
                                    isRunning = false
                                    isDebugRunning = false
                                    awaitingInput = false
                                } else {
                                    currentTab = AppTab.Terminal
                                    output = listOf("*** Ejecución Iniciada (${selectedProfile.name}) ***")
                                    isRunning = true
                                    coroutineScope.launch {
                                        evaluator.evaluate(
                                            code = code,
                                            profile = selectedProfile,
                                            onOutput = { msg -> output = output + msg },
                                            onRequestInput = { varName ->
                                                awaitingInput = true
                                                inputPromptVar = varName
                                                while (awaitingInput && isRunning) { kotlinx.coroutines.delay(100) }
                                                val res = currentInput
                                                currentInput = ""
                                                output = output + "> $res"
                                                res
                                            },
                                            onFinish = {
                                                isRunning = false
                                                awaitingInput = false
                                                output = output + "*** Ejecución Finalizada. ***"
                                            }
                                        )
                                    }
                                }
                            },
                            modifier = Modifier
                                .size(36.dp)
                                .background(if (isRunning || isDebugRunning) Color(0xFFFF5252) else Color(0xFF4CAF50), CircleShape)
                        ) {
                            Icon(
                                imageVector = if (isRunning || isDebugRunning) Icons.Filled.Stop else Icons.Filled.PlayArrow,
                                contentDescription = "Ejecutar",
                                tint = Color.White
                            )
                        }
                    }
                }
                Divider(color = Color(0xFF333344))

                if (currentTab == AppTab.Editor) {
                    // Toolbar Chip Controls
                    Row(
                        modifier = Modifier
                            .fillMaxWidth()
                            .background(surfaceColor)
                            .horizontalScroll(rememberScrollState())
                            .padding(horizontal = 10.dp, vertical = 5.dp),
                        horizontalArrangement = Arrangement.spacedBy(6.dp)
                    ) {
                        PrimaryActionChip("Nuevo", Icons.Outlined.Description) {
                            code = "Algoritmo SinTitulo\n\nFinAlgoritmo"
                        }
                        SecondaryActionChip("Abrir", Icons.Outlined.FolderOpen, primaryText) {
                            filePickerLauncher.launch("*/*")
                        }
                        SecondaryActionChip("Operadores", Icons.Outlined.Functions, primaryText) {
                            showOperatorsDrawer = !showOperatorsDrawer
                        }
                        SecondaryActionChip("Exportar", Icons.Outlined.Code, primaryText) {
                            showExportDialog = true
                        }
                        SecondaryActionChip("Compartir", Icons.Outlined.Share, primaryText) {
                            val shareIntent = Intent(Intent.ACTION_SEND).apply {
                                type = "text/plain"
                                putExtra(Intent.EXTRA_SUBJECT, "Codigo PSeInt")
                                putExtra(Intent.EXTRA_TEXT, code)
                            }
                            context.startActivity(Intent.createChooser(shareIntent, "Compartir Pseudocódigo"))
                        }
                        SecondaryActionChip("Perfil", Icons.Outlined.Tune, primaryText) {
                            showProfilePickerDialog = true
                        }
                    }
                    Divider(color = Color(0xFF333344))

                    // Code Editor Container
                    Row(
                        modifier = Modifier
                            .weight(1f)
                            .fillMaxWidth()
                    ) {
                        if (showLineNumbers) {
                            Column(
                                modifier = Modifier
                                    .width(36.dp)
                                    .fillMaxHeight()
                                    .background(lineNumBg)
                                    .border(1.dp, Color(0xFF333344))
                                    .padding(top = 12.dp),
                                horizontalAlignment = Alignment.CenterHorizontally
                            ) {
                                val lineCount = code.count { it == '\n' } + 1
                                for (i in 1..lineCount) {
                                    val isCurrentDebugLine = (i == currentExecutingLine)
                                    Text(
                                        text = i.toString(),
                                        color = if (isCurrentDebugLine) Color(0xFFFFD54F) else LineNumberColor,
                                        fontWeight = if (isCurrentDebugLine) FontWeight.Bold else FontWeight.Normal,
                                        fontSize = (fontSizeSp - 3).sp,
                                        fontFamily = FontFamily.Monospace,
                                        modifier = Modifier.padding(bottom = 3.dp)
                                    )
                                }
                            }
                        }

                        val scrollState = rememberScrollState()
                        BasicTextField(
                            value = code,
                            onValueChange = { code = it },
                            modifier = Modifier
                                .fillMaxSize()
                                .background(editorBgColor)
                                .padding(12.dp)
                                .verticalScroll(scrollState),
                            textStyle = TextStyle(
                                color = editorTextColor,
                                fontSize = fontSizeSp.sp,
                                fontFamily = FontFamily.Monospace,
                                lineHeight = (fontSizeSp + 6).sp
                            ),
                            cursorBrush = SolidColor(editorTextColor),
                            visualTransformation = PSeIntSyntaxHighlighter()
                        )
                    }

                    // Desktop-Style Status Bar (Syntax Status Display)
                    Row(
                        modifier = Modifier
                            .fillMaxWidth()
                            .background(if (syntaxResult.isValid) Color(0xFF1B4332) else Color(0xFF4A0E17))
                            .padding(horizontal = 12.dp, vertical = 6.dp),
                        verticalAlignment = Alignment.CenterVertically
                    ) {
                        Icon(
                            imageVector = if (syntaxResult.isValid) Icons.Filled.CheckCircle else Icons.Filled.Error,
                            contentDescription = null,
                            tint = if (syntaxResult.isValid) Color(0xFF52B788) else Color(0xFFFF6B6B),
                            modifier = Modifier.size(16.dp)
                        )
                        Spacer(Modifier.width(8.dp))
                        Text(
                            text = syntaxResult.errorMessage,
                            color = Color.White,
                            fontSize = 11.sp,
                            fontFamily = FontFamily.Monospace
                        )
                    }

                    Divider(color = Color(0xFF333344))
                    // Quick Commands Palette with Original Desktop Assets
                    Column(
                        modifier = Modifier
                            .fillMaxWidth()
                            .background(lineNumBg)
                            .padding(6.dp)
                    ) {
                        Row(
                            modifier = Modifier.fillMaxWidth().horizontalScroll(rememberScrollState()),
                            horizontalArrangement = Arrangement.spacedBy(6.dp)
                        ) {
                            PSeIntCommandAssetButton("Escribir", R.drawable.escribir, primaryText) { insertCommand("Escribir \"\"") }
                            PSeIntCommandAssetButton("Leer", R.drawable.leer, primaryText) { insertCommand("Leer variable") }
                            PSeIntCommandAssetButton("Asignar", R.drawable.asignar, primaryText) { insertCommand("variable <- expresion") }
                            PSeIntCommandAssetButton("Definir", R.drawable.asignar, primaryText) { insertCommand("Definir var Como Entero") }
                            PSeIntCommandAssetButton("Dimension", R.drawable.asignar, primaryText) { insertCommand("Dimension arreglo[10]") }
                            PSeIntCommandAssetButton("Si-Entonces", R.drawable.si, primaryText) { insertCommand("Si condicion Entonces\n\t// acciones\nFinSi") }
                            PSeIntCommandAssetButton("Según", R.drawable.segun, primaryText) { insertCommand("Segun variable Hacer\n\topcion1:\n\t\t// acciones\n\tDe Otro Modo:\n\t\t// acciones\nFinSegun") }
                            PSeIntCommandAssetButton("Mientras", R.drawable.mientras, primaryText) { insertCommand("Mientras condicion Hacer\n\t// acciones\nFinMientras") }
                            PSeIntCommandAssetButton("Repetir", R.drawable.repetir, primaryText) { insertCommand("Repetir\n\t// acciones\nHasta Que condicion") }
                            PSeIntCommandAssetButton("Para", R.drawable.para, primaryText) { insertCommand("Para i<-1 Hasta 10 Con Paso 1 Hacer\n\t// acciones\nFinPara") }
                            PSeIntCommandAssetButton("Función", R.drawable.funcion, primaryText) { insertCommand("Funcion res <- MiFuncion(arg)\n\tres <- arg * 2\nFinFuncion") }
                        }
                    }
                } else if (currentTab == AppTab.Terminal) {
                    Column(
                        modifier = Modifier
                            .fillMaxSize()
                            .background(Color(0xFF0F0F14))
                            .padding(16.dp)
                    ) {
                        Text("TERMINAL DE EJECUCIÓN (CONSOLA PSeINT)", color = Color(0xFF4CAF50), fontFamily = FontFamily.Monospace, fontSize = 12.sp, fontWeight = FontWeight.Bold, modifier = Modifier.padding(bottom = 8.dp))
                        val terminalScroll = rememberScrollState()
                        LaunchedEffect(output.size) { terminalScroll.animateScrollTo(terminalScroll.maxValue) }

                        Column(
                            modifier = Modifier
                                .weight(1f)
                                .fillMaxWidth()
                                .verticalScroll(terminalScroll)
                        ) {
                            output.forEach { line ->
                                Text(line, color = if (line.startsWith("Error")) Color(0xFFFF5252) else Color(0xFFE0E0E0), fontFamily = FontFamily.Monospace, fontSize = 14.sp)
                            }
                            if (awaitingInput) {
                                Spacer(modifier = Modifier.height(8.dp))
                                Text("Ingrese valor para [$inputPromptVar]:", color = Color(0xFFFFC107), fontFamily = FontFamily.Monospace, fontSize = 12.sp)
                                OutlinedTextField(
                                    value = currentInput,
                                    onValueChange = { currentInput = it },
                                    modifier = Modifier.fillMaxWidth().padding(top = 4.dp),
                                    placeholder = { Text("Valor...", color = Color.Gray) },
                                    textStyle = TextStyle(color = Color.White, fontFamily = FontFamily.Monospace),
                                    trailingIcon = {
                                        IconButton(onClick = { awaitingInput = false }) {
                                            Icon(Icons.Filled.Send, contentDescription = "Enviar", tint = Color(0xFF4CAF50))
                                        }
                                    },
                                    singleLine = true,
                                    colors = OutlinedTextFieldDefaults.colors(
                                        focusedBorderColor = Color(0xFF4CAF50),
                                        unfocusedBorderColor = Color.DarkGray
                                    )
                                )
                            }
                        }
                    }
                } else if (currentTab == AppTab.Diagram) {
                    DiagramView(code = code, isNassiShneiderman = (diagramType == DiagramType.NassiShneiderman))
                } else if (currentTab == AppTab.Settings) {
                    SettingsView(
                        selectedProfile = selectedProfile,
                        onOpenProfilePicker = { showProfilePickerDialog = true },
                        onEditCustomProfile = { showEditCustomProfileDialog = true },
                        fontSizeSp = fontSizeSp,
                        onFontSizeChanged = {
                            fontSizeSp = it
                            prefs.edit().putInt("font_size", it).apply()
                        },
                        selectedTheme = selectedTheme,
                        onThemeSelected = {
                            selectedTheme = it
                            prefs.edit().putString("selected_theme", it.name).apply()
                        },
                        diagramType = diagramType,
                        onDiagramTypeSelected = {
                            diagramType = it
                            prefs.edit().putString("diagram_type", it.name).apply()
                        },
                        showLineNumbers = showLineNumbers,
                        onShowLineNumbersToggled = {
                            showLineNumbers = it
                            prefs.edit().putBoolean("show_line_numbers", it).apply()
                        },
                        onReopenWizard = { showOnboarding = true },
                        primaryText = primaryText
                    )
                }

                // Operators & Functions Drawer (Exact PSeInt Desktop Screenshot Match)
                if (showOperatorsDrawer) {
                    OperatorsAndFunctionsDialog(
                        onInsert = { symbol ->
                            code = "$code $symbol"
                            showOperatorsDrawer = false
                        },
                        onDismiss = { showOperatorsDrawer = false }
                    )
                }

                // Export Dialog
                if (showExportDialog) {
                    ExportCodeDialog(
                        code = code,
                        onDismiss = { showExportDialog = false },
                        onCopyCode = { exported ->
                            clipboardManager.setText(AnnotatedString(exported))
                            Toast.makeText(context, "Código copiado al portapapeles", Toast.LENGTH_SHORT).show()
                            showExportDialog = false
                        }
                    )
                }

                // Full Profiles Searchable Dialog
                if (showProfilePickerDialog) {
                    ProfilePickerDialog(
                        selectedProfileName = selectedProfile.name,
                        onSelectProfileName = { name ->
                            if (name == "Personalizado") {
                                selectedProfile = customProfileObj
                            } else {
                                selectedProfile = PSeIntProfile(name = name, description = "Perfil seleccionado: $name")
                            }
                            prefs.edit().putString("selected_profile", name).apply()
                            showProfilePickerDialog = false
                        },
                        onDismiss = { showProfilePickerDialog = false }
                    )
                }

                // Edit Custom Profile Dialog
                if (showEditCustomProfileDialog) {
                    EditCustomProfileDialog(
                        customProfile = customProfileObj,
                        onSave = { updated ->
                            customProfileObj = updated
                            selectedProfile = updated
                            PSeIntProfile.saveCustomProfile(context, updated)
                            prefs.edit().putString("selected_profile", "Personalizado").apply()
                            showEditCustomProfileDialog = false
                            Toast.makeText(context, "Perfil Personalizado guardado", Toast.LENGTH_SHORT).show()
                        },
                        onDismiss = { showEditCustomProfileDialog = false }
                    )
                }
            }
        }
    }
}

@Composable
fun OperatorsAndFunctionsDialog(
    onInsert: (String) -> Unit,
    onDismiss: () -> Unit
) {
    AlertDialog(
        onDismissRequest = onDismiss,
        title = { Text("Operadores y Funciones PSeInt", fontWeight = FontWeight.Bold) },
        text = {
            Column(
                modifier = Modifier
                    .fillMaxWidth()
                    .height(340.dp)
                    .verticalScroll(rememberScrollState())
            ) {
                Text("Algebraicos", color = Color(0xFF4CAF50), fontSize = 12.sp, fontWeight = FontWeight.Bold)
                Row(horizontalArrangement = Arrangement.spacedBy(4.dp)) {
                    Button(onClick = { onInsert("+") }, modifier = Modifier.weight(1f)) { Text("+ (suma)") }
                    Button(onClick = { onInsert("-") }, modifier = Modifier.weight(1f)) { Text("- (resta)") }
                    Button(onClick = { onInsert("*") }, modifier = Modifier.weight(1f)) { Text("* (mult)") }
                }
                Row(horizontalArrangement = Arrangement.spacedBy(4.dp)) {
                    Button(onClick = { onInsert("/") }, modifier = Modifier.weight(1f)) { Text("/ (div)") }
                    Button(onClick = { onInsert("^") }, modifier = Modifier.weight(1f)) { Text("^ (pot)") }
                    Button(onClick = { onInsert("%") }, modifier = Modifier.weight(1f)) { Text("% (mod)") }
                }
                Spacer(Modifier.height(10.dp))
                Text("Lógicos y Relacionales", color = Color(0xFF4CAF50), fontSize = 12.sp, fontWeight = FontWeight.Bold)
                Row(horizontalArrangement = Arrangement.spacedBy(4.dp)) {
                    Button(onClick = { onInsert("==") }, modifier = Modifier.weight(1f)) { Text("= (igual)") }
                    Button(onClick = { onInsert("<>") }, modifier = Modifier.weight(1f)) { Text("<> (distinto)") }
                    Button(onClick = { onInsert("<") }, modifier = Modifier.weight(1f)) { Text("< (menor)") }
                }
                Row(horizontalArrangement = Arrangement.spacedBy(4.dp)) {
                    Button(onClick = { onInsert(">") }, modifier = Modifier.weight(1f)) { Text("> (mayor)") }
                    Button(onClick = { onInsert("&") }, modifier = Modifier.weight(1f)) { Text("& (Y)") }
                    Button(onClick = { onInsert("|") }, modifier = Modifier.weight(1f)) { Text("| (O)") }
                }
                Spacer(Modifier.height(10.dp))
                Text("Funciones Matemáticas", color = Color(0xFF4CAF50), fontSize = 12.sp, fontWeight = FontWeight.Bold)
                Row(horizontalArrangement = Arrangement.spacedBy(4.dp)) {
                    Button(onClick = { onInsert("rc()") }, modifier = Modifier.weight(1f)) { Text("rc(x)") }
                    Button(onClick = { onInsert("abs()") }, modifier = Modifier.weight(1f)) { Text("abs(x)") }
                    Button(onClick = { onInsert("trunc()") }, modifier = Modifier.weight(1f)) { Text("trunc(x)") }
                }
                Row(horizontalArrangement = Arrangement.spacedBy(4.dp)) {
                    Button(onClick = { onInsert("redon()") }, modifier = Modifier.weight(1f)) { Text("redon(x)") }
                    Button(onClick = { onInsert("azar()") }, modifier = Modifier.weight(1f)) { Text("azar(x)") }
                }
            }
        },
        confirmButton = {
            TextButton(onClick = onDismiss) { Text("Cerrar") }
        }
    )
}

@Composable
fun ProfilePickerDialog(
    selectedProfileName: String,
    onSelectProfileName: (String) -> Unit,
    onDismiss: () -> Unit
) {
    var searchQuery by remember { mutableStateOf("") }
    val filteredProfiles = remember(searchQuery) {
        if (searchQuery.isEmpty()) PSeIntProfile.PopularNames
        else PSeIntProfile.PopularNames.filter { it.contains(searchQuery, ignoreCase = true) }
    }

    AlertDialog(
        onDismissRequest = onDismiss,
        title = { Text("Opciones del Lenguaje (${PSeIntProfile.PopularNames.size} perfiles)", fontSize = 16.sp, fontWeight = FontWeight.Bold) },
        text = {
            Column(modifier = Modifier.fillMaxWidth().height(360.dp)) {
                Text("Puede buscar por nombre de la institución, materia, docente, siglas, etc.", fontSize = 11.sp, color = Color.Gray, modifier = Modifier.padding(bottom = 6.dp))
                OutlinedTextField(
                    value = searchQuery,
                    onValueChange = { searchQuery = it },
                    placeholder = { Text("Buscar: (ej. UNAM, SENA, UTN...)") },
                    leadingIcon = { Icon(Icons.Filled.Search, contentDescription = null) },
                    modifier = Modifier.fillMaxWidth().padding(bottom = 8.dp),
                    singleLine = true
                )
                LazyColumn(modifier = Modifier.weight(1f)) {
                    items(filteredProfiles) { name ->
                        Row(
                            verticalAlignment = Alignment.CenterVertically,
                            modifier = Modifier
                                .fillMaxWidth()
                                .clickable { onSelectProfileName(name) }
                                .padding(vertical = 10.dp, horizontal = 8.dp)
                        ) {
                            RadioButton(selected = selectedProfileName == name, onClick = { onSelectProfileName(name) })
                            Spacer(Modifier.width(8.dp))
                            Text(name, fontSize = 14.sp)
                        }
                        Divider(color = Color(0xFF333344))
                    }
                }
            }
        },
        confirmButton = {
            TextButton(onClick = onDismiss) { Text("Cerrar") }
        }
    )
}

@Composable
fun EditCustomProfileDialog(
    customProfile: PSeIntProfile,
    onSave: (PSeIntProfile) -> Unit,
    onDismiss: () -> Unit
) {
    var forceDefine by remember { mutableStateOf(customProfile.forceDefineVariables) }
    var allowEquals by remember { mutableStateOf(customProfile.allowEqualsAssignment) }
    var requireSemicolon by remember { mutableStateOf(customProfile.requireSemicolons) }
    var strictTypes by remember { mutableStateOf(customProfile.strictTypes) }

    AlertDialog(
        onDismissRequest = onDismiss,
        title = { Text("Editar Perfil Personalizado", fontWeight = FontWeight.Bold) },
        text = {
            Column {
                CustomToggleRow("Obligar a definir variables", forceDefine) { forceDefine = it }
                CustomToggleRow("Permitir signo = en asignación", allowEquals) { allowEquals = it }
                CustomToggleRow("Exigir punto y coma al final", requireSemicolon) { requireSemicolon = it }
                CustomToggleRow("Tipos de datos estrictos", strictTypes) { strictTypes = it }
            }
        },
        confirmButton = {
            Button(onClick = {
                onSave(customProfile.copy(
                    forceDefineVariables = forceDefine,
                    allowEqualsAssignment = allowEquals,
                    requireSemicolons = requireSemicolon,
                    strictTypes = strictTypes
                ))
            }) {
                Text("Guardar Perfil")
            }
        },
        dismissButton = {
            TextButton(onClick = onDismiss) { Text("Cancelar") }
        }
    )
}

@Composable
fun OnboardingWizard(
    selectedProfile: PSeIntProfile,
    onProfileSelected: (PSeIntProfile) -> Unit,
    selectedTheme: AppTheme,
    onThemeSelected: (AppTheme) -> Unit,
    onFinishOnboarding: () -> Unit
) {
    Column(
        modifier = Modifier
            .fillMaxSize()
            .background(Color(0xFF14141E))
            .padding(24.dp)
            .verticalScroll(rememberScrollState()),
        horizontalAlignment = Alignment.CenterHorizontally,
        verticalArrangement = Arrangement.Center
    ) {
        Image(
            painter = painterResource(id = R.drawable.logo_pseint),
            contentDescription = "PSeInt Logo",
            modifier = Modifier.height(64.dp).padding(bottom = 12.dp)
        )
        Text("¡Bienvenido a PSeInt Mobile!", fontSize = 20.sp, fontWeight = FontWeight.Bold, color = Color.White, textAlign = TextAlign.Center)
        Text("Configuración Inicial de la Aplicación", fontSize = 12.sp, color = Color(0xFFA0A0B0), modifier = Modifier.padding(bottom = 20.dp))

        Surface(
            color = Color(0xFF1E1E2C),
            shape = RoundedCornerShape(12.dp),
            modifier = Modifier.fillMaxWidth().padding(bottom = 16.dp)
        ) {
            Column(modifier = Modifier.padding(14.dp)) {
                Text("1. Perfil Inicial de Lenguaje", color = Color(0xFF4CAF50), fontSize = 13.sp, fontWeight = FontWeight.Bold, modifier = Modifier.padding(bottom = 6.dp))
                listOf("Flexible", "Estricto", "SENA (Colombia)", "UNAM (México)").forEach { name ->
                    Row(
                        verticalAlignment = Alignment.CenterVertically,
                        modifier = Modifier.fillMaxWidth().clickable { onProfileSelected(PSeIntProfile(name, "")) }.padding(vertical = 4.dp)
                    ) {
                        RadioButton(selected = selectedProfile.name == name, onClick = { onProfileSelected(PSeIntProfile(name, "")) })
                        Spacer(Modifier.width(8.dp))
                        Text(name, color = Color.White, fontSize = 13.sp)
                    }
                }
            }
        }

        Surface(
            color = Color(0xFF1E1E2C),
            shape = RoundedCornerShape(12.dp),
            modifier = Modifier.fillMaxWidth().padding(bottom = 20.dp)
        ) {
            Column(modifier = Modifier.padding(14.dp)) {
                Text("2. Tema de Apariencia Inicial", color = Color(0xFF4CAF50), fontSize = 13.sp, fontWeight = FontWeight.Bold, modifier = Modifier.padding(bottom = 6.dp))
                ThemeOptionRow("PSeInt Oscuro (Noche)", AppTheme.Dark, selectedTheme) { onThemeSelected(AppTheme.Dark) }
                ThemeOptionRow("PSeInt Clásico (Blanco Escritorio)", AppTheme.LightClassic, selectedTheme) { onThemeSelected(AppTheme.LightClassic) }
            }
        }

        Button(
            onClick = onFinishOnboarding,
            colors = ButtonDefaults.buttonColors(containerColor = Color(0xFF4CAF50)),
            shape = RoundedCornerShape(10.dp),
            modifier = Modifier.fillMaxWidth().height(48.dp)
        ) {
            Text("COMENZAR A PROGRAMAR", fontSize = 13.sp, fontWeight = FontWeight.Bold, color = Color.White)
        }
    }
}

@Composable
fun ExportCodeDialog(
    code: String,
    onDismiss: () -> Unit,
    onCopyCode: (String) -> Unit
) {
    var selectedLang by remember { mutableStateOf(ExportLanguage.Python) }
    val exportedCode = remember(code, selectedLang) { PSeIntExporter.exportCode(code, selectedLang) }

    AlertDialog(
        onDismissRequest = onDismiss,
        title = { Text("Exportar Pseudocódigo", fontWeight = FontWeight.Bold) },
        text = {
            Column(modifier = Modifier.fillMaxWidth()) {
                Text("Selecciona el lenguaje destino:", fontSize = 12.sp, color = Color.Gray, modifier = Modifier.padding(bottom = 6.dp))
                Row(
                    modifier = Modifier.fillMaxWidth().horizontalScroll(rememberScrollState()),
                    horizontalArrangement = Arrangement.spacedBy(6.dp)
                ) {
                    ExportLanguage.values().forEach { lang ->
                        FilterChip(
                            selected = selectedLang == lang,
                            onClick = { selectedLang = lang },
                            label = { Text(lang.displayName) }
                        )
                    }
                }
                Spacer(modifier = Modifier.height(10.dp))
                Surface(
                    color = Color(0xFF111118),
                    shape = RoundedCornerShape(8.dp),
                    modifier = Modifier.fillMaxWidth().height(160.dp).padding(2.dp)
                ) {
                    Text(
                        text = exportedCode,
                        color = Color(0xFF00E676),
                        fontFamily = FontFamily.Monospace,
                        fontSize = 11.sp,
                        modifier = Modifier.padding(8.dp).verticalScroll(rememberScrollState())
                    )
                }
            }
        },
        confirmButton = {
            Button(onClick = { onCopyCode(exportedCode) }) {
                Text("Copiar Código")
            }
        },
        dismissButton = {
            TextButton(onClick = onDismiss) { Text("Cancelar") }
        }
    )
}

@Composable
fun SettingsView(
    selectedProfile: PSeIntProfile,
    onOpenProfilePicker: () -> Unit,
    onEditCustomProfile: () -> Unit,
    fontSizeSp: Int,
    onFontSizeChanged: (Int) -> Unit,
    selectedTheme: AppTheme,
    onThemeSelected: (AppTheme) -> Unit,
    diagramType: DiagramType,
    onDiagramTypeSelected: (DiagramType) -> Unit,
    showLineNumbers: Boolean,
    onShowLineNumbersToggled: (Boolean) -> Unit,
    onReopenWizard: () -> Unit,
    primaryText: Color
) {
    Column(
        modifier = Modifier
            .fillMaxSize()
            .padding(16.dp)
            .verticalScroll(rememberScrollState())
    ) {
        Text("PERSONALIZACIÓN Y APARIENCIA", color = primaryText, fontSize = 18.sp, fontWeight = FontWeight.Bold, modifier = Modifier.padding(bottom = 12.dp))

        // Onboarding Re-run Button
        Button(
            onClick = onReopenWizard,
            colors = ButtonDefaults.buttonColors(containerColor = Color(0xFF1565C0)),
            shape = RoundedCornerShape(8.dp),
            modifier = Modifier.fillMaxWidth().padding(bottom = 16.dp)
        ) {
            Icon(Icons.Filled.AutoFixHigh, contentDescription = null, tint = Color.White)
            Spacer(Modifier.width(8.dp))
            Text("Reabrir Asistente de Configuración Inicial", color = Color.White, fontSize = 13.sp)
        }

        // Language Profile Section
        Text("Perfil de Lenguaje Activo", color = Color(0xFF4CAF50), fontSize = 14.sp, fontWeight = FontWeight.Bold, modifier = Modifier.padding(bottom = 6.dp))
        Surface(
            color = Color(0xFF1E1E2C),
            shape = RoundedCornerShape(12.dp),
            modifier = Modifier.fillMaxWidth().padding(bottom = 16.dp)
        ) {
            Column(modifier = Modifier.padding(16.dp)) {
                Text("Perfil actual: ${selectedProfile.name}", color = Color.White, fontSize = 15.sp, fontWeight = FontWeight.Bold)
                Spacer(Modifier.height(8.dp))
                Row(horizontalArrangement = Arrangement.spacedBy(8.dp)) {
                    Button(onClick = onOpenProfilePicker, modifier = Modifier.weight(1f)) {
                        Text("Buscar Perfiles (${PSeIntProfile.PopularNames.size})", fontSize = 12.sp)
                    }
                    if (selectedProfile.name == "Personalizado") {
                        OutlinedButton(onClick = onEditCustomProfile) {
                            Text("Editar Reglas", fontSize = 12.sp)
                        }
                    }
                }
            }
        }

        // Appearance & Themes Card
        Text("Tema de Color de la App", color = Color(0xFF4CAF50), fontSize = 14.sp, fontWeight = FontWeight.Bold, modifier = Modifier.padding(bottom = 6.dp))
        Surface(
            color = Color(0xFF1E1E2C),
            shape = RoundedCornerShape(12.dp),
            modifier = Modifier.fillMaxWidth().padding(bottom = 16.dp)
        ) {
            Column {
                ThemeOptionRow("PSeInt Oscuro (Modo Noche)", AppTheme.Dark, selectedTheme) { onThemeSelected(AppTheme.Dark) }
                Divider(color = Color(0xFF333344))
                ThemeOptionRow("PSeInt Clásico (Blanco / Escritorio)", AppTheme.LightClassic, selectedTheme) { onThemeSelected(AppTheme.LightClassic) }
                Divider(color = Color(0xFF333344))
                ThemeOptionRow("Azul Océano (Ocean)", AppTheme.OceanBlue, selectedTheme) { onThemeSelected(AppTheme.OceanBlue) }
                Divider(color = Color(0xFF333344))
                ThemeOptionRow("Hacker Matrix (Verde / Negro)", AppTheme.HackerMatrix, selectedTheme) { onThemeSelected(AppTheme.HackerMatrix) }
            }
        }

        // Diagram Style Card
        Text("Formato de Diagramas", color = Color(0xFF4CAF50), fontSize = 14.sp, fontWeight = FontWeight.Bold, modifier = Modifier.padding(bottom = 6.dp))
        Surface(
            color = Color(0xFF1E1E2C),
            shape = RoundedCornerShape(12.dp),
            modifier = Modifier.fillMaxWidth().padding(bottom = 16.dp)
        ) {
            Column {
                Row(
                    verticalAlignment = Alignment.CenterVertically,
                    modifier = Modifier.fillMaxWidth().clickable { onDiagramTypeSelected(DiagramType.Classic) }.padding(12.dp)
                ) {
                    RadioButton(selected = diagramType == DiagramType.Classic, onClick = { onDiagramTypeSelected(DiagramType.Classic) })
                    Spacer(Modifier.width(8.dp))
                    Text("Diagrama de Flujo Clásico (Rombos/Paralelogramos)", color = Color.White, fontSize = 13.sp)
                }
                Divider(color = Color(0xFF333344))
                Row(
                    verticalAlignment = Alignment.CenterVertically,
                    modifier = Modifier.fillMaxWidth().clickable { onDiagramTypeSelected(DiagramType.NassiShneiderman) }.padding(12.dp)
                ) {
                    RadioButton(selected = diagramType == DiagramType.NassiShneiderman, onClick = { onDiagramTypeSelected(DiagramType.NassiShneiderman) })
                    Spacer(Modifier.width(8.dp))
                    Text("Diagrama Nassi-Shneiderman (Bloques Estructurados)", color = Color.White, fontSize = 13.sp)
                }
            }
        }

        // Editor Options Card
        Text("Opciones de Editor", color = Color(0xFF4CAF50), fontSize = 14.sp, fontWeight = FontWeight.Bold, modifier = Modifier.padding(bottom = 6.dp))
        Surface(
            color = Color(0xFF1E1E2C),
            shape = RoundedCornerShape(12.dp),
            modifier = Modifier.fillMaxWidth().padding(bottom = 16.dp)
        ) {
            Column(modifier = Modifier.padding(16.dp)) {
                Row(
                    verticalAlignment = Alignment.CenterVertically,
                    horizontalArrangement = Arrangement.SpaceBetween,
                    modifier = Modifier.fillMaxWidth()
                ) {
                    Text("Tamaño de Fuente (${fontSizeSp}sp)", color = Color.White, fontSize = 14.sp)
                    Row {
                        IconButton(onClick = { if (fontSizeSp > 10) onFontSizeChanged(fontSizeSp - 2) }) {
                            Icon(Icons.Filled.Remove, contentDescription = "Menos", tint = Color.White)
                        }
                        IconButton(onClick = { if (fontSizeSp < 24) onFontSizeChanged(fontSizeSp + 2) }) {
                            Icon(Icons.Filled.Add, contentDescription = "Más", tint = Color.White)
                        }
                    }
                }
                Divider(color = Color(0xFF333344), modifier = Modifier.padding(vertical = 8.dp))
                CustomToggleRow("Mostrar Números de Línea", showLineNumbers) { onShowLineNumbersToggled(it) }
            }
        }
    }
}

@Composable
fun ThemeOptionRow(title: String, theme: AppTheme, selectedTheme: AppTheme, onClick: () -> Unit) {
    Row(
        verticalAlignment = Alignment.CenterVertically,
        modifier = Modifier.fillMaxWidth().clickable(onClick = onClick).padding(12.dp)
    ) {
        RadioButton(selected = theme == selectedTheme, onClick = onClick)
        Spacer(Modifier.width(8.dp))
        Text(title, color = Color.White, fontSize = 13.sp)
    }
}

@Composable
fun CustomToggleRow(title: String, checked: Boolean, onCheckedChange: (Boolean) -> Unit) {
    Row(
        verticalAlignment = Alignment.CenterVertically,
        horizontalArrangement = Arrangement.SpaceBetween,
        modifier = Modifier.fillMaxWidth().padding(vertical = 6.dp)
    ) {
        Text(title, color = Color.White, fontSize = 13.sp)
        Switch(
            checked = checked,
            onCheckedChange = onCheckedChange,
            colors = SwitchDefaults.colors(checkedThumbColor = Color(0xFF4CAF50))
        )
    }
}

@Composable
fun PSeIntCommandAssetButton(text: String, iconRes: Int, textColor: Color, onClick: () -> Unit) {
    Column(
        horizontalAlignment = Alignment.CenterHorizontally,
        verticalArrangement = Arrangement.Center,
        modifier = Modifier
            .width(72.dp)
            .background(Color(0xFF22222E), RoundedCornerShape(10.dp))
            .border(1.dp, Color(0xFF333344), RoundedCornerShape(10.dp))
            .clickable(onClick = onClick)
            .padding(6.dp)
    ) {
        Image(
            painter = painterResource(id = iconRes),
            contentDescription = text,
            modifier = Modifier.size(26.dp)
        )
        Spacer(modifier = Modifier.height(4.dp))
        Text(text, fontSize = 9.sp, fontWeight = FontWeight.Bold, color = Color.White, textAlign = TextAlign.Center)
    }
}

@Composable
fun PrimaryActionChip(text: String, icon: ImageVector, onClick: () -> Unit) {
    Row(
        verticalAlignment = Alignment.CenterVertically,
        modifier = Modifier
            .background(Color(0xFF4CAF50), RoundedCornerShape(8.dp))
            .clickable(onClick = onClick)
            .padding(horizontal = 12.dp, vertical = 6.dp),
        horizontalArrangement = Arrangement.spacedBy(6.dp)
    ) {
        Icon(icon, contentDescription = null, modifier = Modifier.size(16.dp), tint = Color.White)
        if (text.isNotEmpty()) Text(text, fontSize = 12.sp, fontWeight = FontWeight.Medium, color = Color.White)
    }
}

@Composable
fun SecondaryActionChip(text: String, icon: ImageVector, textColor: Color, onClick: () -> Unit) {
    Row(
        verticalAlignment = Alignment.CenterVertically,
        modifier = Modifier
            .background(Color(0xFF22222E), RoundedCornerShape(8.dp))
            .border(1.dp, Color(0xFF444455), RoundedCornerShape(8.dp))
            .clickable(onClick = onClick)
            .padding(horizontal = 12.dp, vertical = 6.dp),
        horizontalArrangement = Arrangement.spacedBy(6.dp)
    ) {
        Icon(icon, contentDescription = null, modifier = Modifier.size(16.dp), tint = Color.White)
        if (text.isNotEmpty()) Text(text, fontSize = 12.sp, fontWeight = FontWeight.Medium, color = Color.White)
    }
}

@Composable
fun BottomNavButton(text: String, icon: ImageVector, selected: Boolean, primaryText: Color, secondaryText: Color, onClick: () -> Unit) {
    Column(
        horizontalAlignment = Alignment.CenterHorizontally,
        modifier = Modifier.clickable(onClick = onClick).padding(6.dp)
    ) {
        Box(
            modifier = Modifier
                .clip(CircleShape)
                .background(if (selected) Color(0xFF4CAF50) else Color.Transparent)
                .padding(horizontal = 16.dp, vertical = 4.dp)
        ) {
            Icon(icon, contentDescription = null, tint = if (selected) Color.White else secondaryText)
        }
        Spacer(modifier = Modifier.height(2.dp))
        Text(text, fontSize = 10.sp, fontWeight = if (selected) FontWeight.Bold else FontWeight.Medium, color = if (selected) Color(0xFF4CAF50) else secondaryText)
    }
}

private data class Sextuple<A, B, C, D, E, F>(val first: A, val second: B, val third: C, val fourth: D, val fifth: E, val sixth: F)
