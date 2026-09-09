package com.example

import android.app.Application
import androidx.compose.ui.test.*
import androidx.compose.ui.test.junit4.createComposeRule
import androidx.test.core.app.ApplicationProvider
import com.github.takahirom.roborazzi.captureRoboImage
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
class WorkspaceScreenshotTest {
    @get:Rule val rule = createComposeRule()
    private lateinit var vm: WorkspaceViewModel
    @Before fun setup() {
        val app = ApplicationProvider.getApplicationContext<Application>()
        app.getSharedPreferences("pseint_prefs", 0).edit().clear().putBoolean("first_run", false).commit()
        vm = WorkspaceViewModel(app)
        File("build/outputs/screenshots").mkdirs()
    }
    private fun capture(name: String) {
        rule.waitForIdle()
        rule.onRoot().captureRoboImage("build/outputs/screenshots/$name.png")
    }
    private fun awaitState(condition: () -> Boolean) {
        try {
            rule.waitUntil(10_000) {
                org.robolectric.Shadows.shadowOf(android.os.Looper.getMainLooper()).idleFor(20, java.util.concurrent.TimeUnit.MILLISECONDS)
                condition()
            }
        } catch (e: androidx.compose.ui.test.ComposeTimeoutException) {
            throw AssertionError("${vm.executionStatus}; running=${vm.running}, paused=${vm.paused}, line=${vm.executingLine}, input=${vm.inputVariable}, output=${vm.output}", e)
        }
    }
    @Test fun editorAndSearch() {
        rule.setContent { PSeIntApp(vm) }
        rule.onNodeWithTag("code_editor").assertExists()
        capture("editor-clasico")
        rule.onNodeWithText("Editar", useUnmergedTree = true).performClick()
        rule.onNodeWithText("Buscar y reemplazar").performClick()
        rule.onNode(hasText("Buscar") and hasSetTextAction()).performTextInput("nombre")
        rule.onNodeWithContentDescription("Siguiente coincidencia").performClick()
        rule.runOnIdle { org.junit.Assert.assertEquals("nombre", vm.editor.text.substring(vm.editor.selection.min, vm.editor.selection.max)) }
        capture("buscar-reemplazar")
    }
    @Test fun classicFlowchart() {
        vm.openDocument(PSeIntExamples.all[1].code, "MayorDeEdad.psc")
        vm.tab = AppTab.Diagram
        rule.setContent { PSeIntApp(vm) }
        rule.onNodeWithTag("diagram-canvas").assertExists()
        capture("diagrama-clasico")
        rule.onNodeWithText("Clásico").performClick()
        capture("diagrama-nassi")
    }
    @Test fun nestedFlowchart() {
        vm.openDocument(PSeIntExamples.all[3].code, "MenuDeOpciones.psc")
        vm.tab = AppTab.Diagram
        rule.setContent { PSeIntApp(vm) }
        capture("diagrama-repetir-segun")
    }
    @Test fun darkEditorAndSettings() {
        vm.theme = AppTheme.Dark
        rule.setContent { PSeIntApp(vm) }
        capture("editor-oscuro")
        rule.onNodeWithText("Ajustes").performClick()
        capture("ajustes")
    }

    @Test fun consoleInputAndCancellation() {
        rule.setContent { PSeIntApp(vm) }
        rule.runOnIdle { vm.run() }
        awaitState { vm.inputVariable == "nombre" }
        capture("consola-entrada")
        rule.runOnIdle { vm.submitInput("Ada") }
        awaitState { !vm.running }
        rule.runOnIdle { org.junit.Assert.assertTrue(vm.output.joinToString("").contains("Hola, Ada")) }
        capture("consola-resultado")
        rule.runOnIdle { vm.run() }
        awaitState { vm.inputVariable != null }
        rule.runOnIdle { vm.stop() }
        awaitState { !vm.running && vm.inputVariable == null }
        rule.runOnIdle { org.junit.Assert.assertEquals("Ejecución detenida", vm.executionStatus) }
    }

