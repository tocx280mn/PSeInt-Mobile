package com.example

import android.content.Intent
import android.net.Uri
import androidx.activity.compose.rememberLauncherForActivityResult
import androidx.activity.compose.BackHandler
import androidx.activity.result.contract.ActivityResultContracts
import androidx.compose.foundation.*
import androidx.compose.foundation.layout.*
import androidx.compose.foundation.lazy.LazyColumn
import androidx.compose.foundation.lazy.items
import androidx.compose.foundation.shape.RoundedCornerShape
import androidx.compose.foundation.text.KeyboardActions
import androidx.compose.foundation.text.KeyboardOptions
import androidx.compose.foundation.text.selection.SelectionContainer
import androidx.compose.material.icons.Icons
import androidx.compose.material.icons.filled.*
import androidx.compose.material3.*
import androidx.compose.runtime.*
import androidx.compose.runtime.saveable.rememberSaveable
import androidx.compose.ui.Alignment
import androidx.compose.ui.Modifier
import androidx.compose.ui.graphics.Color
import androidx.compose.ui.graphics.vector.ImageVector
import androidx.compose.ui.platform.LocalContext
import androidx.compose.ui.platform.LocalSoftwareKeyboardController
import androidx.compose.ui.res.painterResource
import androidx.compose.ui.text.TextRange
import androidx.compose.ui.text.font.FontFamily
import androidx.compose.ui.text.font.FontWeight
import androidx.compose.ui.text.input.ImeAction
import androidx.compose.ui.text.input.TextFieldValue
import androidx.compose.ui.text.style.TextOverflow
import androidx.compose.ui.unit.dp
import androidx.compose.ui.unit.sp
import androidx.lifecycle.viewmodel.compose.viewModel
import com.example.ui.theme.MyApplicationTheme
import kotlinx.coroutines.Dispatchers
import kotlinx.coroutines.launch
import kotlinx.coroutines.withContext
import java.nio.ByteBuffer
import java.nio.charset.CodingErrorAction

