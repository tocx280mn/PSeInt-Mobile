package com.example

import android.os.Bundle
import androidx.activity.ComponentActivity
import androidx.activity.compose.setContent
import androidx.activity.enableEdgeToEdge
import androidx.compose.runtime.SideEffect
import androidx.core.view.WindowCompat
import androidx.lifecycle.viewmodel.compose.viewModel

class MainActivity : ComponentActivity() {
    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        enableEdgeToEdge()
        setContent {
            val workspace: WorkspaceViewModel = viewModel()
            val light = workspace.theme == AppTheme.LightClassic
            SideEffect {
                WindowCompat.getInsetsController(window, window.decorView).apply {
                    isAppearanceLightStatusBars = light
                    isAppearanceLightNavigationBars = light
                }
            }
            PSeIntApp(workspace)
        }
    }
}

enum class AppTab { Editor, Diagram, Terminal, Settings }
enum class AppTheme { Dark, LightClassic, HackerMatrix, OceanBlue }
enum class DiagramType { Classic, NassiShneiderman }
