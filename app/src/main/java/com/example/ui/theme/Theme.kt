package com.example.ui.theme

import android.os.Build
import androidx.compose.foundation.isSystemInDarkTheme
import androidx.compose.material3.MaterialTheme
import androidx.compose.material3.darkColorScheme
import androidx.compose.runtime.Composable
import androidx.compose.ui.graphics.Color

private val IdeColorScheme = darkColorScheme(
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

@Composable
fun MyApplicationTheme(
    darkTheme: Boolean = true, // Use dark theme
    dynamicColor: Boolean = false,
    content: @Composable () -> Unit
) {
    MaterialTheme(
        colorScheme = IdeColorScheme,
        typography = Typography,
        content = content
    )
}
