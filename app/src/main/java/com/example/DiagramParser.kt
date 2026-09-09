package com.example

import java.text.Normalizer
import java.util.Locale

/** A tolerant structural parser: incomplete code can still be explored in the diagram editor. */
object DiagramParser {
    fun parse(lines: List<String>): List<DiagramAstNode> = Parser(lines).parseBlock()

    private data class SourceLine(val text: String, val index: Int) {
        val normalized: String = Normalizer.normalize(text, Normalizer.Form.NFD)
            .replace(Regex("\\p{M}+"), "").lowercase(Locale.ROOT)

        fun starts(keyword: String): Boolean =
            Regex("^$keyword\\b").containsMatchIn(normalized)

        val closing: Boolean get() = starts("fin\\s*(si|mientras|para|segun|algoritmo|proceso|subproceso|funcion)") ||
            starts("hasta\\s+que") || starts("mientras\\s+que") || starts("si\\s*no") || starts("de\\s*otro\\s*modo")
    }

    private class Parser(lines: List<String>) {
        private val source = lines.flatMapIndexed { index, line ->
            statements(line).map { SourceLine(it, index) }
        }.toMutableList()
        private var cursor = 0

        fun parseBlock(stops: List<String> = emptyList(), stopAtCase: Boolean = false): List<DiagramAstNode> {
            val nodes = mutableListOf<DiagramAstNode>()
            while (cursor < source.size) {
                val line = source[cursor]
                if (stops.any(line::starts) || (stopAtCase && caseColon(line.text) >= 0)) break
                // Leave an outer closing delimiter to its owner when a child block is unfinished.
                if (stops.isNotEmpty() && line.closing) break
                cursor++
                when {
                    line.starts("(algoritmo|proceso|subproceso|funcion)") ->
                        nodes += DiagramAstNode.Terminator(line.text, line.index, true)
                    line.starts("fin\\s*(algoritmo|proceso|subproceso|funcion)") ->
                        nodes += DiagramAstNode.Terminator(line.text, line.index, false)
                    line.starts("si") -> {
                        val yes = parseBlock(listOf("si\\s*no", "fin\\s*si"))
                        val no = if (consume("si\\s*no")) parseBlock(listOf("fin\\s*si")) else emptyList()
                        consume("fin\\s*si")
                        nodes += DiagramAstNode.If(line.text, line.index, yes, no)
                    }
                    line.starts("mientras") -> {
                        val body = parseBlock(listOf("fin\\s*mientras"))
                        consume("fin\\s*mientras")
                        nodes += DiagramAstNode.While(line.text, line.index, body)
                    }
                    line.starts("para") -> {
                        val body = parseBlock(listOf("fin\\s*para"))
                        consume("fin\\s*para")
                        nodes += DiagramAstNode.For(line.text, line.index, body)
                    }
                    line.starts("repetir") -> {
                        val body = parseBlock(listOf("hasta\\s+que", "mientras\\s+que"))
                        val condition = source.getOrNull(cursor)?.takeIf { it.starts("hasta\\s+que") || it.starts("mientras\\s+que") }
                        if (condition != null) cursor++
                        nodes += DiagramAstNode.DoWhile(
                            condition?.text ?: "Hasta Que condicion",
                            condition?.index ?: line.index,
                            body,
                            startLineIndex = line.index,
                            continueWhileTrue = condition?.starts("mientras\\s+que") == true
                        )
                    }
                    line.starts("segun") -> nodes += parseSwitch(line)
                    line.starts("leer") -> nodes += DiagramAstNode.Input(line.text, line.index)
                    line.starts("(escribir|mostrar|imprimir)") -> nodes += DiagramAstNode.Output(line.text, line.index)
                    !line.closing -> nodes += DiagramAstNode.Process(line.text, line.index)
                }
            }
            return nodes
        }

        private fun consume(keyword: String): Boolean {
            if (source.getOrNull(cursor)?.starts(keyword) != true) return false
            cursor++
            return true
        }

        private fun parseSwitch(header: SourceLine): DiagramAstNode.Switch {
            val cases = mutableListOf<Pair<String, List<DiagramAstNode>>>()
            val indices = mutableListOf<Int>()
            var default: List<DiagramAstNode>? = null
            var defaultIndex = header.index
            while (cursor < source.size) {
                val line = source[cursor]
                if (consume("fin\\s*segun")) break
                val isDefault = line.starts("de\\s*otro\\s*modo")
                val colon = if (isDefault) unquotedColon(line.text) else caseColon(line.text)
                if (line.closing && !isDefault) break
                cursor++
                if (colon < 0 && !isDefault) continue
                // A case can put its first instruction after the colon on the same source line.
                if (colon >= 0) {
                    val inline = line.text.substring(colon + 1).trim()
                    if (inline.isNotEmpty()) source.add(cursor, SourceLine(inline, line.index))
                }
                val body = parseBlock(listOf("fin\\s*segun", "de\\s*otro\\s*modo"), stopAtCase = true)
                if (isDefault) {
                    default = body
                    defaultIndex = line.index
                } else {
                    cases += line.text.substring(0, colon).trim() to body
                    indices += line.index
                }
            }
            return DiagramAstNode.Switch(header.text, header.index, cases, default, indices, defaultIndex)
        }
    }

    /** Comments and semicolons have no structural meaning inside quoted strings. */
    private fun statements(line: String): List<String> {
        val result = mutableListOf<String>()
        var quote: Char? = null
        var start = 0
        var index = 0
        while (index < line.length) {
            val char = line[index]
            if (quote != null) {
                when {
                    char == '\\' && index + 1 < line.length -> index++
                    char == quote && line.getOrNull(index + 1) == quote -> index++
                    char == quote -> quote = null
                }
            } else when {
                char == '"' || char == '\'' -> quote = char
                char == '/' && line.getOrNull(index + 1) == '/' -> {
                    line.substring(start, index).trim().takeIf(String::isNotEmpty)?.let(result::add)
                    return result
                }
                char == ';' -> {
                    line.substring(start, index).trim().takeIf(String::isNotEmpty)?.let(result::add)
                    start = index + 1
                }
            }
            index++
        }
        line.substring(start).trim().takeIf(String::isNotEmpty)?.let(result::add)
        return result
    }

    private fun unquotedColon(text: String): Int {
        var quote: Char? = null
        var index = 0
        while (index < text.length) {
            val char = text[index]
            if (quote != null) {
                when {
                    char == '\\' && index + 1 < text.length -> index++
                    char == quote && text.getOrNull(index + 1) == quote -> index++
                    char == quote -> quote = null
                }
            } else when {
                char == '"' || char == '\'' -> quote = char
                char == ':' && text.getOrNull(index + 1) != '=' -> return index
            }
            index++
        }
        return -1
    }

    private fun caseColon(text: String): Int {
        val colon = unquotedColon(text)
        if (colon <= 0) return -1
        val label = text.substring(0, colon).trim()
        val value = """(?:[+-]?\d+(?:\.\d+)?|[\p{L}_][\p{L}\p{N}_]*|"(?:[^"\\]|\\.|"")*"|'(?:[^'\\]|\\.|'')*')"""
        return if (Regex("$value(?:\\s*,\\s*$value)*").matches(label)) colon else -1
    }
}
