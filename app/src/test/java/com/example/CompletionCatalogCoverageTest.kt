package com.example

import androidx.compose.ui.text.TextRange
import androidx.compose.ui.text.input.TextFieldValue
import org.junit.Assert.*
import org.junit.Test
import java.io.File
import java.nio.charset.Charset

/** Read the supplied sources independently; new desktop entries must fail this audit until covered. */
class CompletionCatalogCoverageTest {
    private val all = PSeIntProfile.Flexible
    private fun source(path: String): String = File("../pseint/$path").readText(Charset.forName("windows-1252"))
    private fun completions(text: String, profile: PSeIntProfile = all) = editorCompletions(TextFieldValue(text, TextRange(text.length)), profile)
    private fun normalized(text: String) = text.trim().trimEnd(':').foldCompletion()
    private fun spellings() = completionWords(all).flatMap { it.spellings }.map(::normalized).toSet()

    @Test fun everyDesktopCompletionIsPresentAndReachable() {
        val body = source("wxPSeInt/mxSource.cpp").substringAfter("void mxSource::SetAutocompletion() {").substringBefore("void mxSource::ReloadFromTempPSD")
        val rows = Regex("comp_list_item\\(\"([^\"]+)\",\"(?:\\\\.|[^\"])*\",\"([^\"]*)\"\\)")
            .findAll(body.lines().filterNot { it.trimStart().startsWith("//") }.joinToString("\n"))
            .map { it.groupValues[1].trim() to it.groupValues[2] }.distinct().toList()
        assertTrue("Desktop catalog was not parsed", rows.size > 90)
        val present = spellings()
        for ((label, context) in rows) {
            assertTrue("Missing desktop entry: $label", normalized(label) in present)
            val preceding = when (context.lowercase()) {
                "" -> ""
                "*" -> if (label.startsWith("Es ")) "Si dato " else "Escribir "
                "es", "son" -> "dato "
                "definir" -> "Definir dato "
                "funcion", "subproceso", "subalgoritmo" -> "$context Prueba(dato "
                else -> "$context dato "
            }
            val prefix = label.dropLast(1)
            assertTrue("Unreachable desktop entry: $preceding$prefix → $label",
                completions(preceding + prefix).any { normalized(it.label) == normalized(label) })
        }
        report("desktop-completions", rows.map { "${it.first}\t${it.second}" })
    }

    @Test fun everyEngineKeywordAndAliasCanBeCompleted() {
        val body = source("pseint/Keywords.cpp").substringAfter("void initKeywords").substringBefore("for(const auto &key")
        val rows = Regex("keywords\\[(KW_[A-Z_]+)] = \"([^\"]+)\"").findAll(body)
            .flatMap { match -> match.groupValues[2].split(',').filter { it.isNotBlank() }.map { match.groupValues[1] to it.trim() } }.toList()
        assertTrue("Engine keywords were not parsed", rows.size > 100)
        for ((group, alias) in rows) {
            val (preceding, phrase) = when {
                group.startsWith("KW_TIPO_") -> "Definir dato " to "Como $alias"
                group == "KW_COMO" -> "Definir dato " to "Como"
                group == "KW_ES" -> "dato " to alias
                group in setOf("KW_POR_COPIA", "KW_POR_REFERENCIA") -> "SubProceso Prueba(dato " to alias
                group == "KW_SIN_SALTAR" -> "Escribir dato " to alias
                group in setOf("KW_DESDE", "KW_HASTA", "KW_CONPASO", "KW_HACER") -> "Para i <- 1 " to alias
                group == "KW_DE" -> "Para Cada dato " to alias
                group in setOf("KW_SEGUNDOS", "KW_MILISEGUNDOS") -> "Esperar 1 " to alias
                group == "KW_ENTONCES" -> "Si Verdadero " to alias
                else -> "" to alias
            }
            val prefix = phrase.dropLast(1)
            val candidates = completions(preceding + prefix)
            val expected = completionWords(all).filter { word -> word.spellings.any {
                normalized(it) == normalized(phrase) || (group in setOf("KW_COMO", "KW_ES") && normalized(it).startsWith(normalized(phrase) + " "))
            } }.map { normalized(it.label) }
            assertTrue("Missing engine alias: $group = $alias", expected.isNotEmpty())
            assertTrue("Unreachable engine alias: $preceding$prefix ($alias)", candidates.any { normalized(it.label) in expected })
        }
        report("engine-keywords", rows.map { "${it.first}\t${it.second}" })
    }

