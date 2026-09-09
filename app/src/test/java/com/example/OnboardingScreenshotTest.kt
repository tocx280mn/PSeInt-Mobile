package com.example

import android.app.Application
import androidx.compose.runtime.mutableStateOf
import androidx.compose.ui.test.*
import androidx.compose.ui.test.junit4.createComposeRule
import androidx.test.core.app.ApplicationProvider
import com.github.takahirom.roborazzi.captureRoboImage
import org.junit.Assert.*
import org.junit.Before
import org.junit.Rule
import org.junit.Test
import org.junit.runner.RunWith
import org.robolectric.RobolectricTestRunner
import org.robolectric.annotation.Config
import org.robolectric.annotation.GraphicsMode
import java.io.File

@RunWith(RobolectricTestRunner::class)
@GraphicsMode(GraphicsMode.Mode.NATIVE)
@Config(qualifiers = "w393dp-h852dp-mdpi", sdk = [36])
class OnboardingScreenshotTest {
    @get:Rule val rule = createComposeRule()
    private lateinit var app: Application

    @Before fun freshInstall() {
        app = ApplicationProvider.getApplicationContext()
        app.getSharedPreferences("pseint_prefs", 0).edit().clear().commit()
        File("build/outputs/screenshots").mkdirs()
    }

    @Test fun firstLaunchShowsSetupAndCompletionPersistsChoices() {
        val current = mutableStateOf(WorkspaceViewModel(app))
        val source = current.value.editor.text
        rule.setContent { PSeIntApp(current.value) }
        rule.onNodeWithText("Configuración inicial").assertIsDisplayed()
        rule.onNodeWithTag("code_editor").assertDoesNotExist()
        rule.onRoot().captureRoboImage("build/outputs/screenshots/configuracion-inicial.png")
        rule.runOnIdle {
            val icon = app.getDrawable(R.mipmap.ic_launcher)!!
            val bitmap = android.graphics.Bitmap.createBitmap(192, 192, android.graphics.Bitmap.Config.ARGB_8888)
            icon.setBounds(0, 0, 192, 192)
            icon.draw(android.graphics.Canvas(bitmap))
            File("build/outputs/screenshots/icono-lanzador.png").outputStream().use {
                bitmap.compress(android.graphics.Bitmap.CompressFormat.PNG, 100, it)
            }
        }

        rule.onNodeWithText("Seleccionar Perfil: Flexible").performClick()
        rule.onNode(hasSetTextAction()).performTextInput("Estricto")
        rule.onNode(hasText("Estricto") and !hasSetTextAction()).performClick()
        rule.onNodeWithText("PSeInt Oscuro (Noche)").performScrollTo().performClick()
        rule.onNodeWithText("Comenzar a programar").performScrollTo().performClick()
        rule.onNodeWithTag("code_editor").assertExists()
        rule.runOnIdle {
            assertFalse(current.value.prefs.getBoolean("first_run", true))
            current.value = WorkspaceViewModel(app)
            assertFalse(current.value.showOnboarding)
            assertEquals(PSeIntProfile.Estricto.nativeFlags(), current.value.profile.nativeFlags())
            assertEquals(AppTheme.Dark, current.value.theme)
            assertEquals(source, current.value.editor.text)
        }
        rule.onNodeWithText("Configuración inicial").assertDoesNotExist()
        rule.onNodeWithTag("code_editor").assertExists()
    }

    @Test
    @Config(qualifiers = "w640dp-h360dp-mdpi")
    fun interruptedSetupReturnsWithChoicesAndCanFinishInLandscape() {
        val current = mutableStateOf(WorkspaceViewModel(app))
        rule.setContent { PSeIntApp(current.value) }
        rule.onNodeWithText("PSeInt Oscuro (Noche)").performScrollTo().performClick()
        rule.runOnIdle {
            current.value = WorkspaceViewModel(app)
            assertTrue(current.value.showOnboarding)
            assertEquals(AppTheme.Dark, current.value.theme)
            assertTrue(current.value.prefs.getBoolean("first_run", true))
        }
        rule.onNodeWithTag("code_editor").assertDoesNotExist()
        rule.onNodeWithText("Comenzar a programar").performScrollTo().assertIsDisplayed()
        rule.onRoot().captureRoboImage("build/outputs/screenshots/configuracion-horizontal.png")
        rule.onNodeWithText("Comenzar a programar").performClick()
        rule.onNodeWithTag("code_editor").assertExists()
    }

    @Test fun configuredUsersCanReopenSetupWithoutLosingTheirDocument() {
        app.getSharedPreferences("pseint_prefs", 0).edit().putBoolean("first_run", false).commit()
        val vm = WorkspaceViewModel(app)
        val source = "Algoritmo T\nEscribir 42\nFinAlgoritmo"
        vm.openDocument(source, "MiPrograma.psc")
        rule.setContent { PSeIntApp(vm) }
        rule.onNodeWithTag("code_editor").assertExists()
        rule.onNodeWithText("Ajustes").performClick()
        rule.onNodeWithText("Reabrir Asistente de Configuración Inicial").performClick()
        rule.onNodeWithText("Configuración inicial").assertIsDisplayed()
        rule.runOnIdle {
            // Reopening is optional; it must not reset the completed first-run flag.
            assertFalse(WorkspaceViewModel(app).showOnboarding)
        }
        rule.onNodeWithText("Comenzar a programar").performScrollTo().performClick()
        rule.onNodeWithText("Editor", useUnmergedTree = true).performClick()
        rule.onNodeWithTag("code_editor").assertExists()
        rule.runOnIdle {
            assertEquals(source, vm.editor.text)
            assertEquals("MiPrograma.psc", vm.fileName)
        }
    }
}