@Composable
fun PSeIntApp(vm: WorkspaceViewModel = viewModel()) {
    val context = LocalContext.current
    val keyboard = LocalSoftwareKeyboardController.current
    val scope = rememberCoroutineScope()
    val snackbar = remember { SnackbarHostState() }
    val dark = vm.theme != AppTheme.LightClassic
    var menu by remember { mutableStateOf<String?>(null) }
    var searchVisible by rememberSaveable { mutableStateOf(false) }
    var showCommands by rememberSaveable { mutableStateOf(true) }
    var showProfiles by remember { mutableStateOf(false) }
    var showCustom by remember { mutableStateOf(false) }
    var showOperators by remember { mutableStateOf(false) }
    var showVariables by remember { mutableStateOf(false) }
    var showExamples by remember { mutableStateOf(false) }
    var showHelp by remember { mutableStateOf(false) }
    var replacement by remember { mutableStateOf<(() -> Unit)?>(null) }
    var afterSave by remember { mutableStateOf<(() -> Unit)?>(null) }
    var saveSnapshot by rememberSaveable { mutableStateOf("") }
    var customProfile by remember { mutableStateOf(PSeIntProfile.loadCustomProfile(context)) }
    val notify: (String) -> Unit = { scope.launch { snackbar.showSnackbar(it) } }
    val resolver = context.contentResolver
    val opened = rememberLauncherForActivityResult(ActivityResultContracts.OpenDocument()) { uri ->
        if (uri != null) scope.launch {
            runCatching {
                val document = withContext(Dispatchers.IO) {
                    readPSeIntDocument(resolver, uri)
                }
                runCatching { resolver.takePersistableUriPermission(uri, Intent.FLAG_GRANT_READ_URI_PERMISSION or Intent.FLAG_GRANT_WRITE_URI_PERMISSION) }
                vm.openDocument(document.first, document.second, uri.toString())
            }.onFailure { notify("No se pudo abrir: ${it.message}") }
        }
    }
    val saveAs = rememberLauncherForActivityResult(CreatePSeIntDocument()) { uri ->
        if (uri != null) scope.launch {
            runCatching {
                val snapshot = saveSnapshot
                val saved = withContext(Dispatchers.IO) {
                    writePSeIntDocument(resolver, uri, snapshot, allowRename = true)
                }
                runCatching { resolver.takePersistableUriPermission(saved.first, Intent.FLAG_GRANT_READ_URI_PERMISSION or Intent.FLAG_GRANT_WRITE_URI_PERMISSION) }
                vm.markSaved(saved.first.toString(), saved.second, snapshot)
                notify("Archivo guardado")
                afterSave?.invoke(); afterSave = null
            }.onFailure { afterSave = null; notify("No se pudo guardar: ${it.message}") }
        } else afterSave = null
    }
    fun save(alwaysChoose: Boolean = false) {
        val uri = vm.fileUri
        saveSnapshot = vm.editor.text
        if (uri == null || alwaysChoose || !isPSeIntFile(vm.fileName)) saveAs.launch(pscFileName(vm.fileName))
        else scope.launch {
            val snapshot = saveSnapshot
            runCatching {
                val saved = withContext(Dispatchers.IO) {
                    writePSeIntDocument(resolver, Uri.parse(uri), snapshot, allowRename = false)
                }
                vm.markSaved(saved.first.toString(), saved.second, snapshot)
                notify("Archivo guardado")
                afterSave?.invoke(); afterSave = null
            }.onFailure { afterSave = null; notify("No se pudo guardar. Usa Archivo > Guardar como. ${it.message}") }
        }
    }
    fun replaceDocument(action: () -> Unit) { if (vm.isModified) replacement = action else action() }
    fun run(debug: Boolean) { keyboard?.hide(); vm.run(debug) }
    var profileExportSnapshot by rememberSaveable { mutableStateOf<String?>(null) }
    val profileExport = rememberLauncherForActivityResult(ActivityResultContracts.CreateDocument("application/octet-stream")) { uri ->
        val snapshot = profileExportSnapshot
        profileExportSnapshot = null
        if (uri != null) scope.launch {
            runCatching {
                withContext(Dispatchers.IO) {
                    (resolver.openOutputStream(uri) ?: error("Archivo inaccesible")).use {
                        it.write(desktopBytes(snapshot ?: error("No hay un perfil para exportar")))
                    }
                }
                notify("Perfil exportado")
            }.onFailure { notify("No se pudo exportar el perfil: ${it.message}") }
        }
    }
    val profileImport = rememberLauncherForActivityResult(ActivityResultContracts.OpenDocument()) { uri ->
        if (uri != null) scope.launch {
            runCatching {
                val imported = withContext(Dispatchers.IO) {
                    val bytes = resolver.openInputStream(uri)?.use { it.readBytes() } ?: error("Archivo inaccesible")
                    PSeIntProfile.importProfileFromString(decodePSeIntDocument(bytes))
                }
                customProfile = imported
                PSeIntProfile.saveCustomProfile(context, imported)
                vm.selectProfile(imported)
                showCustom = false
                notify("Perfil importado y seleccionado")
            }.onFailure { notify("No se pudo importar el perfil: ${it.message}") }
        }
    }

    MyApplicationTheme(darkTheme = dark) {
        val colors = MaterialTheme.colorScheme
        val paper = when (vm.theme) {
            AppTheme.LightClassic -> Color.White
            AppTheme.HackerMatrix -> Color(0xFF061109)
            AppTheme.OceanBlue -> Color(0xFF102337)
            AppTheme.Dark -> Color(0xFF202124)
        }
        if (vm.showOnboarding) {
            BackHandler(enabled = !vm.prefs.getBoolean("first_run", true)) { vm.finishOnboarding() }
            OnboardingWizard(
                selectedProfile = vm.profile, onProfileSelected = vm::selectProfile,
                selectedTheme = vm.theme,
                onThemeSelected = { vm.theme = it; vm.prefs.edit().putString("selected_theme", it.name).apply() },
                onFinishOnboarding = vm::finishOnboarding,
                surfaceColor = colors.surface, bgColor = paper, textColor = colors.onSurface
            )
            return@MyApplicationTheme
        }
        Scaffold(
            containerColor = colors.surface,
            snackbarHost = { SnackbarHost(snackbar) },
            modifier = Modifier.imePadding(),
            bottomBar = {
                Column(Modifier.navigationBarsPadding()) {
                    HorizontalDivider()
                    Row(Modifier.fillMaxWidth().height(56.dp)) {
                        WorkspaceTab("Editor", Icons.Default.Edit, vm.tab == AppTab.Editor, Modifier.weight(1f)) { vm.tab = AppTab.Editor }
                        WorkspaceTab("Diagrama", Icons.Default.AccountTree, vm.tab == AppTab.Diagram, Modifier.weight(1f)) { keyboard?.hide(); vm.tab = AppTab.Diagram }
                        WorkspaceTab("Consola", Icons.Default.Terminal, vm.tab == AppTab.Terminal, Modifier.weight(1f)) { keyboard?.hide(); vm.tab = AppTab.Terminal }
                        WorkspaceTab("Ajustes", Icons.Default.Settings, vm.tab == AppTab.Settings, Modifier.weight(1f)) { keyboard?.hide(); vm.tab = AppTab.Settings }
                    }
                }
            }
        ) { padding ->
            Column(Modifier.fillMaxSize().padding(padding)) {
                Row(Modifier.fillMaxWidth().height(48.dp).padding(horizontal = 12.dp), verticalAlignment = Alignment.CenterVertically) {
                    Image(painterResource(R.drawable.desktop_logo), null, Modifier.size(30.dp))
                    Spacer(Modifier.width(8.dp))
                    Text("PSeInt", fontSize = 21.sp, fontWeight = FontWeight.Bold)
                    Text("  Mobile", color = colors.onSurfaceVariant, fontSize = 12.sp)
                    Spacer(Modifier.weight(1f))
                    TextButton(onClick = { showProfiles = true }, contentPadding = PaddingValues(horizontal = 8.dp)) {
                        Text(vm.profile.name, maxLines = 1, overflow = TextOverflow.Ellipsis, modifier = Modifier.widthIn(max = 110.dp), fontSize = 12.sp)
                        Icon(Icons.Default.ArrowDropDown, null, Modifier.size(18.dp))
                    }
                }
                Row(Modifier.fillMaxWidth().horizontalScroll(rememberScrollState()).background(colors.surfaceVariant.copy(alpha = .45f))) {
                    listOf("Archivo", "Editar", "Configurar", "Ejecutar", "Ayuda").forEach { label ->
                        Box {
                            TextButton(onClick = { menu = label }, contentPadding = PaddingValues(horizontal = 10.dp, vertical = 0.dp), modifier = Modifier.height(36.dp).widthIn(min = 0.dp)) {
                                Text(label, color = colors.onSurface, fontSize = 12.sp)
                            }
                            DropdownMenu(expanded = menu == label, onDismissRequest = { menu = null }) {
                                fun action(name: String, enabled: Boolean = true, block: () -> Unit): @Composable () -> Unit = {
                                    DropdownMenuItem(text = { Text(name) }, enabled = enabled, onClick = { menu = null; block() })
                                }
                                when (label) {
                                    "Archivo" -> {
                                        action("Nuevo", !vm.running) { replaceDocument { vm.openDocument(vm.profile.newProgram(), "SinTitulo.psc", saved = false) } }()
                                        action("Abrir…", !vm.running) { replaceDocument { opened.launch(arrayOf("*/*")) } }()
                                        action("Guardar") { save() }()
                                        action("Guardar como…") { save(true) }()
                                        HorizontalDivider()
                                        action("Ejemplos", !vm.running) { showExamples = true }()
                                        action("Compartir código") {
                                            val intent = Intent(Intent.ACTION_SEND).apply { type = "text/plain"; putExtra(Intent.EXTRA_TEXT, vm.editor.text); putExtra(Intent.EXTRA_SUBJECT, vm.fileName) }
                                            context.startActivity(Intent.createChooser(intent, "Compartir pseudocódigo"))
                                        }()
                                    }
                                    "Editar" -> {
                                        action("Deshacer", vm.canUndo) { vm.undo() }()
                                        action("Rehacer", vm.canRedo) { vm.redo() }()
                                        action("Buscar y reemplazar") { searchVisible = !searchVisible; vm.tab = AppTab.Editor }()
                                        action("Ordenar sangría") { vm.edit(TextFieldValue(formatPSeIntCode(vm.editor.text))) }()
                                        action("Comandos y estructuras") { showCommands = !showCommands; vm.tab = AppTab.Editor }()
                                    }
                                    "Configurar" -> {
                                        action("Opciones del lenguaje") { showProfiles = true }()
                                        action("Editar perfil personalizado") { showCustom = true }()
                                        action("Apariencia y editor") { vm.tab = AppTab.Settings }()
                                    }
                                    "Ejecutar" -> {
                                        action("Ejecutar", !vm.running) { run(false) }()
                                        action("Ejecutar paso a paso", !vm.running) { run(true) }()
                                        action("Detener", vm.running) { vm.stop() }()
                                        action("Verificar sintaxis") { vm.validate(); vm.tab = AppTab.Editor }()
                                        action("Lista de variables") { showVariables = true }()
                                    }
                                    else -> {
                                        action("Guía rápida") { showHelp = true }()
                                        action("Ejemplos") { showExamples = true }()
                                        action("Operadores y funciones") { showOperators = true }()
                                    }
                                }
                            }
                        }
                    }
                }
                Row(Modifier.fillMaxWidth().horizontalScroll(rememberScrollState()).padding(horizontal = 4.dp), verticalAlignment = Alignment.CenterVertically) {
                    DesktopTool("Nuevo", R.drawable.desktop_nuevo, !vm.running) { replaceDocument { vm.openDocument(vm.profile.newProgram(), "SinTitulo.psc", saved = false) } }
                    DesktopTool("Abrir", R.drawable.desktop_abrir, !vm.running) { replaceDocument { opened.launch(arrayOf("*/*")) } }
                    DesktopTool("Guardar", R.drawable.desktop_guardar) { save() }
                    VerticalDivider(Modifier.height(25.dp).padding(horizontal = 4.dp))
                    DesktopTool("Deshacer", R.drawable.desktop_deshacer, vm.canUndo) { vm.undo() }
                    DesktopTool("Rehacer", R.drawable.desktop_rehacer, vm.canRedo) { vm.redo() }
                    VerticalDivider(Modifier.height(25.dp).padding(horizontal = 4.dp))
                    if (vm.running) IconButton(onClick = { vm.stop() }) { Icon(Icons.Default.Stop, "Detener", tint = Color(0xFFBF3030)) }
                    else DesktopTool("Ejecutar", R.drawable.desktop_ejecutar) { run(false) }
                    DesktopTool("Paso a paso", R.drawable.desktop_paso, !vm.running) { run(true) }
                    DesktopTool("Ver diagrama", R.drawable.desktop_flujo) { keyboard?.hide(); vm.tab = AppTab.Diagram }
                    DesktopTool("Buscar", R.drawable.desktop_buscar) { searchVisible = !searchVisible; vm.tab = AppTab.Editor }
                    DesktopTool("Ordenar sangría", R.drawable.desktop_indentar) { vm.edit(TextFieldValue(formatPSeIntCode(vm.editor.text))) }
                }
                HorizontalDivider()
                Row(Modifier.fillMaxWidth().background(colors.surfaceVariant.copy(alpha = .5f)).padding(horizontal = 12.dp, vertical = 7.dp), verticalAlignment = Alignment.CenterVertically) {
                    Icon(if (vm.tab == AppTab.Diagram) Icons.Default.AccountTree else Icons.Default.Description, null, Modifier.size(15.dp), tint = colors.primary)
                    Spacer(Modifier.width(6.dp))
                    Text(vm.fileName + if (vm.isModified) " *" else "", fontSize = 12.sp, modifier = Modifier.weight(1f), maxLines = 1, overflow = TextOverflow.Ellipsis)
                    Text(if (vm.running) "EN EJECUCIÓN" else "PSEUDOCÓDIGO", color = colors.onSurfaceVariant, fontSize = 9.sp, letterSpacing = .8.sp)
                }
                BoxWithConstraints(Modifier.weight(1f)) {
                    val compactEditor = maxHeight < 240.dp
                    when (vm.tab) {
                        AppTab.Editor -> Column(Modifier.fillMaxSize()) {
                            if (searchVisible) SearchReplaceBar(vm, onClose = { searchVisible = false })
                            PSeIntCodeEditor(
                                value = vm.editor, onValueChange = vm::edit, modifier = Modifier.weight(1f).fillMaxWidth(),
                                fontSizeSp = vm.fontSize, profile = vm.profile, showLineNumbers = vm.lineNumbers, darkTheme = dark,
                                backgroundColor = paper, currentExecutingLine = vm.executingLine,
                                errorLine = if (vm.syntax.isValid) -1 else vm.syntax.errorLine,
                                autocompleteEnabled = vm.autocomplete, callTipsEnabled = vm.callTips
                            )
                            SyntaxStatusBar(vm)
                            if (!compactEditor) {
                                HorizontalDivider()
                                Row(Modifier.fillMaxWidth().clickable { showCommands = !showCommands }.padding(horizontal = 12.dp, vertical = 6.dp), verticalAlignment = Alignment.CenterVertically) {
                                    Icon(Icons.Default.Widgets, null, Modifier.size(14.dp), tint = colors.onSurfaceVariant)
                                    Text("  Comandos y estructuras", fontSize = 11.sp, modifier = Modifier.weight(1f))
                                    Icon(if (showCommands) Icons.Default.ExpandMore else Icons.Default.ExpandLess, "Alternar comandos", Modifier.size(18.dp))
                                }
                                if (showCommands) CommandPalette(vm.profile) { command -> vm.edit(insertEditorCommand(vm.editor, command)) }
                            }
                        }
                        AppTab.Diagram -> DiagramView(vm.editor.text, { vm.edit(TextFieldValue(it, vm.editor.selection.coerceIn(it.length))) }, vm.diagramType == DiagramType.NassiShneiderman,
                            onDiagramTypeChanged = { ns -> vm.diagramType = if (ns) DiagramType.NassiShneiderman else DiagramType.Classic; vm.prefs.edit().putString("diagram_type", vm.diagramType.name).apply() }, profile = vm.profile)
                        AppTab.Terminal -> ExecutionConsole(vm)
                        AppTab.Settings -> SettingsView(
                            selectedProfile = vm.profile, onOpenProfilePicker = { showProfiles = true }, onEditCustomProfile = { showCustom = true },
                            fontSizeSp = vm.fontSize, onFontSizeChanged = { vm.fontSize = it; vm.prefs.edit().putInt("font_size", it).apply() },
                            selectedTheme = vm.theme, onThemeSelected = { vm.theme = it; vm.prefs.edit().putString("selected_theme", it.name).apply() },
                            diagramType = vm.diagramType, onDiagramTypeSelected = { vm.diagramType = it; vm.prefs.edit().putString("diagram_type", it.name).apply() },
                            showLineNumbers = vm.lineNumbers, onShowLineNumbersToggled = { vm.lineNumbers = it; vm.prefs.edit().putBoolean("show_line_numbers", it).apply() },
                            autocompleteEnabled = vm.autocomplete, onAutocompleteToggled = vm::setAutocompleteEnabled,
                            callTipsEnabled = vm.callTips, onCallTipsToggled = vm::setCallTipsEnabled,
                            onReopenWizard = vm::reopenOnboarding, primaryText = colors.onSurface, surfaceColor = colors.surface, bgColor = paper
                        )
                    }
                }
            }
        }
        if (replacement != null) AlertDialog(
            onDismissRequest = { replacement = null }, title = { Text("Guardar cambios") },
            text = { Text("${vm.fileName} tiene cambios sin guardar. Puedes guardarlos antes de abrir otro documento.") },
            confirmButton = { TextButton(onClick = { afterSave = replacement; replacement = null; save() }) { Text("Guardar y continuar") } },
            dismissButton = { Row { TextButton(onClick = { replacement?.invoke(); replacement = null }) { Text("Descartar") }; TextButton(onClick = { replacement = null }) { Text("Cancelar") } } }
        )
        if (showProfiles) ProfilePickerDialog(vm.profile.name, { vm.selectProfile(it); showProfiles = false }, { showProfiles = false })
        if (showCustom) EditCustomProfileDialog(customProfile, colors.onSurface, colors.surface,
            onSave = { customProfile = it; PSeIntProfile.saveCustomProfile(context, it); vm.selectProfile(it); showCustom = false },
            onExport = { profileExportSnapshot = it.toDesktopProfile(); profileExport.launch("mi_perfil.prf") }, onImport = { profileImport.launch(arrayOf("*/*")) }, onDismiss = { showCustom = false })
        if (showOperators) OperatorsAndFunctionsDialog({ vm.edit(insertEditorText(vm.editor, it)); showOperators = false; vm.tab = AppTab.Editor }, { showOperators = false })
        if (showVariables) VariablesListDialog(vm.editor.text, { showVariables = false })
        if (showExamples) AlertDialog(onDismissRequest = { showExamples = false }, title = { Text("Ejemplos de PSeInt") }, text = {
            Column { PSeIntExamples.all.forEach { example ->
                ListItem(headlineContent = { Text(example.name) }, supportingContent = { Text(example.description) },
                    modifier = Modifier.clickable { showExamples = false; replaceDocument { vm.openDocument(example.code, "${example.name}.psc", saved = false) } })
            } }
        }, confirmButton = { TextButton(onClick = { showExamples = false }) { Text("Cerrar") } })
        if (showHelp) AlertDialog(onDismissRequest = { showHelp = false }, title = { Text("PSeInt en tu móvil") }, text = {
            Column(Modifier.verticalScroll(rememberScrollState())) {
                Text("Escribe tu algoritmo, comprueba la barra de sintaxis y pulsa el botón verde para ejecutarlo. La consola te pedirá los valores de Leer.")
                Spacer(Modifier.height(12.dp))
                Text("Usa las huellas para ejecutar paso a paso. Siguiente ejecuta una instrucción; Continuar avanza automáticamente. Puedes consultar la línea y las variables durante la ejecución.")
                Spacer(Modifier.height(12.dp))
                Text("En Diagrama, arrastra para desplazarte, pellizca para ampliar y toca una figura para editar su instrucción. Ajustar muestra el diagrama completo. Puedes exportarlo como PNG.")
                Spacer(Modifier.height(12.dp))
                Text("El borrador se recupera al volver a abrir la app. Guardar crea o actualiza un archivo .psc. La interfaz y los diagramas toman como referencia PSeInt de Pablo Novara.")
            }
        }, confirmButton = { TextButton(onClick = { showHelp = false }) { Text("Entendido") } })
    }
}

