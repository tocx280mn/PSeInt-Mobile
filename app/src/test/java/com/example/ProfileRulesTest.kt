package com.example

import org.junit.Assert.*
import org.junit.Test

class ProfileRulesTest {
    @Test fun desktopKeysAndHistoricalMigrations() {
        val old = PSeIntProfile.loadFromPrf("Antiguo", "lazy_syntax=0")
        assertFalse(old.allowRepetirMientrasQue)
        assertFalse(old.enableParaCada)
        assertTrue(old.restrictSegunToNumeric)
        assertFalse(old.allowOmitStep1)
        assertFalse(old.preferAlgoritmo)
        val modern = PSeIntProfile.loadFromPrf("Actual", """
            version=20230211
            word_operators=0
            coloquial_conditions=0
            integer_only_switch=1
            deduce_negative_for_step=0
            use_nassi_shneiderman=1
            allow_repeat_while=0
            allow_for_each=0
            prefer_repeat_while=1
            protect_for_counter=0
        """.trimIndent())
        assertFalse(modern.allowWordOperators)
        assertTrue(modern.restrictSegunToNumeric)
        assertFalse(modern.allowOmitStep1)
        assertTrue(modern.useNassiShneiderman)
        assertFalse(modern.allowRepetirMientrasQue)
        assertFalse(modern.preferRepetirMientrasQue)
        assertFalse(modern.enableParaCada)
        assertFalse(modern.protectParaCounter)
    }

    @Test fun everyProfileFlagSurvivesDesktopExportAndImport() {
        for (i in 0 until 24) {
            val flags = CharArray(24) { if (it == i) '1' else '0' }.concatToString()
            val profile = PSeIntProfile.Flexible.withNativeFlags(flags).copy(name="Docente", editorFontSize=19, description="Árbol\nSegundo renglón")
            val restored = PSeIntProfile.importProfileFromString(profile.toDesktopProfile())
            assertEquals("Flag $i", profile, restored)
        }
    }

    @Test fun preferencesChangeTemplatesWithoutForbiddingSynonyms() {
        val p = PSeIntProfile.Flexible.copy(preferAlgoritmo=false, preferFuncion=false, requireSemicolons=true,
            useNassiShneiderman=true, preferRepetirMientrasQue=true)
        assertTrue(p.newProgram().startsWith("Proceso"))
        val commands = workspaceCommandsFor(p)
        assertTrue(commands.first { it.name=="Subproceso" }.code.endsWith("FinSubProceso"))
        assertTrue(commands.first { it.name=="Repetir" }.code.contains("Mientras Que"))
        assertTrue(commands.first { it.name=="Escribir" }.code.endsWith(';'))
        assertTrue(PSeIntEvaluator().checkSyntax("Algoritmo T\nEscribir 1;\nFinAlgoritmo", p).isValid)
        assertTrue(PSeIntEvaluator().checkSyntax("Proceso T\nEscribir 1;\nFinProceso", p.copy(preferAlgoritmo=true)).isValid)
    }

    @Test fun disablingFeaturesRemovesTheirCommandsAndHighlighting() {
        val p = PSeIntProfile.Flexible.copy(allowFunctions=false, allowArrayResize=false, enableParaCada=false,
            enableStringFunctions=false, allowWordOperators=false, colloquialConditions=false)
        assertFalse(workspaceCommandsFor(p).any { it.name in listOf("Función", "Redimensión", "Para cada") })
        val source = "Funcion Longitud(miTexto) Y Redimensionar"
        val tokens = tokenizePSeInt(source, p)
        assertTrue(tokens.none { it.kind in listOf(SyntaxTokenKind.Keyword, SyntaxTokenKind.Function) })
    }

    @Test fun profileDifferencesAffectActualExecutionValidation() {
        val snippets = listOf(
            "Escribir \"a\" + \"b\"" to PSeIntProfile.Flexible.copy(allowStringConcatenation=false),
            "Escribir Longitud(\"abc\")" to PSeIntProfile.Flexible.copy(enableStringFunctions=false),
            "x = 1" to PSeIntProfile.Flexible.copy(allowEqualsAssignment=false),
            "Escribir 1" to PSeIntProfile.Flexible.copy(requireSemicolons=true),
            "Dimension a[2]\nPara Cada x De a Hacer\nFinPara" to PSeIntProfile.Flexible.copy(enableParaCada=false),
            "Repetir\nMientras Que Falso" to PSeIntProfile.Flexible.copy(allowRepetirMientrasQue=false),
            "n <- 3\nDimension a[n]" to PSeIntProfile.Flexible.copy(dynamicArrays=false),
            "Si 2 Es Par Entonces\nFinSi" to PSeIntProfile.Flexible.copy(colloquialConditions=false)
        )
        snippets.forEach { (body, restricted) ->
            val code = "Algoritmo T\n$body\nFinAlgoritmo"
            assertTrue(body, PSeIntEvaluator().checkSyntax(code,PSeIntProfile.Flexible).isValid)
            assertFalse(body, PSeIntEvaluator().checkSyntax(code,restricted).isValid)
        }
    }
}
