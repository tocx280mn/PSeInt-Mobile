package com.example

import androidx.compose.foundation.Image
import androidx.compose.foundation.clickable
import androidx.compose.foundation.horizontalScroll
import androidx.compose.foundation.layout.*
import androidx.compose.foundation.rememberScrollState
import androidx.compose.material3.MaterialTheme
import androidx.compose.material3.Text
import androidx.compose.runtime.Composable
import androidx.compose.ui.Alignment
import androidx.compose.ui.Modifier
import androidx.compose.ui.res.painterResource
import androidx.compose.ui.text.TextRange
import androidx.compose.ui.text.input.TextFieldValue
import androidx.compose.ui.unit.dp
import androidx.compose.ui.unit.sp

data class PSeIntCommand(val name: String, val icon: Int, val code: String)
val workspaceCommands = listOf(
    PSeIntCommand("Escribir", R.drawable.escribir, "Escribir \"\";"),
    PSeIntCommand("Leer", R.drawable.leer, "Leer variable;"),
    PSeIntCommand("Asignar", R.drawable.asignar, "variable <- 0;"),
    PSeIntCommand("Definir", R.drawable.asignar, "Definir variable Como Entero;"),
    PSeIntCommand("Si", R.drawable.si, "Si condicion Entonces\n    // Verdadero\nSiNo\n    // Falso\nFinSi"),
    PSeIntCommand("Según", R.drawable.segun, "Segun variable Hacer\n    1:\n        Escribir \"Uno\";\n    De Otro Modo:\n        Escribir \"Otro valor\";\nFinSegun"),
    PSeIntCommand("Mientras", R.drawable.mientras, "Mientras condicion Hacer\n    // Instrucciones\nFinMientras"),
    PSeIntCommand("Repetir", R.drawable.repetir, "Repetir\n    // Instrucciones\nHasta Que condicion"),
    PSeIntCommand("Para", R.drawable.para, "Para i <- 1 Hasta 10 Con Paso 1 Hacer\n    Escribir i;\nFinPara"),
    PSeIntCommand("Dimensión", R.drawable.asignar, "Dimension arreglo[10];"),
    PSeIntCommand("Función", R.drawable.funcion, "Funcion resultado <- Doble(numero)\n    resultado <- numero * 2;\nFinFuncion")
)

fun workspaceCommandsFor(profile: PSeIntProfile): List<PSeIntCommand> = buildList {
    workspaceCommands.filter { it.name != "Función" || profile.allowFunctions }.forEach { command ->
        var code = command.code
        var name = command.name
        if (name == "Función") {
            if (profile.forceDefineVariables) code = code.replace("    resultado <-", "    Definir resultado Como Real;\n    resultado <-")
            if (!profile.preferFuncion) { name = "Subproceso"; code = code.replace("Funcion", "SubProceso") }
        }
        if (name == "Repetir" && profile.allowRepetirMientrasQue && profile.preferRepetirMientrasQue) code = code.replace("Hasta Que", "Mientras Que")
        if (!profile.requireSemicolons) code = code.lines().joinToString("\n") { it.removeSuffix(";") }
        add(command.copy(name = name, code = code))
    }
    val end = if (profile.requireSemicolons) ";" else ""
    if (profile.allowArrayResize) add(PSeIntCommand("Redimensión", R.drawable.asignar, "Redimension arreglo[20]$end"))
    if (profile.enableParaCada) add(PSeIntCommand("Para cada", R.drawable.para, "Para Cada elemento De arreglo Hacer\n    Escribir elemento$end\nFinPara"))
}

@Composable
fun CommandPalette(profile: PSeIntProfile = PSeIntProfile.Flexible, onInsert: (String) -> Unit) {
    Row(Modifier.fillMaxWidth().horizontalScroll(rememberScrollState()).padding(horizontal = 6.dp, vertical = 3.dp)) {
        workspaceCommandsFor(profile).forEach { command ->
            Column(Modifier.width(68.dp).height(55.dp).clickable { onInsert(command.code) },
                horizontalAlignment = Alignment.CenterHorizontally, verticalArrangement = Arrangement.Center) {
                Image(painterResource(command.icon), null, Modifier.width(38.dp).height(26.dp))
                Text(command.name, fontSize = 10.sp, color = MaterialTheme.colorScheme.onSurface)
            }
        }
    }
}