private fun TextRange.coerceIn(length: Int) = TextRange(start.coerceIn(0, length), end.coerceIn(0, length))

internal fun decodePSeIntDocument(bytes: ByteArray): String {
    val decoded = if (bytes.size >= 2 && ((bytes[0] == 0xFF.toByte() && bytes[1] == 0xFE.toByte()) || (bytes[0] == 0xFE.toByte() && bytes[1] == 0xFF.toByte())))
        bytes.toString(Charsets.UTF_16)
    else runCatching { Charsets.UTF_8.newDecoder().onMalformedInput(CodingErrorAction.REPORT).decode(ByteBuffer.wrap(bytes)).toString() }
        .getOrElse { bytes.toString(charset("windows-1252")) }
    return decoded.removePrefix("\uFEFF").replace("\r\n", "\n").replace('\r', '\n')
}

@Composable
fun DesktopTool(label: String, image: Int, enabled: Boolean = true, onClick: () -> Unit) {
    IconButton(onClick = onClick, enabled = enabled, modifier = Modifier.size(44.dp)) {
        Image(painterResource(image), label, Modifier.size(27.dp), alpha = if (enabled) 1f else .32f)
    }
}

@Composable
private fun WorkspaceTab(label: String, icon: ImageVector, selected: Boolean, modifier: Modifier, onClick: () -> Unit) {
    val colors = MaterialTheme.colorScheme
    Column(modifier.fillMaxHeight().background(if (selected) colors.primary.copy(alpha = .08f) else Color.Transparent).clickable(onClick = onClick),
        horizontalAlignment = Alignment.CenterHorizontally, verticalArrangement = Arrangement.Center) {
        Icon(icon, null, Modifier.size(21.dp), tint = if (selected) colors.primary else colors.onSurfaceVariant)
        Text(label, fontSize = 10.sp, color = if (selected) colors.primary else colors.onSurfaceVariant, fontWeight = if (selected) FontWeight.Bold else FontWeight.Normal)
    }
}