    @Test fun everyNativeFunctionHasCompletionAndArgumentHelp() {
        val names = Regex("m_predefs\\[\"([^\"]+)\"]").findAll(source("pseint/FuncsPredefs.cpp")).map { it.groupValues[1] }.toSet()
        assertTrue(names.size > 25)
        val present = spellings()
        for (name in names) {
            assertTrue("Missing native function: $name", normalized(name) in present)
            assertTrue("Unreachable function: $name", completions("Escribir " + name.dropLast(1)).any { normalized(it.label) == normalized(name) })
            val text = "Escribir $name("
            assertTrue("Missing arguments: $name", editorCallTip(TextFieldValue(text, TextRange(text.length)), all)!!.contains('('))
        }
        report("native-functions", names.sorted())
    }

    @Test fun everyNativeColloquialConditionIsRecognized() {
        val body = source("pseint/SynCheck.cpp").substringAfter("GetColoquialConditions() {").substringBefore("return v;")
        val phrases = Regex("coloquial_aux\\(\"([^\"]+)\"").findAll(body).map { it.groupValues[1].trim().replace("|", "O") }.toSet()
        assertTrue(phrases.size > 25)
        for (phrase in phrases) {
            val candidates = completions("Si dato " + phrase.dropLast(1))
            val canonical = completionWords(all).filter { word -> word.spellings.any { normalized(it) == normalized(phrase) } }.map { normalized(it.label) }
            assertTrue("Missing colloquial form: $phrase", canonical.isNotEmpty())
            assertTrue("Unreachable colloquial form: $phrase", candidates.any { normalized(it.label) in canonical })
        }
        report("colloquial-conditions", phrases.sorted())
    }

    @Test fun everyDesktopCallTipKeyHasMobileHelp() {
        val body = source("wxPSeInt/mxSource.cpp").substringAfter("void mxSource::SetCalltips() {").substringBefore("void mxSource::SetAutocompletion")
        val keys = Regex("calltips_(instructions|functions)\\.push_back\\(calltip_text\\(_Z\\(\"([^\"]+)\"").findAll(body).map { it.groupValues[1] to it.groupValues[2] }.toSet()
        assertTrue(keys.size > 40)
        for ((kind, key) in keys) {
            if (kind == "instructions") assertNotNull("Missing help: $key", completionInstructionHelp(key, all))
            else {
                val text = "Escribir $key("
                assertTrue("Missing function help: $key", editorCallTip(TextFieldValue(text, TextRange(text.length)), all)!!.contains('('))
            }
        }
        report("desktop-call-tips", keys.map { "${it.first}\t${it.second}" })
    }

    private fun report(name: String, rows: List<String>) {
        File("build/compatibility/completion").mkdirs()
        File("build/compatibility/completion/$name.tsv").writeText(rows.joinToString("\n", postfix = "\n"))
    }

    @Test fun bundledProfilesGateTheirOwnSuggestions() {
        val files = File("src/main/assets/profiles").listFiles()!!.filter { it.isFile }
        assertTrue(files.size > 400)
        for (file in files) {
            val profile = PSeIntProfile.loadFromPrf(file.name, decodePSeIntDocument(file.readBytes())).normalized()
            val cases = listOf(
                Triple("Al", "Algoritmo", profile.allowFunctions),
                Triple("Fu", "Funcion", profile.allowFunctions),
                Triple("Fin SubP", "Fin SubProceso", profile.allowFunctions && profile.flexibleSyntax),
                Triple("Escribir Long", "Longitud", profile.enableStringFunctions),
                Triple("Si n Es P", "Es Par", profile.colloquialConditions),
                Triple("n Es Re", "Es Real", profile.flexibleSyntax),
                Triple("Para Ca", "Para Cada", profile.enableParaCada),
                Triple("Mientras Qu", "Mientras Que", profile.allowRepetirMientrasQue),
                Triple("Redimen", "Redimensionar", profile.allowArrayResize),
                Triple("Inf", "Informar", profile.flexibleSyntax)
            )
            for ((text, label, enabled) in cases) assertEquals("${file.name}: $label", enabled, completions(text, profile).any { it.label == label })
        }
        report("profiles", files.map { it.name }.sorted())
    }
}
