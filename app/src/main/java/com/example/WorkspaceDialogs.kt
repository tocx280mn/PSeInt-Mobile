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
import androidx.compose.material.icons.automirrored.filled.DirectionsWalk
import androidx.compose.material.icons.automirrored.filled.Send
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
import androidx.compose.ui.semantics.contentDescription
import androidx.compose.ui.semantics.semantics
import androidx.compose.ui.text.AnnotatedString
import androidx.compose.ui.text.TextStyle
import androidx.compose.ui.text.font.FontFamily
import androidx.compose.ui.text.font.FontWeight
import androidx.compose.ui.text.style.TextAlign
import androidx.compose.ui.unit.dp
import androidx.compose.ui.unit.sp
import com.example.ui.theme.*
import kotlinx.coroutines.Job
import kotlinx.coroutines.launch
import java.io.BufferedReader
import java.io.InputStreamReader

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
                Spacer(Modifier.height(10.dp))
                Text("Trigonométricas", color = Color(0xFF4CAF50), fontSize = 12.sp, fontWeight = FontWeight.Bold)
                Row(horizontalArrangement = Arrangement.spacedBy(4.dp)) {
                    Button(onClick = { onInsert("sen()") }, modifier = Modifier.weight(1f)) { Text("sen(x)") }
                    Button(onClick = { onInsert("cos()") }, modifier = Modifier.weight(1f)) { Text("cos(x)") }
                    Button(onClick = { onInsert("tan()") }, modifier = Modifier.weight(1f)) { Text("tan(x)") }
                }
                Row(horizontalArrangement = Arrangement.spacedBy(4.dp)) {
                    Button(onClick = { onInsert("asen()") }, modifier = Modifier.weight(1f)) { Text("asen(x)") }
                    Button(onClick = { onInsert("acos()") }, modifier = Modifier.weight(1f)) { Text("acos(x)") }
                    Button(onClick = { onInsert("atan()") }, modifier = Modifier.weight(1f)) { Text("atan(x)") }
                }
                Spacer(Modifier.height(10.dp))
                Text("Más Matemáticas", color = Color(0xFF4CAF50), fontSize = 12.sp, fontWeight = FontWeight.Bold)
                Row(horizontalArrangement = Arrangement.spacedBy(4.dp)) {
                    Button(onClick = { onInsert("ln()") }, modifier = Modifier.weight(1f)) { Text("ln(x)") }
                    Button(onClick = { onInsert("exp()") }, modifier = Modifier.weight(1f)) { Text("exp(x)") }
                }
                Spacer(Modifier.height(10.dp))
                Text("Cadenas", color = Color(0xFF4CAF50), fontSize = 12.sp, fontWeight = FontWeight.Bold)
                Row(horizontalArrangement = Arrangement.spacedBy(4.dp)) {
                    Button(onClick = { onInsert("Longitud()") }, modifier = Modifier.weight(1f)) { Text("Longitud()") }
                    Button(onClick = { onInsert("Subcadena()") }, modifier = Modifier.weight(1f)) { Text("Subcadena()") }
                }
                Row(horizontalArrangement = Arrangement.spacedBy(4.dp)) {
                    Button(onClick = { onInsert("Mayusculas()") }, modifier = Modifier.weight(1f)) { Text("Mayusculas()") }
                    Button(onClick = { onInsert("Minusculas()") }, modifier = Modifier.weight(1f)) { Text("Minusculas()") }
                }
                Row(horizontalArrangement = Arrangement.spacedBy(4.dp)) {
                    Button(onClick = { onInsert("Concatenar()") }, modifier = Modifier.weight(1f)) { Text("Concatenar()") }
                    Button(onClick = { onInsert("ConvertirANumero()") }, modifier = Modifier.weight(1f)) { Text("ANumero()") }
                    Button(onClick = { onInsert("ConvertirATexto()") }, modifier = Modifier.weight(1f)) { Text("ATexto()") }
                }
                Spacer(Modifier.height(10.dp))
                Text("Constantes", color = Color(0xFF4CAF50), fontSize = 12.sp, fontWeight = FontWeight.Bold)
                Row(horizontalArrangement = Arrangement.spacedBy(4.dp)) {
                    Button(onClick = { onInsert("PI") }, modifier = Modifier.weight(1f)) { Text("PI") }
                    Button(onClick = { onInsert("EULER") }, modifier = Modifier.weight(1f)) { Text("EULER") }
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
    onSelectProfile: (PSeIntProfile) -> Unit,
    onDismiss: () -> Unit
) {
    val context = LocalContext.current
    var searchQuery by remember { mutableStateOf("") }
    
    val allProfiles = remember { PSeIntProfile.loadAllProfiles(context) }
    
    val filteredProfiles = remember(searchQuery, allProfiles) {
        if (searchQuery.isEmpty()) allProfiles
        else allProfiles.filter { it.name.contains(searchQuery, ignoreCase = true) }
    }

    AlertDialog(
        onDismissRequest = onDismiss,
        modifier = Modifier.padding(horizontal = 24.dp).fillMaxWidth(),
        properties = androidx.compose.ui.window.DialogProperties(usePlatformDefaultWidth = false),
        title = { Text("Opciones del Lenguaje (${allProfiles.size} perfiles)", fontSize = 16.sp, fontWeight = FontWeight.Bold) },
        text = {
            Column(modifier = Modifier.fillMaxWidth().heightIn(max = 360.dp)) {
                Text("Puede buscar por nombre de la institución, materia, docente, siglas, etc.", fontSize = 11.sp, color = Color.Gray, modifier = Modifier.padding(bottom = 6.dp))
                OutlinedTextField(
                    value = searchQuery,
                    onValueChange = { searchQuery = it },
                    placeholder = { Text("Buscar: (ej. UNAM, SENA, UTN...)") },
                    leadingIcon = { Icon(Icons.Filled.Search, contentDescription = null) },
                    modifier = Modifier.fillMaxWidth().padding(bottom = 8.dp),
                    singleLine = true
                )
                LazyColumn(modifier = Modifier.weight(1f, fill = false)) {
                    items(filteredProfiles, key = { it.name }) { profile ->
                        Row(
                            verticalAlignment = Alignment.CenterVertically,
                            modifier = Modifier
                                .fillMaxWidth()
                                .clickable { onSelectProfile(profile) }
                                .padding(vertical = 6.dp)
                        ) {
                            RadioButton(
                                selected = selectedProfileName == profile.name,
                                onClick = { onSelectProfile(profile) }
                            )
                            Spacer(Modifier.width(8.dp))
                            Column {
                                Text(profile.name, fontSize = 14.sp, fontWeight = FontWeight.Medium)
                                if (profile.description.isNotEmpty()) {
                                    Text(profile.description.take(80) + if(profile.description.length > 80) "..." else "", fontSize = 11.sp, color = Color.Gray)
                                }
                            }
                        }
                        HorizontalDivider(color = Color(0xFF333344))
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
    textColor: Color,
    surfaceColor: Color,
    onSave: (PSeIntProfile) -> Unit,
    onExport: (PSeIntProfile) -> Unit,
    onImport: () -> Unit,
    onDismiss: () -> Unit
) {
    var forceDefine by remember { mutableStateOf(customProfile.forceDefineVariables) }
    var allowEquals by remember { mutableStateOf(customProfile.allowEqualsAssignment) }
    var requireSemicolon by remember { mutableStateOf(customProfile.requireSemicolons) }

    var profileFontSize by remember { mutableIntStateOf(customProfile.editorFontSize) }
    var uninitializedVariables by remember { mutableStateOf(customProfile.uninitializedVariables) }
    var allowStringConcatenation by remember { mutableStateOf(customProfile.allowStringConcatenation) }
    var enableStringFunctions by remember { mutableStateOf(customProfile.enableStringFunctions) }
    var allowWordOperators by remember { mutableStateOf(customProfile.allowWordOperators) }
    var base0Arrays by remember { mutableStateOf(customProfile.base0Arrays) }
    var dynamicArrays by remember { mutableStateOf(customProfile.dynamicArrays) }
    var allowArrayResize by remember { mutableStateOf(customProfile.allowArrayResize) }
    var allowFunctions by remember { mutableStateOf(customProfile.allowFunctions) }
    var flexibleSyntax by remember { mutableStateOf(customProfile.flexibleSyntax) }
    var colloquialConditions by remember { mutableStateOf(customProfile.colloquialConditions) }
    var restrictSegunToNumeric by remember { mutableStateOf(customProfile.restrictSegunToNumeric) }
    var allowOmitStep1 by remember { mutableStateOf(customProfile.allowOmitStep1) }
    var useNassiShneiderman by remember { mutableStateOf(customProfile.useNassiShneiderman) }
    var alternativeIoShapes by remember { mutableStateOf(customProfile.alternativeIoShapes) }
    var allowAccentsInVariables by remember { mutableStateOf(customProfile.allowAccentsInVariables) }
    var preferAlgoritmo by remember { mutableStateOf(customProfile.preferAlgoritmo) }
    var preferFuncion by remember { mutableStateOf(customProfile.preferFuncion) }
    var allowRepetirMientrasQue by remember { mutableStateOf(customProfile.allowRepetirMientrasQue) }
    var enableParaCada by remember { mutableStateOf(customProfile.enableParaCada) }
    var preferRepetirMientrasQue by remember { mutableStateOf(customProfile.preferRepetirMientrasQue) }
    var protectParaCounter by remember { mutableStateOf(customProfile.protectParaCounter) }

    fun editedProfile() = customProfile.copy(
                    forceDefineVariables = forceDefine,
                    allowEqualsAssignment = allowEquals,
                    requireSemicolons = requireSemicolon,
                    strictTypes = forceDefine,
                    allowImplicitVariables = !forceDefine,
                    editorFontSize = profileFontSize,
                    uninitializedVariables = uninitializedVariables,
                    allowStringConcatenation = allowStringConcatenation,
                    enableStringFunctions = enableStringFunctions,
                    allowWordOperators = allowWordOperators,
                    base0Arrays = base0Arrays,
                    dynamicArrays = dynamicArrays,
                    allowArrayResize = allowArrayResize,
                    allowFunctions = allowFunctions,
                    flexibleSyntax = flexibleSyntax,
                    colloquialConditions = colloquialConditions,
                    restrictSegunToNumeric = restrictSegunToNumeric,
                    allowOmitStep1 = allowOmitStep1,
                    useNassiShneiderman = useNassiShneiderman,
                    alternativeIoShapes = alternativeIoShapes,
                    allowAccentsInVariables = allowAccentsInVariables,
                    preferAlgoritmo = preferAlgoritmo,
                    preferFuncion = preferFuncion,
                    allowRepetirMientrasQue = allowRepetirMientrasQue,
                    enableParaCada = enableParaCada,
                    preferRepetirMientrasQue = preferRepetirMientrasQue,
                    protectParaCounter = protectParaCounter
                ).normalized()

    AlertDialog(
        onDismissRequest = onDismiss,
        title = { Text("Editar Perfil Personalizado", fontWeight = FontWeight.Bold, color = textColor) },
        containerColor = surfaceColor,
        text = {
            Column(modifier = Modifier.verticalScroll(rememberScrollState())) {
                Row(modifier = Modifier.fillMaxWidth(), horizontalArrangement = Arrangement.SpaceEvenly) {
                    Button(onClick = { onExport(editedProfile()) }) { Text("Exportar", fontSize = 12.sp) }
                    Button(onClick = onImport) { Text("Importar", fontSize = 12.sp) }
                }
                Spacer(modifier = Modifier.height(8.dp))
                CustomToggleRow("No permitir utilizar variables o posiciones de arreglos sin inicializar", uninitializedVariables, textColor) { uninitializedVariables = it }
                CustomToggleRow("Obligar a definir los tipos de variables", forceDefine, textColor) { forceDefine = it }
                CustomToggleRow("Controlar el uso de ; al final de sentencias secuenciales", requireSemicolon, textColor) { requireSemicolon = it }
                CustomToggleRow("Permitir concatenar variables de texto con el operador +", allowStringConcatenation, textColor) { allowStringConcatenation = it }
                CustomToggleRow("Habilitar funciones para el manejo de cadenas", enableStringFunctions, textColor) { enableStringFunctions = it }
                CustomToggleRow("Permitir las palabras Y, O, NO, MOD para los operadores &, |, ~, %", allowWordOperators, textColor) { allowWordOperators = it }
                CustomToggleRow("Utilizar indices en arreglos y cadenas en base 0", base0Arrays, textColor) { base0Arrays = it }
                CustomToggleRow("Permitir utilizar variables para dimensionar arreglos", dynamicArrays, textColor) { dynamicArrays = it }
                CustomToggleRow("Permitir redimensionar arreglos", allowArrayResize, textColor) { allowArrayResize = it }
                Text("Tamaño de letra del perfil: $profileFontSize", color = textColor)
                Slider(value = profileFontSize.toFloat(), onValueChange = { profileFontSize = it.toInt() }, valueRange = 10f..24f, steps = 13)
                CustomToggleRow("Permitir asignar con el signo de igual (=)", allowEquals, textColor) { allowEquals = it }
                CustomToggleRow("Permitir definir funciones/subprocesos", allowFunctions, textColor) { allowFunctions = it }
                CustomToggleRow("Utilizar sintaxis flexible", flexibleSyntax, textColor) { flexibleSyntax = it }
                CustomToggleRow("Permitir condiciones en lenguaje coloquial", colloquialConditions, textColor) { colloquialConditions = it }
                CustomToggleRow("Limitar la estructura Segun a variables numericas", restrictSegunToNumeric, textColor) { restrictSegunToNumeric = it }
                CustomToggleRow("Permitir omitir el paso -1 en ciclos Para", allowOmitStep1, textColor) { allowOmitStep1 = it }
                CustomToggleRow("Utilizar diagramas Nassi-Shneiderman", useNassiShneiderman, textColor) { useNassiShneiderman = it }
                CustomToggleRow("Formas alternativas para Entrada/Salida en diagramas", alternativeIoShapes, textColor) { alternativeIoShapes = it }
                CustomToggleRow("Permitir acentos en los nombres de variables", allowAccentsInVariables, textColor) { allowAccentsInVariables = it }
                CustomToggleRow("Preferir la palabra Algoritmo sobre Proceso", preferAlgoritmo, textColor) { preferAlgoritmo = it }
                CustomToggleRow("Preferir la palabra Funcion sobre SubProceso", preferFuncion, textColor) { preferFuncion = it }
                CustomToggleRow("Permitir la palabra Repetir para mientras-que", allowRepetirMientrasQue, textColor) { allowRepetirMientrasQue = it }
                CustomToggleRow("Habilitar estructura Para Cada", enableParaCada, textColor) { enableParaCada = it }
                CustomToggleRow("Preferir repetir mientras que sobre repetir hasta que", preferRepetirMientrasQue, textColor) { preferRepetirMientrasQue = it }
                CustomToggleRow("Proteger contador de ciclos Para", protectParaCounter, textColor) { protectParaCounter = it }
            }
        },
        confirmButton = {
            Button(onClick = {
                onSave(editedProfile())
            }) {
                Text("Guardar")
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
    onFinishOnboarding: () -> Unit,
    surfaceColor: Color,
    bgColor: Color,
    textColor: Color
) {
    var showProfilePicker by remember { mutableStateOf(false) }
    
    Column(
        modifier = Modifier
            .fillMaxSize()
            .background(bgColor)
            .safeDrawingPadding()
            .padding(24.dp)
            .verticalScroll(rememberScrollState()),
        horizontalAlignment = Alignment.CenterHorizontally,
        verticalArrangement = Arrangement.Center
    ) {
        Image(
            painter = painterResource(id = R.drawable.pseint_app_icon),
            contentDescription = "PSeInt Logo",
            modifier = Modifier.size(76.dp).padding(bottom = 12.dp)
        )
        Text("¡Bienvenido a PSeInt Mobile!", fontSize = 20.sp, fontWeight = FontWeight.Bold, color = textColor, textAlign = TextAlign.Center)
        Text("Configuración inicial", fontSize = 12.sp, color = MaterialTheme.colorScheme.onSurfaceVariant, modifier = Modifier.padding(bottom = 20.dp))

        Surface(
            color = surfaceColor,
            shape = RoundedCornerShape(12.dp),
            modifier = Modifier.fillMaxWidth().padding(bottom = 16.dp)
        ) {
            Column(modifier = Modifier.padding(14.dp)) {
                Text("1. Perfil de lenguaje", color = MaterialTheme.colorScheme.primary, fontSize = 13.sp, fontWeight = FontWeight.Bold, modifier = Modifier.padding(bottom = 6.dp))
                
                OutlinedButton(
                    onClick = { showProfilePicker = true },
                    modifier = Modifier.fillMaxWidth().padding(vertical = 8.dp)
                ) {
                    Text("Seleccionar Perfil: ${selectedProfile.name}")
                }
                
                Text(
                    text = "Elige el perfil que use tu docente. Si estás comenzando, puedes usar Flexible y cambiarlo después en Ajustes.",
                    fontSize = 11.sp,
                    color = MaterialTheme.colorScheme.onSurfaceVariant
                )
            }
        }
        
        if (showProfilePicker) {
            ProfilePickerDialog(
                selectedProfileName = selectedProfile.name,
                onSelectProfile = { profile ->
                    onProfileSelected(profile)
                    showProfilePicker = false
                },
                onDismiss = { showProfilePicker = false }
            )
        }

        Surface(
            color = surfaceColor,
            shape = RoundedCornerShape(12.dp),
            modifier = Modifier.fillMaxWidth().padding(bottom = 20.dp)
        ) {
            Column(modifier = Modifier.padding(14.dp)) {
                Text("2. Apariencia", color = MaterialTheme.colorScheme.primary, fontSize = 13.sp, fontWeight = FontWeight.Bold, modifier = Modifier.padding(bottom = 6.dp))
                ThemeOptionRow("PSeInt Oscuro (Noche)", AppTheme.Dark, selectedTheme, textColor) { onThemeSelected(AppTheme.Dark) }
                ThemeOptionRow("PSeInt Clásico (Blanco Escritorio)", AppTheme.LightClassic, selectedTheme, textColor) { onThemeSelected(AppTheme.LightClassic) }
                ThemeOptionRow("Azul Océano (Ocean)", AppTheme.OceanBlue, selectedTheme, textColor) { onThemeSelected(AppTheme.OceanBlue) }
                ThemeOptionRow("Hacker Matrix (Verde / Negro)", AppTheme.HackerMatrix, selectedTheme, textColor) { onThemeSelected(AppTheme.HackerMatrix) }
            }
        }

        Button(
            onClick = onFinishOnboarding,
            colors = ButtonDefaults.buttonColors(),
            shape = RoundedCornerShape(10.dp),
            modifier = Modifier.fillMaxWidth().height(48.dp)
        ) {
            Text("Comenzar a programar", fontSize = 13.sp, fontWeight = FontWeight.Bold)
        }
    }
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
    autocompleteEnabled: Boolean,
    onAutocompleteToggled: (Boolean) -> Unit,
    callTipsEnabled: Boolean,
    onCallTipsToggled: (Boolean) -> Unit,
    onReopenWizard: () -> Unit,
    primaryText: Color,
    surfaceColor: Color,
    bgColor: Color
) {
    Column(
        modifier = Modifier
            .fillMaxSize()
            .background(bgColor)
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
            color = surfaceColor,
            shape = RoundedCornerShape(12.dp),
            modifier = Modifier.fillMaxWidth().padding(bottom = 16.dp)
        ) {
            Column(modifier = Modifier.padding(16.dp)) {
                Text("Perfil actual: ${selectedProfile.name}", color = primaryText, fontSize = 15.sp, fontWeight = FontWeight.Bold)
                Spacer(Modifier.height(8.dp))
                Row(horizontalArrangement = Arrangement.spacedBy(8.dp)) {
                    Button(onClick = onOpenProfilePicker, modifier = Modifier.weight(1f)) {
                        Text("Buscar Perfiles", fontSize = 12.sp)
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
            color = surfaceColor,
            shape = RoundedCornerShape(12.dp),
            modifier = Modifier.fillMaxWidth().padding(bottom = 16.dp)
        ) {
            Column {
                ThemeOptionRow("PSeInt Oscuro (Modo Noche)", AppTheme.Dark, selectedTheme, primaryText) { onThemeSelected(AppTheme.Dark) }
                HorizontalDivider(color = Color(0xFF333344))
                ThemeOptionRow("PSeInt Clásico (Blanco / Escritorio)", AppTheme.LightClassic, selectedTheme, primaryText) { onThemeSelected(AppTheme.LightClassic) }
                HorizontalDivider(color = Color(0xFF333344))
                ThemeOptionRow("Azul Océano (Ocean)", AppTheme.OceanBlue, selectedTheme, primaryText) { onThemeSelected(AppTheme.OceanBlue) }
                HorizontalDivider(color = Color(0xFF333344))
                ThemeOptionRow("Hacker Matrix (Verde / Negro)", AppTheme.HackerMatrix, selectedTheme, primaryText) { onThemeSelected(AppTheme.HackerMatrix) }
            }
        }

        // Diagram Style Card
        Text("Formato de Diagramas", color = Color(0xFF4CAF50), fontSize = 14.sp, fontWeight = FontWeight.Bold, modifier = Modifier.padding(bottom = 6.dp))
        Surface(
            color = surfaceColor,
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
                    Text("Diagrama de Flujo Clásico (Rombos/Paralelogramos)", color = primaryText, fontSize = 13.sp)
                }
                HorizontalDivider(color = Color(0xFF333344))
                Row(
                    verticalAlignment = Alignment.CenterVertically,
                    modifier = Modifier.fillMaxWidth().clickable { onDiagramTypeSelected(DiagramType.NassiShneiderman) }.padding(12.dp)
                ) {
                    RadioButton(selected = diagramType == DiagramType.NassiShneiderman, onClick = { onDiagramTypeSelected(DiagramType.NassiShneiderman) })
                    Spacer(Modifier.width(8.dp))
                    Text("Diagrama Nassi-Shneiderman (Bloques Estructurados)", color = primaryText, fontSize = 13.sp)
                }
            }
        }

        // Editor Options Card
        Text("Opciones de Editor", color = Color(0xFF4CAF50), fontSize = 14.sp, fontWeight = FontWeight.Bold, modifier = Modifier.padding(bottom = 6.dp))
        Surface(
            color = surfaceColor,
            shape = RoundedCornerShape(12.dp),
            modifier = Modifier.fillMaxWidth().padding(bottom = 16.dp)
        ) {
            Column(modifier = Modifier.padding(16.dp)) {
                Row(
                    verticalAlignment = Alignment.CenterVertically,
                    horizontalArrangement = Arrangement.SpaceBetween,
                    modifier = Modifier.fillMaxWidth()
                ) {
                    Text("Tamaño de Fuente (${fontSizeSp}sp)", color = primaryText, fontSize = 14.sp)
                    Row {
                        IconButton(onClick = { if (fontSizeSp > 10) onFontSizeChanged(fontSizeSp - 2) }) {
                            Icon(Icons.Filled.Remove, contentDescription = "Menos", tint = primaryText)
                        }
                        IconButton(onClick = { if (fontSizeSp < 24) onFontSizeChanged(fontSizeSp + 2) }) {
                            Icon(Icons.Filled.Add, contentDescription = "Más", tint = primaryText)
                        }
                    }
                }
                HorizontalDivider(color = Color(0xFF333344), modifier = Modifier.padding(vertical = 8.dp))
                CustomToggleRow("Mostrar Números de Línea", showLineNumbers, primaryText) { onShowLineNumbersToggled(it) }
                CustomToggleRow("Utilizar autocompletado", autocompleteEnabled, primaryText, onAutocompleteToggled)
                Text("Sugiere instrucciones, tipos y nombres mientras escribes. Toca una sugerencia para completarla.", color = primaryText.copy(alpha = .7f), fontSize = 12.sp)
                CustomToggleRow("Utilizar ayudas emergentes", callTipsEnabled, primaryText, onCallTipsToggled)
                Text("Muestra qué argumentos necesita cada instrucción o función.", color = primaryText.copy(alpha = .7f), fontSize = 12.sp)
            }
        }
        
        // Credits Card
        Text("Créditos y Reconocimientos", color = Color(0xFF4CAF50), fontSize = 14.sp, fontWeight = FontWeight.Bold, modifier = Modifier.padding(bottom = 6.dp))
        Surface(
            color = surfaceColor,
            shape = RoundedCornerShape(12.dp),
            modifier = Modifier.fillMaxWidth().padding(bottom = 32.dp)
        ) {
            Column(modifier = Modifier.padding(16.dp)) {
                Text(
                    text = "Esta aplicación (PSeInt Mobile) está inspirada en el proyecto original PSeInt para escritorio.",
                    color = primaryText,
                    fontSize = 13.sp,
                    lineHeight = 18.sp
                )
                Spacer(Modifier.height(8.dp))
                Text(
                    text = "Creador original de PSeInt:",
                    color = Color.Gray,
                    fontSize = 12.sp,
                    fontWeight = FontWeight.Bold
                )
                Text(
                    text = "Pablo Novara",
                    color = Color(0xFF64B5F6),
                    fontSize = 16.sp,
                    fontWeight = FontWeight.Bold,
                    modifier = Modifier.padding(vertical = 4.dp)
                )
                Text(
                    text = "Proyecto creado en 2003 (Universidad Nacional del Litoral, Argentina).",
                    color = Color.Gray,
                    fontSize = 11.sp
                )
            }
        }
    }
}

@Composable
fun ThemeOptionRow(title: String, theme: AppTheme, selectedTheme: AppTheme, textColor: Color, onClick: () -> Unit) {
    Row(
        verticalAlignment = Alignment.CenterVertically,
        modifier = Modifier.fillMaxWidth().clickable(onClick = onClick).padding(12.dp)
    ) {
        RadioButton(selected = theme == selectedTheme, onClick = onClick)
        Spacer(Modifier.width(8.dp))
        Text(title, color = textColor, fontSize = 13.sp)
    }
}

@Composable
fun CustomToggleRow(title: String, checked: Boolean, textColor: Color, onCheckedChange: (Boolean) -> Unit) {
    Row(
        verticalAlignment = Alignment.CenterVertically,
        horizontalArrangement = Arrangement.SpaceBetween,
        modifier = Modifier.fillMaxWidth().padding(vertical = 6.dp)
    ) {
        Text(title, color = textColor, fontSize = 13.sp, modifier = Modifier.weight(1f).padding(end = 12.dp))
        Switch(
            checked = checked,
            modifier = Modifier.semantics { contentDescription = title },
            onCheckedChange = onCheckedChange,
            colors = SwitchDefaults.colors(
                checkedThumbColor = Color.White,
                checkedTrackColor = Color(0xFF4CAF50),
                uncheckedThumbColor = Color(0xFFB0B0B0),
                uncheckedTrackColor = Color(0xFFE0E0E0),
                uncheckedBorderColor = Color.Transparent
            )
        )
    }
}
