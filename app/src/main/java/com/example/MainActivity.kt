package com.example

import android.content.Context
import android.content.Intent
import android.os.Bundle
import android.widget.Toast
import androidx.activity.ComponentActivity
import androidx.activity.compose.setContent
import androidx.activity.enableEdgeToEdge
import androidx.compose.foundation.Image
import androidx.compose.foundation.background
import androidx.compose.foundation.border
import androidx.compose.foundation.clickable
import androidx.compose.foundation.horizontalScroll
import androidx.compose.foundation.layout.*
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

    var code by remember {
        mutableStateOf(
            """Algoritmo Ejemplo_PSeInt
    Definir nombre Como Caracter
    Definir edad Como Entero
    
    Escribir "Ingrese su nombre:"
    Leer nombre
    
    Escribir "Ingrese su edad:"
    Leer edad
    
    Si edad >= 18 Entonces
        Escribir "Hola ", nombre, ", eres mayor de edad."
    Sino
        Escribir "Hola ", nombre, ", eres menor de edad."
    FinSi
FinAlgoritmo"""
        )
    }

    var output by remember { mutableStateOf(listOf<String>()) }
    var isRunning by remember { mutableStateOf(false) }
    var awaitingInput by remember { mutableStateOf(false) }
    var inputPromptVar by remember { mutableStateOf("") }
    var currentInput by remember { mutableStateOf("") }

    var currentTab by remember { mutableStateOf(AppTab.Editor) }
    var showExportDialog by remember { mutableStateOf(false) }

    val coroutineScope = rememberCoroutineScope()
    val evaluator = remember { PSeIntEvaluator() }

    val savedProfileName = prefs.getString("selected_profile", "Flexible") ?: "Flexible"
    var selectedProfile by remember {
        mutableStateOf(PSeIntProfile.PopularProfiles.find { it.name == savedProfileName } ?: PSeIntProfile.Flexible)
    }

    var fontSizeSp by remember { mutableStateOf(14) }
    var selectedTheme by remember { mutableStateOf(AppTheme.Dark) }
    var diagramType by remember { mutableStateOf(DiagramType.Classic) }
    var showLineNumbers by remember { mutableStateOf(true) }

    val clipboardManager = LocalClipboardManager.current

    // Theme Colors Definition
    val (editorBgColor, editorTextColor, editorLineNumBg, surfaceColor) = when (selectedTheme) {
        AppTheme.Dark -> Quadruple(IdeSurface, IdeText, IdeSurfaceVariant, IdeBackground)
        AppTheme.LightClassic -> Quadruple(Color(0xFFFAFAFA), Color(0xFF111111), Color(0xFFE0E0E0), Color(0xFFF0F0F0))
        AppTheme.HackerMatrix -> Quadruple(Color(0xFF050505), Color(0xFF00FF66), Color(0xFF0A1A0F), Color(0xFF001100))
        AppTheme.OceanBlue -> Quadruple(Color(0xFF0F1B2B), Color(0xFFE2F1FF), Color(0xFF18293D), Color(0xFF08101C))
    }

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
            onThemeSelected = { selectedTheme = it },
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
                    Divider(color = IdeBorder)
                    Row(
                        modifier = Modifier
                            .fillMaxWidth()
                            .height(64.dp)
                            .background(surfaceColor)
                            .padding(horizontal = 16.dp),
                        horizontalArrangement = Arrangement.SpaceAround,
                        verticalAlignment = Alignment.CenterVertically
                    ) {
                        BottomNavButton("Editor", Icons.Outlined.Edit, currentTab == AppTab.Editor) { currentTab = AppTab.Editor }
                        BottomNavButton("Diagrama", Icons.Outlined.AccountTree, currentTab == AppTab.Diagram) { currentTab = AppTab.Diagram }
                        BottomNavButton("Terminal", Icons.Outlined.Terminal, currentTab == AppTab.Terminal) { currentTab = AppTab.Terminal }
                        BottomNavButton("Ajustes", Icons.Outlined.Settings, currentTab == AppTab.Settings) { currentTab = AppTab.Settings }
                    }
                }
            }
        ) { innerPadding ->
            Column(
                modifier = Modifier
                    .fillMaxSize()
                    .padding(innerPadding)
            ) {
                // Header Bar
                Row(
                    modifier = Modifier
                        .fillMaxWidth()
                        .height(60.dp)
                        .background(surfaceColor)
                        .padding(horizontal = 16.dp),
                    verticalAlignment = Alignment.CenterVertically,
                    horizontalArrangement = Arrangement.SpaceBetween
                ) {
                    Row(verticalAlignment = Alignment.CenterVertically, horizontalArrangement = Arrangement.spacedBy(10.dp)) {
                        Image(
                            painter = painterResource(id = R.drawable.logo_pseint),
                            contentDescription = "PSeInt Logo",
                            modifier = Modifier.height(32.dp)
                        )
                        Column {
                            Text("PSeInt Mobile", fontSize = 16.sp, fontWeight = FontWeight.Bold, color = editorTextColor)
                            Text("PERFIL: ${selectedProfile.name.uppercase()}", fontSize = 10.sp, fontWeight = FontWeight.SemiBold, color = IdeSecondary, letterSpacing = 0.5.sp)
                        }
                    }
                    Row(verticalAlignment = Alignment.CenterVertically, horizontalArrangement = Arrangement.spacedBy(8.dp)) {
                        IconButton(
                            onClick = {
                                if (isRunning) {
                                    isRunning = false
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
                                .size(38.dp)
                                .background(if (isRunning) Color(0xFFFFDAD6) else IdeSecondary, CircleShape)
                        ) {
                            Icon(
                                imageVector = if (isRunning) Icons.Filled.Stop else Icons.Filled.PlayArrow,
                                contentDescription = "Ejecutar",
                                tint = if (isRunning) Color(0xFFBA1A1A) else Color(0xFF001D36)
                            )
                        }
                    }
                }
                Divider(color = IdeBorder)

                if (currentTab == AppTab.Editor) {
                    // Secondary Toolbar with Export & Share
                    Row(
                        modifier = Modifier
                            .fillMaxWidth()
                            .background(surfaceColor)
                            .horizontalScroll(rememberScrollState())
                            .padding(horizontal = 12.dp, vertical = 6.dp),
                        horizontalArrangement = Arrangement.spacedBy(8.dp)
                    ) {
                        PrimaryActionChip("Nuevo", Icons.Outlined.Description) {
                            code = "Algoritmo SinTitulo\n\nFinAlgoritmo"
                        }
                        SecondaryActionChip("Exportar", Icons.Outlined.Code) {
                            showExportDialog = true
                        }
                        SecondaryActionChip("Compartir", Icons.Outlined.Share) {
                            val shareIntent = Intent(Intent.ACTION_SEND).apply {
                                type = "text/plain"
                                putExtra(Intent.EXTRA_SUBJECT, "Codigo PSeInt")
                                putExtra(Intent.EXTRA_TEXT, code)
                            }
                            context.startActivity(Intent.createChooser(shareIntent, "Compartir Pseudocódigo"))
                        }
                        SecondaryActionChip("Limpiar", Icons.Outlined.Delete) {
                            code = ""
                        }
                        SecondaryActionChip("Ejemplo", Icons.Outlined.Lightbulb) {
                            code = """Algoritmo SumaNumeros
    Definir a, b, suma Como Entero
    Escribir "Ingrese primer número:"
    Leer a
    Escribir "Ingrese segundo número:"
    Leer b
    suma <- a + b
    Escribir "La suma es: ", suma
FinAlgoritmo"""
                        }
                    }
                    Divider(color = IdeBorder)

                    // Code Editor Area
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
                                    .background(editorLineNumBg)
                                    .border(1.dp, IdeBorder)
                                    .padding(top = 14.dp),
                                horizontalAlignment = Alignment.CenterHorizontally
                            ) {
                                val lineCount = code.count { it == '\n' } + 1
                                for (i in 1..lineCount) {
                                    Text(
                                        text = i.toString(),
                                        color = LineNumberColor,
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
                                .padding(14.dp)
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

                    Divider(color = IdeBorder)
                    // Quick Commands Palette with Original Desktop PNG Assets
                    Column(
                        modifier = Modifier
                            .fillMaxWidth()
                            .background(editorLineNumBg)
                            .padding(8.dp)
                    ) {
                        Text("TODOS LOS COMANDOS DE PSEINT", fontSize = 10.sp, fontWeight = FontWeight.Bold, color = IdeTextMuted, letterSpacing = 0.5.sp, modifier = Modifier.padding(bottom = 6.dp, start = 4.dp))
                        Row(
                            modifier = Modifier.fillMaxWidth().horizontalScroll(rememberScrollState()),
                            horizontalArrangement = Arrangement.spacedBy(6.dp)
                        ) {
                            PSeIntCommandAssetButton("Escribir", R.drawable.escribir) { insertCommand("Escribir \"\"") }
                            PSeIntCommandAssetButton("Leer", R.drawable.leer) { insertCommand("Leer variable") }
                            PSeIntCommandAssetButton("Asignar", R.drawable.asignar) { insertCommand("variable <- expresion") }
                            PSeIntCommandAssetButton("Definir", R.drawable.asignar) { insertCommand("Definir var Como Entero") }
                            PSeIntCommandAssetButton("Dimension", R.drawable.asignar) { insertCommand("Dimension arreglo[10]") }
                            PSeIntCommandAssetButton("Si-Entonces", R.drawable.si) { insertCommand("Si condicion Entonces\n\t// acciones\nFinSi") }
                            PSeIntCommandAssetButton("Según", R.drawable.segun) { insertCommand("Segun variable Hacer\n\topcion1:\n\t\t// acciones\n\tDe Otro Modo:\n\t\t// acciones\nFinSegun") }
                            PSeIntCommandAssetButton("Mientras", R.drawable.mientras) { insertCommand("Mientras condicion Hacer\n\t// acciones\nFinMientras") }
                            PSeIntCommandAssetButton("Repetir", R.drawable.repetir) { insertCommand("Repetir\n\t// acciones\nHasta Que condicion") }
                            PSeIntCommandAssetButton("Para", R.drawable.para) { insertCommand("Para i<-1 Hasta 10 Con Paso 1 Hacer\n\t// acciones\nFinPara") }
                            PSeIntCommandAssetButton("Función", R.drawable.funcion) { insertCommand("Funcion res <- MiFuncion(arg)\n\tres <- arg * 2\nFinFuncion") }
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
                                            Icon(Icons.Filled.Send, contentDescription = "Enviar", tint = IdeSecondary)
                                        }
                                    },
                                    singleLine = true,
                                    colors = OutlinedTextFieldDefaults.colors(
                                        focusedBorderColor = IdeSecondary,
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
                        onProfileSelected = {
                            selectedProfile = it
                            prefs.edit().putString("selected_profile", it.name).apply()
                        },
                        fontSizeSp = fontSizeSp,
                        onFontSizeChanged = { fontSizeSp = it },
                        selectedTheme = selectedTheme,
                        onThemeSelected = { selectedTheme = it },
                        diagramType = diagramType,
                        onDiagramTypeSelected = { diagramType = it },
                        showLineNumbers = showLineNumbers,
                        onShowLineNumbersToggled = { showLineNumbers = it },
                        onReopenWizard = { showOnboarding = true }
                    )
                }

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
            }
        }
    }
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
            modifier = Modifier.height(70.dp).padding(bottom = 12.dp)
        )
        Text("¡Bienvenido a PSeInt Mobile!", fontSize = 22.sp, fontWeight = FontWeight.Bold, color = Color.White, textAlign = TextAlign.Center)
        Text("Configuración Inicial de la Aplicación", fontSize = 13.sp, color = Color(0xFFA0A0B0), modifier = Modifier.padding(bottom = 24.dp))

        // Step 1: Select Profile
        Surface(
            color = Color(0xFF1E1E2C),
            shape = RoundedCornerShape(12.dp),
            modifier = Modifier.fillMaxWidth().padding(bottom = 16.dp)
        ) {
            Column(modifier = Modifier.padding(16.dp)) {
                Text("1. Selecciona tu Perfil de Lenguaje", color = IdeSecondary, fontSize = 14.sp, fontWeight = FontWeight.Bold, modifier = Modifier.padding(bottom = 8.dp))
                PSeIntProfile.PopularProfiles.take(4).forEach { profile ->
                    Row(
                        verticalAlignment = Alignment.CenterVertically,
                        modifier = Modifier.fillMaxWidth().clickable { onProfileSelected(profile) }.padding(vertical = 6.dp)
                    ) {
                        RadioButton(selected = selectedProfile.name == profile.name, onClick = { onProfileSelected(profile) })
                        Spacer(Modifier.width(8.dp))
                        Text(profile.name, color = Color.White, fontSize = 14.sp)
                    }
                }
            }
        }

        // Step 2: Select Theme
        Surface(
            color = Color(0xFF1E1E2C),
            shape = RoundedCornerShape(12.dp),
            modifier = Modifier.fillMaxWidth().padding(bottom = 24.dp)
        ) {
            Column(modifier = Modifier.padding(16.dp)) {
                Text("2. Apariencia Inicial", color = IdeSecondary, fontSize = 14.sp, fontWeight = FontWeight.Bold, modifier = Modifier.padding(bottom = 8.dp))
                ThemeOptionRow("PSeInt Oscuro (Noche)", AppTheme.Dark, selectedTheme) { onThemeSelected(AppTheme.Dark) }
                ThemeOptionRow("PSeInt Clásico (Blanco Escritorio)", AppTheme.LightClassic, selectedTheme) { onThemeSelected(AppTheme.LightClassic) }
            }
        }

        Button(
            onClick = onFinishOnboarding,
            colors = ButtonDefaults.buttonColors(containerColor = IdeSecondary),
            shape = RoundedCornerShape(12.dp),
            modifier = Modifier.fillMaxWidth().height(50.dp)
        ) {
            Text("COMENZAR A PROGRAMAR", fontSize = 14.sp, fontWeight = FontWeight.Bold, color = Color.White)
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
        title = { Text("Exportar Código a Otro Lenguaje", fontWeight = FontWeight.Bold) },
        text = {
            Column(modifier = Modifier.fillMaxWidth()) {
                Text("Selecciona el lenguaje destino:", fontSize = 12.sp, color = IdeTextMuted, modifier = Modifier.padding(bottom = 8.dp))
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
                Spacer(modifier = Modifier.height(12.dp))
                Surface(
                    color = Color(0xFF111118),
                    shape = RoundedCornerShape(8.dp),
                    modifier = Modifier.fillMaxWidth().height(160.dp).padding(4.dp)
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
            TextButton(onClick = onDismiss) {
                Text("Cancelar")
            }
        }
    )
}

@Composable
fun SettingsView(
    selectedProfile: PSeIntProfile,
    onProfileSelected: (PSeIntProfile) -> Unit,
    fontSizeSp: Int,
    onFontSizeChanged: (Int) -> Unit,
    selectedTheme: AppTheme,
    onThemeSelected: (AppTheme) -> Unit,
    diagramType: DiagramType,
    onDiagramTypeSelected: (DiagramType) -> Unit,
    showLineNumbers: Boolean,
    onShowLineNumbersToggled: (Boolean) -> Unit,
    onReopenWizard: () -> Unit
) {
    Column(
        modifier = Modifier
            .fillMaxSize()
            .background(IdeBackground)
            .padding(16.dp)
            .verticalScroll(rememberScrollState())
    ) {
        Text("PERSONALIZACIÓN Y APARIENCIA", color = IdeText, fontSize = 18.sp, fontWeight = FontWeight.Bold, modifier = Modifier.padding(bottom = 6.dp))
        Text("Personaliza el tema, los diagramas y el editor igual que en PSeInt Desktop:", color = IdeTextMuted, fontSize = 12.sp, modifier = Modifier.padding(bottom = 16.dp))

        // Onboarding Re-run Button
        Button(
            onClick = onReopenWizard,
            colors = ButtonDefaults.buttonColors(containerColor = IdeSecondary),
            shape = RoundedCornerShape(8.dp),
            modifier = Modifier.fillMaxWidth().padding(bottom = 16.dp)
        ) {
            Icon(Icons.Filled.AutoFixHigh, contentDescription = null, tint = Color.White)
            Spacer(Modifier.width(8.dp))
            Text("Reabrir Asistente de Configuración Inicial", color = Color.White, fontSize = 13.sp)
        }

        // Appearance & Themes Card
        Text("Tema de Color de la App", color = IdeSecondary, fontSize = 14.sp, fontWeight = FontWeight.Bold, modifier = Modifier.padding(bottom = 8.dp))
        Surface(
            color = IdeSurface,
            shape = RoundedCornerShape(12.dp),
            border = androidx.compose.foundation.BorderStroke(1.dp, IdeBorder),
            modifier = Modifier.padding(bottom = 16.dp)
        ) {
            Column {
                ThemeOptionRow("PSeInt Oscuro (Modo Noche)", AppTheme.Dark, selectedTheme) { onThemeSelected(AppTheme.Dark) }
                Divider(color = IdeBorder)
                ThemeOptionRow("PSeInt Clásico (Blanco / Escritorio)", AppTheme.LightClassic, selectedTheme) { onThemeSelected(AppTheme.LightClassic) }
                Divider(color = IdeBorder)
                ThemeOptionRow("Azul Océano (Ocean)", AppTheme.OceanBlue, selectedTheme) { onThemeSelected(AppTheme.OceanBlue) }
                Divider(color = IdeBorder)
                ThemeOptionRow("Hacker Matrix (Verde / Negro)", AppTheme.HackerMatrix, selectedTheme) { onThemeSelected(AppTheme.HackerMatrix) }
            }
        }

        // Diagram Style Card
        Text("Formato de Diagramas", color = IdeSecondary, fontSize = 14.sp, fontWeight = FontWeight.Bold, modifier = Modifier.padding(bottom = 8.dp))
        Surface(
            color = IdeSurface,
            shape = RoundedCornerShape(12.dp),
            border = androidx.compose.foundation.BorderStroke(1.dp, IdeBorder),
            modifier = Modifier.padding(bottom = 16.dp)
        ) {
            Column {
                Row(
                    verticalAlignment = Alignment.CenterVertically,
                    modifier = Modifier.fillMaxWidth().clickable { onDiagramTypeSelected(DiagramType.Classic) }.padding(12.dp)
                ) {
                    RadioButton(selected = diagramType == DiagramType.Classic, onClick = { onDiagramTypeSelected(DiagramType.Classic) })
                    Spacer(Modifier.width(8.dp))
                    Text("Diagrama de Flujo Clásico (Rombos/Paralelogramos)", color = IdeText, fontSize = 13.sp)
                }
                Divider(color = IdeBorder)
                Row(
                    verticalAlignment = Alignment.CenterVertically,
                    modifier = Modifier.fillMaxWidth().clickable { onDiagramTypeSelected(DiagramType.NassiShneiderman) }.padding(12.dp)
                ) {
                    RadioButton(selected = diagramType == DiagramType.NassiShneiderman, onClick = { onDiagramTypeSelected(DiagramType.NassiShneiderman) })
                    Spacer(Modifier.width(8.dp))
                    Text("Diagrama Nassi-Shneiderman (Bloques Estructurados)", color = IdeText, fontSize = 13.sp)
                }
            }
        }

        // Editor Options Card
        Text("Opciones de Editor", color = IdeSecondary, fontSize = 14.sp, fontWeight = FontWeight.Bold, modifier = Modifier.padding(bottom = 8.dp))
        Surface(
            color = IdeSurface,
            shape = RoundedCornerShape(12.dp),
            border = androidx.compose.foundation.BorderStroke(1.dp, IdeBorder),
            modifier = Modifier.padding(bottom = 16.dp)
        ) {
            Column(modifier = Modifier.padding(16.dp)) {
                Row(
                    verticalAlignment = Alignment.CenterVertically,
                    horizontalArrangement = Arrangement.SpaceBetween,
                    modifier = Modifier.fillMaxWidth()
                ) {
                    Text("Tamaño de Fuente (${fontSizeSp}sp)", color = IdeText, fontSize = 14.sp)
                    Row {
                        IconButton(onClick = { if (fontSizeSp > 10) onFontSizeChanged(fontSizeSp - 2) }) {
                            Icon(Icons.Filled.Remove, contentDescription = "Menos", tint = IdeText)
                        }
                        IconButton(onClick = { if (fontSizeSp < 24) onFontSizeChanged(fontSizeSp + 2) }) {
                            Icon(Icons.Filled.Add, contentDescription = "Más", tint = IdeText)
                        }
                    }
                }
                Divider(color = IdeBorder, modifier = Modifier.padding(vertical = 8.dp))
                CustomToggleRow("Mostrar Números de Línea", showLineNumbers) { onShowLineNumbersToggled(it) }
            }
        }

        // Language Profiles Card
        Text("Perfil de Lenguaje Institucional", color = IdeSecondary, fontSize = 14.sp, fontWeight = FontWeight.Bold, modifier = Modifier.padding(bottom = 8.dp))
        Surface(
            color = IdeSurface,
            shape = RoundedCornerShape(12.dp),
            border = androidx.compose.foundation.BorderStroke(1.dp, IdeBorder)
        ) {
            Column {
                PSeIntProfile.PopularProfiles.forEachIndexed { index, profile ->
                    Row(
                        verticalAlignment = Alignment.CenterVertically,
                        modifier = Modifier
                            .fillMaxWidth()
                            .clickable { onProfileSelected(profile) }
                            .padding(vertical = 12.dp, horizontal = 16.dp)
                    ) {
                        RadioButton(
                            selected = selectedProfile.name == profile.name,
                            onClick = { onProfileSelected(profile) },
                            colors = RadioButtonDefaults.colors(selectedColor = IdeSecondary, unselectedColor = IdeTextMuted)
                        )
                        Spacer(modifier = Modifier.width(12.dp))
                        Column {
                            Text(profile.name, color = IdeText, fontSize = 15.sp, fontWeight = FontWeight.Bold)
                            Text(profile.description, color = IdeTextMuted, fontSize = 11.sp)
                        }
                    }
                    if (index < PSeIntProfile.PopularProfiles.size - 1) {
                        Divider(color = IdeBorder, modifier = Modifier.padding(horizontal = 16.dp))
                    }
                }
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
        Text(title, color = IdeText, fontSize = 13.sp)
    }
}

@Composable
fun CustomToggleRow(title: String, checked: Boolean, onCheckedChange: (Boolean) -> Unit) {
    Row(
        verticalAlignment = Alignment.CenterVertically,
        horizontalArrangement = Arrangement.SpaceBetween,
        modifier = Modifier.fillMaxWidth().padding(vertical = 6.dp)
    ) {
        Text(title, color = IdeText, fontSize = 13.sp)
        Switch(
            checked = checked,
            onCheckedChange = onCheckedChange,
            colors = SwitchDefaults.colors(checkedThumbColor = IdeSecondary)
        )
    }
}

@Composable
fun PSeIntCommandAssetButton(text: String, iconRes: Int, onClick: () -> Unit) {
    Column(
        horizontalAlignment = Alignment.CenterHorizontally,
        verticalArrangement = Arrangement.Center,
        modifier = Modifier
            .width(74.dp)
            .background(IdeSurface, RoundedCornerShape(10.dp))
            .border(1.dp, IdeBorder, RoundedCornerShape(10.dp))
            .clickable(onClick = onClick)
            .padding(6.dp)
    ) {
        Image(
            painter = painterResource(id = iconRes),
            contentDescription = text,
            modifier = Modifier.size(28.dp)
        )
        Spacer(modifier = Modifier.height(4.dp))
        Text(text, fontSize = 9.sp, fontWeight = FontWeight.Bold, color = IdeText, textAlign = TextAlign.Center)
    }
}

@Composable
fun PrimaryActionChip(text: String, icon: ImageVector, onClick: () -> Unit) {
    Row(
        verticalAlignment = Alignment.CenterVertically,
        modifier = Modifier
            .background(IdeSecondary, RoundedCornerShape(8.dp))
            .clickable(onClick = onClick)
            .padding(horizontal = 12.dp, vertical = 6.dp),
        horizontalArrangement = Arrangement.spacedBy(6.dp)
    ) {
        Icon(icon, contentDescription = null, modifier = Modifier.size(16.dp), tint = Color.White)
        if (text.isNotEmpty()) Text(text, fontSize = 12.sp, fontWeight = FontWeight.Medium, color = Color.White)
    }
}

@Composable
fun SecondaryActionChip(text: String, icon: ImageVector, onClick: () -> Unit) {
    Row(
        verticalAlignment = Alignment.CenterVertically,
        modifier = Modifier
            .border(1.dp, IdeTextMuted, RoundedCornerShape(8.dp))
            .clickable(onClick = onClick)
            .padding(horizontal = 12.dp, vertical = 6.dp),
        horizontalArrangement = Arrangement.spacedBy(6.dp)
    ) {
        Icon(icon, contentDescription = null, modifier = Modifier.size(16.dp), tint = IdeText)
        if (text.isNotEmpty()) Text(text, fontSize = 12.sp, fontWeight = FontWeight.Medium, color = IdeText)
    }
}

@Composable
fun BottomNavButton(text: String, icon: ImageVector, selected: Boolean, onClick: () -> Unit) {
    Column(
        horizontalAlignment = Alignment.CenterHorizontally,
        modifier = Modifier.clickable(onClick = onClick).padding(8.dp)
    ) {
        Box(
            modifier = Modifier
                .clip(CircleShape)
                .background(if (selected) IdeSecondary else Color.Transparent)
                .padding(horizontal = 18.dp, vertical = 4.dp)
        ) {
            Icon(icon, contentDescription = null, tint = if (selected) Color.White else IdeTextMuted)
        }
        Spacer(modifier = Modifier.height(2.dp))
        Text(text, fontSize = 10.sp, fontWeight = if (selected) FontWeight.Bold else FontWeight.Medium, color = if (selected) IdeSecondary else IdeTextMuted)
    }
}

private data class Quadruple<A, B, C, D>(val first: A, val second: B, val third: C, val fourth: D)
