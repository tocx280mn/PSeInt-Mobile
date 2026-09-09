package com.example

import androidx.compose.ui.text.TextRange
import androidx.compose.ui.text.input.TextFieldValue
import java.util.Locale

fun insertEditorText(value: TextFieldValue, text: String): TextFieldValue {
    val start = value.selection.min.coerceIn(0, value.text.length)
    val end = value.selection.max.coerceIn(start, value.text.length)
    return TextFieldValue(value.text.replaceRange(start, end, text), TextRange(start + text.length))
}

/** Returns one-based line and column for the active selection end. */
fun editorLineColumn(value: TextFieldValue): Pair<Int, Int> {
    val cursor = value.selection.end.coerceIn(0, value.text.length)
    val prefix = value.text.take(cursor)
    return prefix.count { it == '\n' } + 1 to cursor - prefix.lastIndexOf('\n')
}

private fun statementForIndentation(line: String): String {
    val comment = tokenizePSeInt(line).firstOrNull { it.kind == SyntaxTokenKind.Comment }?.start ?: line.length
    return line.take(comment).trim().trimEnd(';').lowercase(Locale.ROOT).replace(Regex("\\s+"), " ")
}

private val blockStart = Regex("^(algoritmo|proceso|funcion|función|subproceso|subalgoritmo)\\b|^si\\b.*\\bentonces$|^(mientras|para|segun|según)\\b.*\\bhacer$|^repetir$")
private val blockEnd = Regex("^fin\\s*(algoritmo|proceso|funcion|función|subproceso|subalgoritmo|si|segun|según|mientras|para)\\b|^(hasta|mientras)\\s+que\\b")

private fun isCaseLabel(line: String): Boolean {
    val tokens = tokenizePSeInt(line)
    val colon = line.indices.firstOrNull { index ->
        line[index] == ':' && tokens.none { (it.kind == SyntaxTokenKind.String || it.kind == SyntaxTokenKind.Comment) && index in it.start until it.end }
    } ?: return false
    if (colon == 0) return false
    val allowedKeywords = setOf("caso", "opcion", "opción", "de", "otro", "modo")
    return tokens.none {
        it.start < colon && (it.kind == SyntaxTokenKind.Function || (it.kind == SyntaxTokenKind.Keyword && line.substring(it.start, it.end).lowercase(Locale.ROOT) !in allowedKeywords))
    } && "<-" !in line.take(colon) && "=" !in line.take(colon)
}

internal fun autoIndentEditorChange(previous: TextFieldValue, next: TextFieldValue): TextFieldValue {
    // Only intercept a single typed newline. Pasting and IME composition keep their exact content.
    if (!previous.selection.collapsed || !next.selection.collapsed || next.composition != null || next.text.length != previous.text.length + 1) return next
    val cursor = next.selection.end
    if (cursor <= 0 || next.text[cursor - 1] != '\n' || next.text.removeRange(cursor - 1, cursor) != previous.text) return next
    val previousLine = next.text.substring(0, cursor - 1).substringAfterLast('\n')
    val indentation = previousLine.takeWhile { it == ' ' || it == '\t' }
    val statement = statementForIndentation(previousLine)
    val extra = if (blockStart.containsMatchIn(statement) || statement == "sino" || statement.endsWith(':')) "    " else ""
    val inserted = indentation + extra
    return if (inserted.isEmpty()) next else insertEditorText(next, inserted)
}

fun indentEditorSelection(value: TextFieldValue, unindent: Boolean = false): TextFieldValue {
    if (value.selection.collapsed && !unindent) return insertEditorText(value, "    ")
    val start = value.text.lastIndexOf('\n', (value.selection.min - 1).coerceAtLeast(-1)) + 1
    val selectedEnd = value.selection.max
    val end = if (selectedEnd > start && value.text.getOrNull(selectedEnd - 1) == '\n') selectedEnd - 1 else selectedEnd
    val lineEnd = value.text.indexOf('\n', end).let { if (it < 0) value.text.length else it }
    val original = value.text.substring(start, lineEnd)
    val changed = original.split('\n').joinToString("\n") { line ->
        if (!unindent) "    $line"
        else if (line.startsWith('\t')) line.drop(1)
        else line.drop(line.takeWhile { it == ' ' }.length.coerceAtMost(4))
    }
    val text = value.text.replaceRange(start, lineEnd, changed)
    if (value.selection.collapsed) {
        val removed = original.length - changed.length
        return TextFieldValue(text, TextRange((value.selection.end - removed).coerceAtLeast(start)))
    }
    val range = if (value.selection.reversed) TextRange(start + changed.length, start) else TextRange(start, start + changed.length)
    return TextFieldValue(text, range)
}

/** Changes indentation only; strings, comments, and instruction contents are preserved. */
fun formatPSeIntCode(source: String): String {
    data class Block(val isSwitch: Boolean, var hasCase: Boolean = false)
    val blocks = mutableListOf<Block>()
    return source.replace("\r\n", "\n").replace('\r', '\n').split('\n').joinToString("\n") { raw ->
        val line = raw.trim()
        if (line.isEmpty()) return@joinToString ""
        val statement = statementForIndentation(line)
        if (blockEnd.containsMatchIn(statement) && blocks.isNotEmpty()) blocks.removeAt(blocks.lastIndex)
        val caseLine = blocks.lastOrNull()?.isSwitch == true && isCaseLabel(line)
        if (caseLine) blocks.last().hasCase = false
        var indentation = blocks.sumOf { 1 + if (it.hasCase) 1 else 0 }
        if (statement == "sino" || statement == "si no") indentation = (indentation - 1).coerceAtLeast(0)
        val formatted = "    ".repeat(indentation) + line
        if (caseLine) blocks.last().hasCase = true
        if (blockStart.containsMatchIn(statement)) blocks.add(Block(statement.startsWith("segun ") || statement.startsWith("según ")))
        formatted
    }
}
