package com.example

import android.app.Application
import androidx.compose.ui.input.key.Key
import androidx.compose.ui.test.*
import androidx.compose.ui.test.junit4.createComposeRule
import androidx.test.core.app.ApplicationProvider
import com.github.takahirom.roborazzi.captureRoboImage
import com.github.takahirom.roborazzi.captureScreenRoboImage
import com.github.takahirom.roborazzi.ExperimentalRoborazziApi
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
@OptIn(ExperimentalTestApi::class, ExperimentalRoborazziApi::class)
@GraphicsMode(GraphicsMode.Mode.NATIVE)
@Config(qualifiers = "w393dp-h852dp-mdpi", sdk = [36])
class EditorAssistanceScreenshotTest {
    @get:Rule val rule = createComposeRule()
    private lateinit var vm: WorkspaceViewModel
    private val app get() = ApplicationProvider.getApplicationContext<Application>()
    @Before fun setup() {
        app.getSharedPreferences("pseint_prefs", 0).edit().clear().putBoolean("first_run", false).commit()
        vm = WorkspaceViewModel(app)
        vm.openDocument("", "Programa.psc", saved = false)
        File("build/outputs/screenshots").mkdirs()
    }
    @Test fun tapCompletionKeepsEditorFocusedAndSupportsUndo() {
        rule.setContent { PSeIntApp(vm) }
        rule.onNodeWithTag("code_editor").performTextInput("Esc")
        rule.onNodeWithTag("completion_Escribir").assertIsDisplayed()
        captureScreenRoboImage("build/outputs/screenshots/autocompletado-escribir.png")
        rule.onNodeWithTag("completion_Escribir").performClick()
        rule.runOnIdle { assertEquals("Escribir ", vm.editor.text) }
        rule.onNodeWithTag("code_editor").assertIsFocused()
        rule.onNodeWithTag("editor_call_tip").assertIsDisplayed()
        captureScreenRoboImage("build/outputs/screenshots/ayuda-escribir.png")
        rule.runOnIdle { vm.undo(); assertEquals("Esc", vm.editor.text) }
        rule.onNodeWithTag("editor_assistance").assertDoesNotExist()
        rule.runOnIdle { vm.redo(); assertEquals("Escribir ", vm.editor.text) }
    }
    @Test fun keyboardCompletionAndPersistentSettings() {
        rule.setContent { PSeIntApp(vm) }
        rule.onNodeWithTag("code_editor").performTextInput("Definir variable Com")
        rule.onNodeWithTag("completion_Como Entero").assertIsDisplayed()
        captureScreenRoboImage("build/outputs/screenshots/autocompletado-tipos.png")
        rule.onNodeWithTag("code_editor").performKeyInput { pressKey(Key.DirectionDown); pressKey(Key.Tab) }
        rule.runOnIdle { assertEquals("Definir variable Como Entero ", vm.editor.text) }
        rule.onNodeWithText("Ajustes").performClick()
        val autocompleteSwitch = hasContentDescription("Utilizar autocompletado") and isToggleable()
        rule.onNode(autocompleteSwitch).performScrollTo().assertIsOn().performClick()
        val helpSwitch = hasContentDescription("Utilizar ayudas emergentes") and isToggleable()
        rule.onNode(helpSwitch).performScrollTo().assertIsOn().performClick()
        rule.onRoot().captureRoboImage("build/outputs/screenshots/ajustes-autocompletado.png")
        rule.runOnIdle {
            val restored = WorkspaceViewModel(app)
            assertFalse(restored.autocomplete); assertFalse(restored.callTips)
        }
        rule.onNodeWithText("Editor", useUnmergedTree = true).performClick()
        rule.onNodeWithTag("code_editor").performTextReplacement("Esc")
        rule.onNodeWithTag("editor_assistance").assertDoesNotExist()
        rule.onNodeWithTag("code_editor").performKeyInput { pressKey(Key.Tab) }
        rule.runOnIdle { assertEquals("Esc    ", vm.editor.text) }
    }
    @Test @Config(qualifiers = "w640dp-h360dp-mdpi", sdk = [36])
    fun darkLandscapeAndDismissal() {
        vm.theme = AppTheme.Dark
        rule.setContent { PSeIntApp(vm) }
        rule.onNodeWithTag("code_editor").performTextInput("Definir n Com")
        rule.onNodeWithTag("completion_Como Entero").assertIsDisplayed()
        captureScreenRoboImage("build/outputs/screenshots/autocompletado-oscuro.png")
        rule.onNodeWithTag("code_editor").performKeyInput { pressKey(Key.Escape) }
        rule.onNodeWithTag("editor_assistance").assertDoesNotExist()
        rule.onNodeWithTag("code_editor").performTextReplacement("// Esc")
        rule.onNodeWithTag("editor_assistance").assertDoesNotExist()
    }

    @Test fun completeCatalogWorksWithTouchAndScrollableConditions() {
        rule.setContent { PSeIntApp(vm) }
        rule.onNodeWithTag("code_editor").performTextInput("Escribir FechaA")
        rule.onNodeWithTag("completion_FechaActual").assertIsDisplayed().performClick()
        rule.runOnIdle { assertEquals("Escribir FechaActual()", vm.editor.text) }
        rule.onNodeWithTag("editor_call_tip").assertIsDisplayed()
        captureScreenRoboImage("build/outputs/screenshots/autocompletado-fecha.png")
        rule.onNodeWithTag("code_editor").performTextReplacement("Si dato Es ")
        rule.onNodeWithTag("completion_list").performScrollToNode(hasTestTag("completion_Es Positivo"))
        rule.onNodeWithTag("completion_Es Positivo").assertIsDisplayed()
        captureScreenRoboImage("build/outputs/screenshots/autocompletado-condiciones.png")
        rule.onNodeWithTag("completion_Es Positivo").performClick()
        rule.runOnIdle { assertEquals("Si dato Es Positivo ", vm.editor.text) }
    }
}
