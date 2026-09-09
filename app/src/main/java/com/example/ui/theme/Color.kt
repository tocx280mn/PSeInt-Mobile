package com.example.ui.theme

import androidx.compose.ui.graphics.Color

val IdeBackground = Color(0xFF0F1115)
val IdeSurface = Color(0xFF161B22)
val IdeSurfaceVariant = Color(0xFF21262D)
val IdePrimary = Color(0xFF58A6FF)
val IdeSecondary = Color(0xFF1F6FEB)
val IdeText = Color(0xFFE6EDF3)
val IdeTextMuted = Color(0xFF8B949E)
val IdeBorder = Color(0xFF30363D)

// Syntax Highlighting Colors
// Classic colors from the desktop editor (wxPSeInt/mxSource.cpp).
val CodeKeyword = Color(0xFF000080)
val CodeFunction = CodeKeyword
val CodeString = Color(0xFF006400)
val CodeNumber = Color(0xFFA0522D)
val CodeComment = Color(0xFF727272)
val CodeVariable = Color(0xFF111111)
val CodeOperator = CodeKeyword
val LineNumberColor = Color(0xFF6E7681)
val CurrentLineHighlight = Color(0xFF21262D)

data class EditorSyntaxColors(
    val keyword: Color,
    val string: Color,
    val number: Color,
    val comment: Color
) {
    companion object {
        fun forTheme(darkTheme: Boolean) = if (darkTheme) {
            EditorSyntaxColors(Color(0xFF9999FA), Color(0xFF99FA99), Color(0xFFFAFA99), Color(0xFFAAAAAA))
        } else {
            EditorSyntaxColors(CodeKeyword, CodeString, CodeNumber, CodeComment)
        }
    }
}
