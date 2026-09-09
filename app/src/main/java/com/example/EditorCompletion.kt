package com.example

import androidx.compose.ui.text.TextRange
import androidx.compose.ui.text.input.TextFieldValue
import java.util.Locale

internal data class EditorCompletion(val label: String, val insertion: String, val start: Int, val end: Int)
private fun String.foldCode() = lowercase(Locale.ROOT)
private fun Char.isCodeWord() = isLetterOrDigit() || this == '_'

/** Preserves offsets and line breaks while hiding comments and quoted text. */
private fun codeContext(source: String, cursor: Int): Pair<String, Boolean> {
    val result = source.toCharArray()
    var quote: Char? = null
    var comment = false
    var escaped = false
    var inLiteralAtCursor = false
    for (i in source.indices) {
        if (i == cursor) inLiteralAtCursor = quote != null || comment
        val c = source[i]
        if (c == '\n' || c == '\r') { quote = null; comment = false; escaped = false; continue }
        if (comment) { result[i] = ' '; continue }
        if (quote != null) {
            result[i] = ' '
            if (escaped) escaped = false else if (c == '\\') escaped = true else if (c == quote) quote = null
        } else if (c == '"' || c == '\'') {
            quote = c; result[i] = ' '
        } else if (c == '/' && source.getOrNull(i + 1) == '/') {
            comment = true; result[i] = ' '
        }
    }
    if (cursor == source.length) inLiteralAtCursor = quote != null || comment
    return String(result) to inLiteralAtCursor
}

internal fun editorCompletions(value: TextFieldValue, profile: PSeIntProfile): List<EditorCompletion> {
    if (!value.selection.collapsed) return emptyList()
    val cursor = value.selection.end.coerceIn(0, value.text.length)
    val (code, excluded) = codeContext(value.text, cursor)
    if (excluded) return emptyList()
    val statementStart = maxOf(code.lastIndexOf('\n', cursor - 1), code.lastIndexOf(';', cursor - 1)) + 1
    val statement = code.substring(statementStart, cursor)
    val first = Regex("[\\p{L}_][\\p{L}\\p{N}_]*").find(statement)?.value?.foldCompletion().orEmpty()
    val words = completionWords(profile).toMutableList()
    // Names already present in code are useful for variable and user-function completion.
    Regex("[\\p{L}_][\\p{L}\\p{N}_]*").findAll(code).filter {
        it.value.foldCompletion() !in completionReservedNames && cursor !in it.range.first..(it.range.last + 1) &&
            (profile.allowAccentsInVariables || it.value.all { c -> c.code < 128 })
    }.map { it.value }.distinctBy { it.foldCode() }.forEach { words.add(CompletionWord(it, it, expression = true, identifier = true)) }
    val end = value.text.indexOfFirstFrom(cursor) { !it.isCodeWord() }
    val starts = Regex("[\\p{L}_][\\p{L}\\p{N}_]*").findAll(statement).map { it.range.first }.toList()
    return words.mapNotNull { word ->
        starts.firstNotNullOfOrNull { start ->
            val prefix = statement.substring(start).replace(Regex("\\s+"), " ")
            val before = statement.take(start).trim()
            val declaration = Regex("^[\\p{L}_][\\p{L}\\p{N}_]*(\\s*,\\s*[\\p{L}_][\\p{L}\\p{N}_]*)*$").matches(before) && first !in completionReservedNames
            val inContext = when {
                "@declaration" in word.contexts -> declaration
                "@condition" in word.contexts -> before.isNotEmpty() && first !in setOf("definir", "algoritmo", "proceso", "funcion", "subproceso", "subalgoritmo")
                "@foreach" in word.contexts -> Regex("^para\\s*cada\\b", RegexOption.IGNORE_CASE).containsMatchIn(before)
                else -> first in word.contexts
            }
            val allowed = if (word.contexts.isNotEmpty()) inContext
                else if (word.expression) first !in setOf("definir", "algoritmo", "proceso", "funcion", "subproceso", "subalgoritmo")
                else before.isEmpty()
            val matches = word.spellings.any { spelling ->
                val typed = if (word.identifier) prefix.foldCode() else prefix.foldCompletion()
                val match = if (word.identifier) spelling.foldCode() else spelling.foldCompletion()
                val minPrefix = if (match.length == 2) 1 else 2
                typed.length >= minPrefix && match.startsWith(typed) && match != typed
            }
            if (allowed && matches)
                EditorCompletion(word.label, word.insertion, statementStart + start, end)
            else null
        }
    }.distinctBy { it.label.foldCode() }.sortedBy { it.label.foldCode() }
}

