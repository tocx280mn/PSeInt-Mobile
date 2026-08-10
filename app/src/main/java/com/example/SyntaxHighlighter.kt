package com.example

import androidx.compose.ui.graphics.Color
import androidx.compose.ui.text.AnnotatedString
import androidx.compose.ui.text.SpanStyle
import androidx.compose.ui.text.buildAnnotatedString
import androidx.compose.ui.text.input.OffsetMapping
import androidx.compose.ui.text.input.TransformedText
import androidx.compose.ui.text.input.VisualTransformation
import androidx.compose.ui.text.withStyle
import com.example.ui.theme.*

class PSeIntSyntaxHighlighter : VisualTransformation {

    private val keywords = listOf(
        "Algoritmo", "FinAlgoritmo", "Proceso", "FinProceso",
        "Escribir", "Leer", "Definir", "Como", "Entero", "Real", "Logico", "Caracter",
        "Si", "Entonces", "Sino", "FinSi", "Segun", "Hacer", "De Otro Modo", "FinSegun",
        "Mientras", "FinMientras", "Repetir", "Hasta Que", "Para", "FinPara", "Con Paso",
        "Funcion", "FinFuncion", "Verdadero", "Falso", "Y", "O", "NO", "MOD"
    )

    private val keywordRegex = Regex("\\b(${keywords.joinToString("|")})\\b", RegexOption.IGNORE_CASE)
    private val stringRegex = Regex("\".*?\"")
    private val numberRegex = Regex("\\b\\d+(\\.\\d+)?\\b")
    private val commentRegex = Regex("//.*")

    override fun filter(text: AnnotatedString): TransformedText {
        val annotatedString = buildAnnotatedString {
            append(text.text)
            
            // Basic syntax highlighting
            // Find comments
            val comments = commentRegex.findAll(text.text)
            for (match in comments) {
                addStyle(SpanStyle(color = CodeComment), match.range.first, match.range.last + 1)
            }
            
            // Find strings
            val strings = stringRegex.findAll(text.text)
            for (match in strings) {
                // Don't overlap with comments
                if (!isOverlap(match.range, comments)) {
                    addStyle(SpanStyle(color = CodeString), match.range.first, match.range.last + 1)
                }
            }
            
            // Find keywords
            val matches = keywordRegex.findAll(text.text)
            for (match in matches) {
                if (!isOverlap(match.range, strings) && !isOverlap(match.range, comments)) {
                    addStyle(SpanStyle(color = CodeKeyword), match.range.first, match.range.last + 1)
                }
            }
            
            // Find numbers
            val numbers = numberRegex.findAll(text.text)
            for (match in numbers) {
                if (!isOverlap(match.range, strings) && !isOverlap(match.range, comments) && !isOverlap(match.range, matches)) {
                    addStyle(SpanStyle(color = CodeNumber), match.range.first, match.range.last + 1)
                }
            }
        }
        return TransformedText(annotatedString, OffsetMapping.Identity)
    }

    private fun isOverlap(range: IntRange, others: Sequence<MatchResult>): Boolean {
        for (other in others) {
            if (range.first <= other.range.last && other.range.first <= range.last) {
                return true
            }
        }
        return false
    }
}