/** Insert complete statements on a code line, keeping closing tags and subprograms intact. */
fun insertEditorCommand(value: TextFieldValue, command: String): TextFieldValue {
    val source = value.text
    if (command.startsWith("Funcion ", true) || command.startsWith("SubProceso ", true)) {
        val result = source.trimEnd() + "\n\n" + command + "\n"
        return TextFieldValue(result, TextRange(result.length))
    }
    if (!value.selection.collapsed) return insertEditorText(value, command)
    val cursor = value.selection.start.coerceIn(0, source.length)
    var start = source.lastIndexOf('\n', (cursor - 1).coerceAtLeast(-1)) + 1
    var end = source.indexOf('\n', start).let { if (it < 0) source.length else it }
    val line = source.substring(start, end)
    val trimmed = line.trim()
    // Files commonly end with a blank line after FinAlgoritmo. A command inserted
    // there belongs to the main body, not to the text after the closing tag.
    if (trimmed.isEmpty()) {
        val previousHeader = Regex("(?im)^[ \\t]*(?:Fin)?(?:Algoritmo|Proceso|SubProceso|Funcion)\\b")
            .findAll(source.substring(0, start)).lastOrNull()?.value?.trim()
        if (previousHeader?.startsWith("Fin", true) == true) {
            val mainEnd = Regex("(?im)^[ \\t]*Fin(?:Algoritmo|Proceso)\\b").find(source)
            if (mainEnd != null) return insertEditorCommand(value.copy(selection = TextRange(mainEnd.range.first)), command)
        }
    }
    var indentation = line.takeWhile { it == ' ' || it == '\t' }.replace("\t", "    ")
    if (trimmed.startsWith("Algoritmo ", true) || trimmed.startsWith("Proceso ", true)) {
        start = if (end < source.length) end + 1 else end
        end = start
        indentation = "    "
    } else if (Regex("(?i)^(fin\\w*|sino|(?:hasta|mientras)\\s+que)\\b").containsMatchIn(trimmed)) {
        end = start
        indentation += "    "
    } else if (trimmed.isNotEmpty()) {
        start = end
        end = start
    }
    if (indentation.isEmpty()) indentation = "    "
    val insertion = command.lines().joinToString("\n") { indentation + it }
    val before = source.substring(0, start)
    val after = source.substring(end)
    val prefix = if (before.isNotEmpty() && !before.endsWith('\n')) "\n" else ""
    val suffix = if (after.startsWith('\n')) "" else "\n"
    val result = before + prefix + insertion + suffix + after
    return TextFieldValue(result, TextRange(before.length + prefix.length + insertion.length))
}

data class PSeIntExample(val name: String, val description: String, val code: String)
object PSeIntExamples {
    val all = listOf(
        PSeIntExample("Bienvenida", "Entrada y salida de texto", WorkspaceViewModel.WELCOME_CODE),
        PSeIntExample("MayorDeEdad", "Decisiones con Si / SiNo", """Algoritmo MayorDeEdad
    Definir edad Como Entero;
    Escribir "Ingresa tu edad:";
    Leer edad;
    Si edad >= 18 Entonces
        Escribir "Eres mayor de edad";
    SiNo
        Escribir "Eres menor de edad";
    FinSi
FinAlgoritmo"""),
        PSeIntExample("TablaDeMultiplicar", "Ciclo Para y operaciones", """Algoritmo TablaDeMultiplicar
    Definir numero, i Como Entero;
    Escribir "¿Qué tabla quieres consultar?";
    Leer numero;
    Para i <- 1 Hasta 10 Con Paso 1 Hacer
        Escribir numero, " x ", i, " = ", numero * i;
    FinPara
FinAlgoritmo"""),
        PSeIntExample("MenuDeOpciones", "Repetir y selección múltiple", """Algoritmo MenuDeOpciones
    Definir opcion Como Entero;
    Repetir
        Escribir "1. Saludar   2. Despedirse   0. Salir";
        Leer opcion;
        Segun opcion Hacer
            1:
                Escribir "¡Hola!";
            2:
                Escribir "¡Hasta pronto!";
            De Otro Modo:
                Escribir "Fin del menú";
        FinSegun
    Hasta Que opcion = 0
FinAlgoritmo""")
    )
}