@Composable
private fun SyntaxStatusBar(vm: WorkspaceViewModel) {
    val valid = vm.syntax.isValid
    val dark = vm.theme != AppTheme.LightClassic
    val statusColor = if (valid) { if (dark) Color(0xFF8ED29F) else Color(0xFF227538) }
        else if (dark) Color(0xFFFFB4AB) else Color(0xFFB3261E)
    val (line, column) = editorLineColumn(vm.editor)
    Row(Modifier.fillMaxWidth().background(statusColor.copy(alpha = .08f)).clickable(enabled = !valid) { vm.goToLine(vm.syntax.errorLine) }
        .padding(horizontal = 10.dp, vertical = 7.dp), verticalAlignment = Alignment.CenterVertically) {
        Icon(if (valid) Icons.Default.CheckCircleOutline else Icons.Default.ErrorOutline, null, Modifier.size(15.dp), tint = statusColor)
        Spacer(Modifier.width(6.dp))
        Text(if (valid) "Pseudocódigo correcto" else "L${vm.syntax.errorLine}: ${vm.syntax.errorMessage}", color = statusColor, fontSize = 11.sp,
            modifier = Modifier.weight(1f), maxLines = 2, overflow = TextOverflow.Ellipsis)
        if (valid) Text("Lín $line · Col $column", fontSize = 10.sp, color = MaterialTheme.colorScheme.onSurfaceVariant)
    }
}

