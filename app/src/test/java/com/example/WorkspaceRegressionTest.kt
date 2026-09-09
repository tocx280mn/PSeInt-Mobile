package com.example

import androidx.compose.ui.text.TextRange
import androidx.compose.ui.text.input.TextFieldValue
import org.junit.Assert.*
import org.junit.Test

class WorkspaceRegressionTest {
    @Test fun commandsStayInsideAlgorithmAndKeepLaterSubprograms() {
        val source = "Algoritmo T\n\nFinAlgoritmo\n\nSubProceso Saludo\nEscribir 1\nFinSubProceso"
        val cursor = source.indexOf("FinAlgoritmo")
        val result = insertEditorCommand(TextFieldValue(source, TextRange(cursor)), "Leer x;").text
        assertTrue(result.indexOf("Leer x;") < result.indexOf("FinAlgoritmo"))
        assertTrue(result.endsWith("SubProceso Saludo\nEscribir 1\nFinSubProceso"))
    }
    @Test fun insertionIntoBlankLineDoesNotDuplicateIndentation() {
        val source = "Algoritmo T\n    \nFinAlgoritmo"
        val result = insertEditorCommand(TextFieldValue(source, TextRange(source.indexOf("    ") + 4)), "Escribir 1;")
        assertEquals("Algoritmo T\n    Escribir 1;\nFinAlgoritmo", result.text)
    }
    @Test fun newFunctionsAreOutsideMainAlgorithm() {
        val source = "Algoritmo T\n\nFinAlgoritmo"
        val result = insertEditorCommand(TextFieldValue(source, TextRange(12)), "Funcion Saludo\nFinFuncion")
        assertTrue(result.text.indexOf("Funcion Saludo") > result.text.indexOf("FinAlgoritmo"))
    }
    @Test fun trailingEmptyLinesCannotPutCommandsAfterFinAlgoritmo() {
        val source = "Algoritmo T\nFinAlgoritmo\n\n"
        val result = insertEditorCommand(TextFieldValue(source, TextRange(source.length)), "Escribir 1;").text
        assertTrue(result.indexOf("Escribir 1;") < result.indexOf("FinAlgoritmo"))
        assertTrue(PSeIntEvaluator().checkSyntax(result, PSeIntProfile.Flexible).isValid)
    }
    @Test fun legacyAndUnicodeDocumentsKeepAccentsAndNormalizeLines() {
        val source = "Algoritmo Prueba\r\nEscribir \"niño, acción\"\r\nFinAlgoritmo"
        val expected = source.replace("\r\n", "\n")
        listOf(Charsets.UTF_8, Charsets.UTF_16, charset("windows-1252")).forEach {
            assertEquals(it.name(), expected, decodePSeIntDocument(source.toByteArray(it)))
        }
    }
    @Test fun insertionBeforeRepeatWhileClosingStaysInsideLoop() {
        val source = "Algoritmo T\n    Repetir\n    Mientras Que Falso\nFinAlgoritmo"
        val cursor = source.indexOf("Mientras Que")
        val result = insertEditorCommand(TextFieldValue(source, TextRange(cursor)), "Escribir 1").text
        assertEquals("Algoritmo T\n    Repetir\n        Escribir 1\n    Mientras Que Falso\nFinAlgoritmo", result)
        assertTrue(PSeIntEvaluator().checkSyntax(result, PSeIntProfile.Flexible).isValid)
    }
    @Test fun bundledExamplesExecuteWithoutErrors() = kotlinx.coroutines.test.runTest {
        for (example in PSeIntExamples.all) {
            val output = mutableListOf<String>()
            PSeIntEvaluator().evaluate(example.code, PSeIntProfile.Flexible, onOutput = output::add,
                onRequestInput = { if (it == "nombre") "Ada" else "0" }, onFinish = {})
            assertFalse("${example.name}: $output", output.any { it.startsWith("Error") })
        }
    }
}
