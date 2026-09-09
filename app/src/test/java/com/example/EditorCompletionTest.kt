package com.example

import androidx.compose.ui.text.TextRange
import androidx.compose.ui.text.input.TextFieldValue
import org.junit.Assert.*
import org.junit.Test

class EditorCompletionTest {
    private fun value(code: String): TextFieldValue {
        val cursor = code.indexOf('|').let { if (it < 0) code.length else it }
        return TextFieldValue(code.replace("|", ""), TextRange(cursor))
    }
    private fun labels(code: String, profile: PSeIntProfile = PSeIntProfile.Flexible) = editorCompletions(value(code), profile).map { it.label }

    @Test fun desktopExamplesAndCaseInsensitivePrefixes() {
        assertEquals(listOf("Escribir"), labels("Algoritmo T\n    eSc"))
        assertEquals(listOf("Como Caracter", "Como Entero", "Como Logico", "Como Real"), labels("Definir variable Com"))
        assertEquals(listOf("Como Caracter", "Como Entero", "Como Logico", "Como Real"), labels("Definir variable Como "))
        assertEquals(listOf("Como Real"), labels("Definir variable Como Re"))
        assertTrue(labels("Escribir Com").none { it.startsWith("Como ") })
    }
    @Test fun completionPreservesSurroundingCodeAndReplacesWholeWord() {
        val source = value("Algoritmo T\n    Esc|ribir \"Hola\"\nFinAlgoritmo")
        val result = applyEditorCompletion(source, editorCompletions(source, PSeIntProfile.Flexible).single())
        assertEquals("Algoritmo T\n    Escribir \"Hola\"\nFinAlgoritmo", result.text)
        assertEquals(result.text.indexOf("Escribir") + 8, result.selection.end)
    }
    @Test fun strictTypeCompletionAddsSemicolonOnce() {
        val source = value("Definir n Com|;\nEscribir n;")
        val choice = editorCompletions(source, PSeIntProfile.Estricto).first { it.label == "Como Entero" }
        assertEquals("Definir n Como Entero;\nEscribir n;", applyEditorCompletion(source, choice).text)
    }
    @Test fun ignoredStringsCommentsSelectionsAndOneLetter() {
        listOf("Escribir \"Esc", "Escribir 'Esc", "// Esc", "Escribir \"a\\\"Esc", "E").forEach { assertTrue(it, labels(it).isEmpty()) }
        val selected = TextFieldValue("Escribir", TextRange(0, 3))
        assertTrue(editorCompletions(selected, PSeIntProfile.Flexible).isEmpty())
        assertNull(editorCallTip(selected, PSeIntProfile.Flexible))
        assertEquals(listOf("Escribir"), labels("// comentario\nEsc"))
        assertEquals(listOf("Escribir"), labels("Escribir \"; //\"; Esc"))
    }
    @Test fun respectsDisabledProfileFeatures() {
        val restricted = PSeIntProfile.Flexible.copy(allowFunctions = false, flexibleSyntax = false,
            enableStringFunctions = false, allowArrayResize = false, enableParaCada = false, allowRepetirMientrasQue = false)
        assertTrue(labels("Fu", restricted).isEmpty())
        assertFalse(labels("Al", restricted).contains("Algoritmo"))
        assertFalse(labels("Mo", restricted).contains("Mostrar"))
        assertFalse(labels("Informar dato Sin", restricted).contains("Sin Saltar"))
        assertTrue(labels("Escribir dato Sin", restricted).contains("Sin Saltar"))
        assertTrue(labels("Mo").contains("Mostrar"))
        assertTrue(labels("Escribir Subc", restricted).isEmpty())
        assertTrue(labels("Redi", restricted).isEmpty())
        assertTrue(labels("Para Ca", restricted).isEmpty())
        assertTrue(labels("Mientras Qu", restricted).isEmpty())
        assertTrue(labels("Escribir Subc").contains("Subcadena"))
    }
    @Test fun existingNamesAndFunctionParentheses() {
        assertTrue(labels("Definir contador Como Entero\nEscribir cont").contains("contador"))
        assertTrue(labels("Definir contador Como Entero\ncont").contains("contador"))
        assertFalse(labels("// contador\nEscribir cont").contains("contador"))
        assertTrue(labels("Definir valor Como Entero\nEscribir val").contains("valor"))
        val source = value("Escribir Subc|(texto, 1, 2)")
        val result = applyEditorCompletion(source, editorCompletions(source, PSeIntProfile.Flexible).first { it.label == "Subcadena" })
        assertEquals("Escribir Subcadena(texto, 1, 2)", result.text)
        assertEquals(result.text.indexOf('(') + 1, result.selection.end)
        assertNull(result.composition)
    }
    @Test fun contextualArgumentHints() {
        assertTrue(editorCallTip(value("Escribir "), PSeIntProfile.Flexible)!!.contains("expresiones"))
        assertEquals("subcadena(cadena, posición inicial, posición final)", editorCallTip(value("Escribir Subcadena("), PSeIntProfile.Flexible))
        assertEquals("azar(límite superior (excluido))", editorCallTip(value("Escribir Subcadena(texto, Azar("), PSeIntProfile.Flexible))
        assertTrue(editorCallTip(value("Escribir \"listo\"; Leer "), PSeIntProfile.Flexible)!!.contains("variables"))
        assertNull(editorCallTip(value("Escribir // comentario"), PSeIntProfile.Flexible))
        assertNull(editorCallTip(value("Escribir \"texto"), PSeIntProfile.Flexible))
    }