@Composable
private fun SearchReplaceBar(vm: WorkspaceViewModel, onClose: () -> Unit) {
    var query by rememberSaveable { mutableStateOf("") }
    var replacement by rememberSaveable { mutableStateOf("") }
    val matches = remember(vm.editor.text, query) {
        if (query.isEmpty()) emptyList() else Regex(Regex.escape(query), RegexOption.IGNORE_CASE).findAll(vm.editor.text).map { it.range.first }.toList()
    }
    fun next() {
        if (matches.isEmpty()) return
        val index = matches.firstOrNull { it >= vm.editor.selection.max } ?: matches.first()
        vm.edit(vm.editor.copy(selection = TextRange(index, index + query.length), composition = null))
    }
    Column(Modifier.padding(horizontal = 8.dp, vertical = 4.dp)) {
        Row(verticalAlignment = Alignment.CenterVertically) {
            OutlinedTextField(query, { query = it }, label = { Text("Buscar") }, singleLine = true, modifier = Modifier.weight(1f),
                keyboardOptions = KeyboardOptions(imeAction = ImeAction.Search), keyboardActions = KeyboardActions(onSearch = { next() }),
                trailingIcon = { Text("${matches.size}", fontSize = 11.sp, modifier = Modifier.padding(8.dp)) })
            IconButton(onClick = ::next, enabled = matches.isNotEmpty()) { Icon(Icons.Default.ArrowDownward, "Siguiente coincidencia") }
            IconButton(onClick = onClose) { Icon(Icons.Default.Close, "Cerrar búsqueda") }
        }
        Row(verticalAlignment = Alignment.CenterVertically) {
            OutlinedTextField(replacement, { replacement = it }, label = { Text("Reemplazar por") }, singleLine = true, modifier = Modifier.weight(1f))
            TextButton(enabled = matches.isNotEmpty(), onClick = {
                val selected = vm.editor.text.substring(vm.editor.selection.min, vm.editor.selection.max)
                if (selected.equals(query, true)) vm.edit(insertEditorText(vm.editor, replacement)) else next()
            }) { Text("Uno", fontSize = 11.sp) }
            TextButton(enabled = matches.isNotEmpty(), onClick = {
                val updated = Regex(Regex.escape(query), RegexOption.IGNORE_CASE).replace(vm.editor.text) { replacement }
                vm.edit(TextFieldValue(updated))
            }) { Text("Todos", fontSize = 11.sp) }
        }
    }
}

