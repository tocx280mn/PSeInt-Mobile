package com.example

import androidx.compose.ui.text.AnnotatedString
import androidx.compose.ui.text.SpanStyle
import androidx.compose.ui.text.buildAnnotatedString
import androidx.compose.ui.text.font.FontStyle
import androidx.compose.ui.text.font.FontWeight
import androidx.compose.ui.text.input.OffsetMapping
import androidx.compose.ui.text.input.TransformedText
import androidx.compose.ui.text.input.VisualTransformation
import com.example.ui.theme.EditorSyntaxColors
import java.util.Locale

/** A single token pass prevents // inside a quoted string from becoming a comment. */
class PSeIntSyntaxHighlighter(private val darkTheme: Boolean = false, private val profile: PSeIntProfile = PSeIntProfile.Flexible) : VisualTransformation {
    override fun filter(text: AnnotatedString): TransformedText {
        val colors = EditorSyntaxColors.forTheme(darkTheme)
        val highlighted = buildAnnotatedString {
            append(text)
            for (token in tokenizePSeInt(text.text, profile)) {
                val style = when (token.kind) {
                    SyntaxTokenKind.Keyword -> SpanStyle(colors.keyword, fontWeight = FontWeight.Bold)
                    SyntaxTokenKind.Function -> SpanStyle(colors.keyword)
                    SyntaxTokenKind.String -> SpanStyle(colors.string)
                    SyntaxTokenKind.Number -> SpanStyle(colors.number)
                    SyntaxTokenKind.Comment -> SpanStyle(colors.comment, fontStyle = FontStyle.Italic)
                    SyntaxTokenKind.Operator -> SpanStyle(colors.keyword)
                }
                addStyle(style, token.start, token.end)
            }
        }
        return TransformedText(highlighted, OffsetMapping.Identity)
    }
}

internal enum class SyntaxTokenKind { Keyword, Function, String, Number, Comment, Operator }
internal data class SyntaxToken(val start: Int, val end: Int, val kind: SyntaxTokenKind)

private val keywords = setOf(
    "algoritmo", "finalgoritmo", "proceso", "finproceso", "escribir", "leer", "definir",
    "como", "entero", "real", "logico", "lógico", "caracter", "carácter", "si", "sí",
    "entonces", "sino", "finsi", "segun", "según", "hacer", "finsegun", "finsegún",
    "mientras", "finmientras", "repetir", "para", "finpara", "funcion", "función",
    "finfuncion", "finfunción", "y", "o", "no", "mod", "subproceso", "finsubproceso",
    "subalgoritmo", "finsubalgoritmo", "dimension", "dimensión", "redimensionar", "desde",
    "opcion", "opción", "caso", "limpiarpantalla", "borrarpantalla", "cadena", "texto",
    "informar", "imprimir", "mostrar", "entera", "enteros", "reales", "logica", "lógica",
    "numero", "número", "numerico", "numérico", "numerica", "numérica", "caracteres",
    "hasta", "que", "con", "paso", "de", "otro", "modo", "sin", "saltar", "bajar",
    "limpiar", "borrar", "pantalla", "por", "valor", "referencia", "fin", "cada",
    "esperar", "tecla", "segundos", "milisegundos"
)
private val builtInFunctions = completionFunctions.flatMap { listOf(it.name) + it.aliases }.map { it.lowercase(Locale.ROOT) }.toSet()
private val constants = setOf("verdadero", "falso", "pi", "euler")
private val numberToken = Regex("(?:\\d+(?:\\.\\d*)?|\\.\\d+)(?:[eE][+-]?\\d+)?")

private val functionKeywords = setOf("funcion", "función", "finfuncion", "finfunción", "subproceso", "finsubproceso", "subalgoritmo", "finsubalgoritmo", "por", "valor", "referencia", "copia")
private val wordOperators = setOf("y", "o", "no", "mod")
private val lazyKeywords = setOf("desde", "opcion", "opción", "caso", "imprimir", "mostrar", "informar")
private val colloquialKeywords = setOf("es", "son", "par", "impar", "igual", "divisible", "multiplo", "múltiplo", "distinto", "distinta", "cero", "positivo", "negativo", "positiva", "negativa", "mayor", "menor")
private val stringFunctions = setOf("longitud", "subcadena", "mayusculas", "mayúsculas", "minusculas", "minúsculas", "concatenar", "convertiranumero", "convertiranúmero", "convertiratexto")

internal fun tokenizePSeInt(source: String, profile: PSeIntProfile = PSeIntProfile.Flexible): List<SyntaxToken> = buildList {
    val activeKeywords = keywords.toMutableSet().apply {
        if (!profile.allowFunctions) removeAll(functionKeywords)
        if (!profile.allowWordOperators && !profile.colloquialConditions) removeAll(wordOperators)
        if (!profile.flexibleSyntax) removeAll(lazyKeywords)
        if (!profile.enableParaCada) remove("cada")
        if (profile.allowArrayResize) addAll(setOf("redimension", "redimensión", "redimensionar")) else remove("redimensionar")
        if (profile.colloquialConditions) addAll(colloquialKeywords)
    }
    var index = 0
    while (index < source.length) {
        val start = index
        val char = source[index]
        when {
            source.startsWith("//", index) -> {
                index = source.indexOf('\n', index).let { if (it < 0) source.length else it }
                add(SyntaxToken(start, index, SyntaxTokenKind.Comment))
            }
            char == '"' || char == '\'' -> {
                val quote = char
                index++
                while (index < source.length && source[index] != '\n' && source[index] != '\r') {
                    if (source[index] == '\\' && index + 1 < source.length && source[index + 1] != '\n') {
                        index += 2
                    } else if (source[index++] == quote) {
                        break
                    }
                }
                add(SyntaxToken(start, index, SyntaxTokenKind.String))
            }
            char.isLetter() || char == '_' -> {
                index++
                while (index < source.length && (source[index].isLetterOrDigit() || source[index] == '_')) index++
                val word = source.substring(start, index).lowercase(Locale.ROOT)
                val kind = when {
                    word in activeKeywords -> SyntaxTokenKind.Keyword
                    word in constants -> SyntaxTokenKind.Number
                    word in builtInFunctions && (profile.enableStringFunctions || word !in stringFunctions) -> {
                        var next = index
                        while (next < source.length && source[next].isWhitespace()) next++
                        if (next < source.length && source[next] == '(') SyntaxTokenKind.Function else null
                    }
                    else -> null
                }
                if (kind != null) add(SyntaxToken(start, index, kind))
            }
            char.isDigit() || (char == '.' && source.getOrNull(index + 1)?.isDigit() == true) -> {
                val match = numberToken.matchAt(source, index)
                if (match != null) {
                    index = match.range.last + 1
                    add(SyntaxToken(start, index, SyntaxTokenKind.Number))
                } else index++
            }
            char in "+-*/^%=<>~&|¬" -> {
                index++
                add(SyntaxToken(start, index, SyntaxTokenKind.Operator))
            }
            else -> index++
        }
    }
}
