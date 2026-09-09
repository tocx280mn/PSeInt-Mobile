package com.example

import androidx.compose.ui.text.TextRange
import androidx.compose.ui.text.input.TextFieldValue
import org.junit.Assert.assertEquals
import org.junit.Test

class EditorOperationsTest {
    @Test fun insertionReplacesSelectionAndLeavesCursorAtInsertion() {
        val result = insertEditorText(TextFieldValue("Leer dato", TextRange(5, 9)), "nombre")
        assertEquals("Leer nombre", result.text)
        assertEquals(TextRange(11), result.selection)
    }

    @Test fun insertionHandlesReverseSelection() {
        val result = insertEditorText(TextFieldValue("Escribir dato", TextRange(13, 9)), "42")
        assertEquals("Escribir 42", result.text)
        assertEquals(TextRange(11), result.selection)
    }

    @Test fun lineAndColumnIncludeBlankLines() {
        assertEquals(3 to 2, editorLineColumn(TextFieldValue("uno\n\nx", TextRange(6))))
        assertEquals(1 to 1, editorLineColumn(TextFieldValue("")))
    }

    @Test fun newlineInheritsIndentationAndOpensBlock() {
        val previous = TextFieldValue("    Si edad >= 18 Entonces", TextRange(26))
        val next = TextFieldValue(previous.text + "\n", TextRange(previous.text.length + 1))
        val result = autoIndentEditorChange(previous, next)
        assertEquals(previous.text + "\n        ", result.text)
        assertEquals(TextRange(result.text.length), result.selection)
    }

    @Test fun pasteIsNotAutoIndented() {
        val previous = TextFieldValue("Algoritmo A", TextRange(11))
        val next = TextFieldValue("Algoritmo A\nLeer x", TextRange(18))
        assertEquals(next, autoIndentEditorChange(previous, next))
    }

    @Test fun selectedLinesIndentWithoutAffectingFollowingLine() {
        val result = indentEditorSelection(TextFieldValue("Leer a\nLeer b\nFinAlgoritmo", TextRange(0, 14)))
        assertEquals("    Leer a\n    Leer b\nFinAlgoritmo", result.text)
    }

    @Test fun shiftTabPreservesCursorWhenNoIndentExists() {
        val original = TextFieldValue("Leer a", TextRange(3))
        assertEquals(original, indentEditorSelection(original, unindent = true))
    }

    @Test fun formatterTracksBranchesInsideSwitchAndPreservesLiteralColons() {
        val source = "Algoritmo A\nSegun opcion Hacer\n1:\nEscribir \"https://ejemplo:80\"\nSi x Entonces\nLeer x\nSino\nEscribir x\nFinSi\n2:\nEscribir \"dos\"\nDe Otro Modo:\nEscribir \"otro\"\nFinSegun\nFinAlgoritmo"
        val expected = "Algoritmo A\n    Segun opcion Hacer\n        1:\n            Escribir \"https://ejemplo:80\"\n            Si x Entonces\n                Leer x\n            Sino\n                Escribir x\n            FinSi\n        2:\n            Escribir \"dos\"\n        De Otro Modo:\n            Escribir \"otro\"\n    FinSegun\nFinAlgoritmo"
        assertEquals(expected, formatPSeIntCode(source))
        assertEquals(expected, formatPSeIntCode(expected))
    }

    @Test fun formatterPreservesQuotedWhitespaceAndIgnoresBlockNamesInComments() {
        val source = "Algoritmo A\n// Si x Entonces\nEscribir \"a  b\" // Repetir\nRepetir\nLeer x\nHasta Que x = 1\nFin Algoritmo"
        assertEquals("Algoritmo A\n    // Si x Entonces\n    Escribir \"a  b\" // Repetir\n    Repetir\n        Leer x\n    Hasta Que x = 1\nFin Algoritmo", formatPSeIntCode(source))
    }
}