@Composable
private fun ExecutionConsole(vm: WorkspaceViewModel) {
    var input by rememberSaveable(vm.inputVariable) { mutableStateOf("") }
    Column(Modifier.fillMaxSize()) {
        Row(Modifier.fillMaxWidth().padding(horizontal = 12.dp, vertical = 8.dp), verticalAlignment = Alignment.CenterVertically) {
            Box(Modifier.size(7.dp).background(if (vm.running) Color(0xFF389C42) else Color.Gray, RoundedCornerShape(4.dp)))
            Text("  ${vm.executionStatus}", fontSize = 12.sp, modifier = Modifier.weight(1f))
            if (!vm.running) TextButton(onClick = { vm.run() }) { Text("Ejecutar") }
        }
        val scroll = rememberScrollState()
        LaunchedEffect(vm.output, vm.inputVariable) { scroll.animateScrollTo(scroll.maxValue) }
        Column(Modifier.weight(1f).fillMaxWidth().background(Color(0xFF111713)).padding(14.dp).verticalScroll(scroll)) {
            if (vm.output.isEmpty() && !vm.running) Text("La salida de tu algoritmo aparecerá aquí.\nPulsa Ejecutar para comenzar.", color = Color(0xFFB0BEB4), fontFamily = FontFamily.Monospace, fontSize = 13.sp)
            SelectionContainer {
                Text(vm.output.joinToString(""), color = Color(0xFFE6EEE7), fontFamily = FontFamily.Monospace, fontSize = vm.fontSize.sp, lineHeight = (vm.fontSize + 6).sp)
            }
        }
        if (vm.inputVariable != null) Row(Modifier.padding(10.dp), verticalAlignment = Alignment.CenterVertically) {
            OutlinedTextField(input, { input = it }, label = { Text("Leer ${vm.inputVariable}") }, singleLine = true, modifier = Modifier.weight(1f),
                keyboardOptions = KeyboardOptions(imeAction = ImeAction.Send), keyboardActions = KeyboardActions(onSend = { vm.submitInput(input); input = "" }))
            TextButton(onClick = { vm.submitInput(input); input = "" }) { Text("Enviar") }
        }
        if (vm.debugging || vm.variables.isNotEmpty()) {
            Column(Modifier.fillMaxWidth().heightIn(max = 225.dp).verticalScroll(rememberScrollState()).padding(horizontal = 12.dp, vertical = 6.dp)) {
                Row(verticalAlignment = Alignment.CenterVertically) {
                    Text("Prueba de escritorio", fontWeight = FontWeight.Bold, fontSize = 12.sp, modifier = Modifier.weight(1f))
                    if (vm.executingLine > 0) TextButton(onClick = { vm.goToLine(vm.executingLine) }) { Text("Línea ${vm.executingLine}", fontSize = 11.sp) }
                }
                if (vm.debugging) Row(horizontalArrangement = Arrangement.spacedBy(8.dp)) {
                    OutlinedButton(onClick = vm::nextStep, enabled = vm.paused && vm.inputVariable == null, contentPadding = PaddingValues(horizontal = 10.dp)) { Text("Siguiente", fontSize = 12.sp) }
                    OutlinedButton(onClick = vm::togglePause, contentPadding = PaddingValues(horizontal = 10.dp)) { Text(if (vm.paused) "Continuar" else "Pausar", fontSize = 12.sp) }
                    TextButton(onClick = vm::stop) { Text("Detener", fontSize = 12.sp) }
                }
                if (vm.explanation.isNotEmpty()) Text(vm.explanation, fontSize = 11.sp, color = MaterialTheme.colorScheme.onSurfaceVariant, modifier = Modifier.padding(vertical = 6.dp))
                vm.variables.forEach { (name, value) ->
                    HorizontalDivider()
                    Row(Modifier.fillMaxWidth().padding(vertical = 5.dp)) {
                        Text(name, fontFamily = FontFamily.Monospace, fontSize = 12.sp, modifier = Modifier.weight(1f))
                        Text(value, fontFamily = FontFamily.Monospace, fontSize = 12.sp, modifier = Modifier.weight(1f))
                    }
                }
            }
        }
    }
}