    @Test fun datesAndTimesInsertEmptyParenthesesOnlyOnce() {
        for (name in listOf("FechaActual", "HoraActual")) {
            for (suffix in listOf("", "()", "(  )")) {
                val source = value("Escribir ${name.dropLast(2)}|$suffix")
                val completed = applyEditorCompletion(source, editorCompletions(source, PSeIntProfile.Flexible).first { it.label == name })
                assertEquals("Escribir $name" + suffix.ifEmpty { "()" }, completed.text)
                assertNull(completed.composition)
            }
        }
    }
    @Test fun colloquialAndRelaxedDeclarationsHaveDifferentContexts() {
        assertTrue(labels("Si n Es ").size > 12)
        assertTrue(labels("Si n Es ").contains("Es Positivo"))
        assertTrue(labels("Si n Es Mayor O I").contains("Es Mayor O Igual A"))
        assertTrue(labels("n Es Re").contains("Es Real"))
        assertFalse(labels("Si n Es Re").contains("Es Real"))
        assertTrue(labels("a, b Son En").contains("Son Enteros"))
        assertFalse(labels("Si n Es P", PSeIntProfile.Flexible.copy(colloquialConditions = false)).contains("Es Par"))
        assertFalse(labels("n Es Re", PSeIntProfile.Flexible.copy(flexibleSyntax = false)).contains("Es Real"))
    }
    @Test fun accentedKeywordsAndAliasesKeepCanonicalSuggestions() {
        assertTrue(labels("Funció").contains("Funcion"))
        assertTrue(labels("Escribir Mayúsc").contains("Mayusculas"))
        assertTrue(labels("SubProceso T(n Por Cop").contains("Por Valor"))
        assertTrue(labels("Definir n Como Numér").contains("Como Real"))
        assertTrue(labels("Escribir n Sin Baj").contains("Sin Saltar"))
        assertTrue(labels("HastaQu").contains("Hasta Que"))
        assertTrue(labels("Para Cada n E").contains("De"))
    }
    @Test fun shortCommandsAndAllClosingCommandsRemainReachable() {
        assertTrue(labels("S").contains("Si"))
        assertTrue(labels("Escribir R").contains("RC"))
        assertTrue(labels("Fin").size > 12)
        assertTrue(labels("Fin").contains("FinSubProceso"))
        assertTrue(labels("Fin SubP").contains("Fin SubProceso"))
        assertTrue(labels("Fin SubP", PSeIntProfile.Flexible.copy(allowFunctions = false)).isEmpty())
        assertTrue(labels("Fin SubP", PSeIntProfile.Flexible.copy(flexibleSyntax = false)).isEmpty())
    }
    @Test fun disabledKeywordsCannotReappearAsDocumentNames() {
        val profile = PSeIntProfile.Flexible.copy(enableStringFunctions = false, allowFunctions = false)
        assertFalse(labels("Longitud(texto)\nEscribir Long", profile).contains("Longitud"))
        assertFalse(labels("Funcion T\nFu", profile).contains("Funcion"))
        assertTrue(labels("Escribir FechaA", profile).contains("FechaActual"))
        assertTrue(labels("Escribir HoraA", profile).contains("HoraActual"))
    }
    @Test fun newFunctionsAreHighlightedAndHintsRespectProfiles() {
        val code = "Escribir FechaActual(), HoraActual()"
        val functionNames = tokenizePSeInt(code).filter { it.kind == SyntaxTokenKind.Function }.map { code.substring(it.start, it.end) }
        assertEquals(listOf("FechaActual", "HoraActual"), functionNames)
        assertTrue(editorCallTip(value("Escribir FechaActual()"), PSeIntProfile.Flexible)!!.contains("AAAAMMDD"))
        assertTrue(editorCallTip(value("Para i <- 1 Hasta "), PSeIntProfile.Flexible)!!.contains("valor final"))
        assertTrue(editorCallTip(value("Para i <- 1 Hasta 5 Con Paso "), PSeIntProfile.Flexible)!!.contains("valor del paso"))
        assertTrue(editorCallTip(value("Hasta Que "), PSeIntProfile.Flexible)!!.contains("condición"))
    }
    @Test fun completedProgramsAreAcceptedByTheOriginalEngine() {
        val samples = listOf(
            "Algoritmo T\nEscribir FechaA|\nFinAlgoritmo" to "FechaActual",
            "Algoritmo T\nEscribir HoraA|\nFinAlgoritmo" to "HoraActual",
            "Algoritmo T\nSi 2 Es Mayor O I| 1 Entonces\nEscribir 1\nFinSi\nFinAlgoritmo" to "Es Mayor O Igual A",
            "Algoritmo T\nn Es Re|\nn <- 1\nEscribir n\nFinAlgoritmo" to "Es Real"
        )
        for ((source, label) in samples) {
            val input = value(source)
            val result = applyEditorCompletion(input, editorCompletions(input, PSeIntProfile.Flexible).first { it.label == label })
            val syntax = PSeIntEvaluator().checkSyntax(result.text, PSeIntProfile.Flexible)
            assertTrue("$label: $syntax\n${result.text}", syntax.isValid)
        }
    }
}