private inline fun String.indexOfFirstFrom(start: Int, predicate: (Char) -> Boolean): Int {
    for (i in start until length) if (predicate(this[i])) return i
    return length
}

internal fun applyEditorCompletion(value: TextFieldValue, completion: EditorCompletion): TextFieldValue {
    var insertion = completion.insertion
    val next = value.text.getOrNull(completion.end)
    var existingParentheses = 0
    if (insertion.endsWith("()") && next == '(') {
        insertion = insertion.dropLast(2)
        existingParentheses = if (value.text.getOrNull(completion.end + 1) == ')') 2 else 1
    }
    if ((insertion.endsWith(' ') && (next?.isWhitespace() == true || next in listOf(';', ',', ')', ']'))) ||
        (insertion.lastOrNull() in listOf(';', '(') && next == insertion.lastOrNull())) insertion = insertion.dropLast(1)
    val text = value.text.replaceRange(completion.start, completion.end, insertion)
    val cursor = completion.start + insertion.length + existingParentheses + if (next == '(' && completion.insertion.endsWith('(')) 1 else 0
    return TextFieldValue(text, TextRange(cursor))
}

internal fun editorCallTip(value: TextFieldValue, profile: PSeIntProfile): String? {
    if (!value.selection.collapsed) return null
    val cursor = value.selection.end.coerceIn(0, value.text.length)
    val (code, excluded) = codeContext(value.text, cursor)
    if (excluded) return null
    val start = maxOf(code.lastIndexOf('\n', cursor - 1), code.lastIndexOf(';', cursor - 1)) + 1
    val statement = code.substring(start, cursor)
    val parentheses = mutableListOf<Int>()
    statement.forEachIndexed { index, c -> if (c == '(') parentheses.add(index) else if (c == ')' && parentheses.isNotEmpty()) parentheses.removeAt(parentheses.lastIndex) }
    val function = parentheses.lastOrNull()?.let { Regex("[\\p{L}_][\\p{L}\\p{N}_]*\\s*$").find(statement.take(it))?.value?.trim()?.foldCompletion() }
        ?: Regex("([\\p{L}_][\\p{L}\\p{N}_]*)\\(\\)\\s*$").find(statement)?.groupValues?.get(1)?.foldCompletion()
    if (function != null) {
        val spec = completionFunctions.firstOrNull { (listOf(it.name) + it.aliases).any { name -> name.foldCompletion() == function } }
        if (spec != null && (!spec.stringFunction || profile.enableStringFunctions)) return "${spec.name.foldCode()}(${spec.arguments})"
    }
    val command = Regex("^\\s*([\\p{L}]+)\\s+").find(statement)?.groupValues?.get(1)?.foldCompletion() ?: return null
    // A newly typed contextual keyword takes precedence over the instruction's general help.
    val last = Regex("([\\p{L}]+)\\s+$").find(statement)?.groupValues?.get(1)?.foldCompletion()
    val contextual = when {
        last == "que" && command in setOf("hasta", "mientras") -> "condición, expresión lógica"
        last == "hasta" && command == "para" -> "valor final"
        last == "desde" && command == "para" && profile.flexibleSyntax -> "valor inicial"
        last == "paso" && command == "para" -> "valor del paso"
        last == "entonces" && command == "si" -> "acciones por verdadero"
        else -> null
    }
    val help = contextual ?: completionInstructionHelp(command, profile)
    return help?.let { "{ $it }" }
}