    @Test fun debuggerRequiresOnePermitPerInstruction() {
        vm.openDocument("Algoritmo T\nx <- 1\nx <- 2\nEscribir x\nFinAlgoritmo", "Pasos.psc")
        rule.setContent { PSeIntApp(vm) }
        rule.runOnIdle { vm.run(true) }
        awaitState { vm.paused && vm.executingLine == 1 }
        rule.runOnIdle { vm.nextStep() }
        awaitState { vm.paused && vm.executingLine == 2 }
        rule.runOnIdle { org.junit.Assert.assertTrue(vm.output.isEmpty()); vm.nextStep() }
        awaitState { vm.executingLine == 3 }
        rule.runOnIdle { org.junit.Assert.assertEquals("1", vm.variables["x"]); vm.nextStep() }
        awaitState { vm.executingLine == 4 }
        capture("depuracion")
        rule.runOnIdle { org.junit.Assert.assertEquals("2", vm.variables["x"]); vm.nextStep() }
        awaitState { vm.executingLine == 5 }
        rule.runOnIdle { vm.nextStep() }
        awaitState { !vm.running }
        rule.runOnIdle { org.junit.Assert.assertEquals("2\n", vm.output.joinToString("")) }
    }

    @Test fun savedDraftRecoversAndUndoRemainsConsistent() {
        rule.setContent { PSeIntApp(vm) }
        rule.runOnIdle {
            vm.openDocument("Algoritmo T\nFinAlgoritmo", "Recuperado.psc")
            vm.edit(androidx.compose.ui.text.input.TextFieldValue("Algoritmo T\nEscribir 1\nFinAlgoritmo"))
            val recovered = WorkspaceViewModel(ApplicationProvider.getApplicationContext())
            org.junit.Assert.assertEquals(vm.editor.text, recovered.editor.text)
            org.junit.Assert.assertEquals("Recuperado.psc", recovered.fileName)
            org.junit.Assert.assertTrue(recovered.isModified)
            vm.undo()
            org.junit.Assert.assertEquals("Algoritmo T\nFinAlgoritmo", vm.editor.text)
            vm.redo()
            org.junit.Assert.assertTrue(vm.editor.text.contains("Escribir 1"))
            vm.markSaved("content://test/document.psc", vm.fileName, vm.editor.text)
            org.junit.Assert.assertFalse(vm.isModified)
        }
    }

    @Test fun profileConfiguresEditorCommandsDiagramAndSurvivesRestart() {
        val selected = PSeIntProfile.Flexible.copy(name="Mi escuela", preferAlgoritmo=false, preferFuncion=false,
            allowFunctions=false, requireSemicolons=true, useNassiShneiderman=true, alternativeIoShapes=true, editorFontSize=18)
        val original = vm.editor.text
        vm.selectProfile(selected)
        rule.setContent { PSeIntApp(vm) }
        rule.runOnIdle {
            org.junit.Assert.assertEquals(original,vm.editor.text)
            org.junit.Assert.assertEquals(18,vm.fontSize)
            org.junit.Assert.assertEquals(DiagramType.NassiShneiderman,vm.diagramType)
            val recovered=WorkspaceViewModel(ApplicationProvider.getApplicationContext())
            org.junit.Assert.assertEquals(selected.normalized(),recovered.profile)
            org.junit.Assert.assertEquals(18,recovered.fontSize)
            org.junit.Assert.assertEquals(DiagramType.NassiShneiderman,recovered.diagramType)
        }
        capture("perfil-editor")
        rule.runOnIdle {
            vm.openDocument(PSeIntExamples.all[1].code,"MayorDeEdad.psc")
            vm.diagramType=DiagramType.Classic
            vm.tab=AppTab.Diagram
        }
        capture("perfil-diagrama-alternativo")
    }
}
