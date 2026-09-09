package com.example.ui.theme

import androidx.compose.material3.MaterialTheme
import androidx.compose.material3.darkColorScheme
import androidx.compose.material3.lightColorScheme
import androidx.compose.runtime.Composable
import androidx.compose.ui.graphics.Color

private val DarkIdeColorScheme = darkColorScheme(
    primary = IdePrimary,
    secondary = IdeSecondary,
    background = IdeBackground,
    surface = IdeSurface,
    surfaceVariant = IdeSurfaceVariant,
    onPrimary = Color.White,
    onSecondary = IdeText,
    onBackground = IdeText,
    onSurface = IdeText,
    onSurfaceVariant = IdeTextMuted,
    outline = IdeBorder
)

private val ClassicIdeColorScheme = lightColorScheme(
    primary = Color(0xFF175A9E),
    secondary = Color(0xFF28773C),
    background = Color(0xFFF0F1F3),
    surface = Color(0xFFFAFAFA),
    surfaceVariant = Color(0xFFE9EBEF),
    onPrimary = Color.White,
    onSecondary = Color.White,
    onBackground = Color(0xFF22252B),
    onSurface = Color(0xFF22252B),
    onSurfaceVariant = Color(0xFF626973),
    outline = Color(0xFFB5BBC4),
    primaryContainer = Color(0xFFDBEBFC),
    onPrimaryContainer = Color(0xFF113B63),
    error = Color(0xFFB3261E)
)

@Composable
fun MyApplicationTheme(
    darkTheme: Boolean = false,
    dynamicColor: Boolean = false,
    content: @Composable () -> Unit
) {
    MaterialTheme(
        colorScheme = if (darkTheme) DarkIdeColorScheme else ClassicIdeColorScheme,
        typography = Typography,
        content = content
    )
}
