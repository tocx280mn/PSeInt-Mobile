package com.example

import androidx.compose.runtime.CompositionLocalProvider
import androidx.compose.ui.platform.LocalDensity
import androidx.compose.ui.test.junit4.createComposeRule
import androidx.compose.ui.text.rememberTextMeasurer
import androidx.compose.ui.unit.Density
import org.junit.Assert.*
import org.junit.Rule
import org.junit.Test
import org.junit.runner.RunWith
import org.robolectric.RobolectricTestRunner
import org.robolectric.annotation.Config
import org.robolectric.annotation.GraphicsMode

@RunWith(RobolectricTestRunner::class)
@GraphicsMode(GraphicsMode.Mode.NATIVE)
@Config(sdk = [36])
class DiagramGeometryTest {
    @get:Rule val rule = createComposeRule()
    @Test fun nestedBranchesAndLoopBodiesStayWithinExportBounds() {
        val code = """Algoritmo Geometria
Si x > 0 Entonces
    Para i <- 1 Hasta 10 Hacer
        Si i > 3 Entonces
            Escribir "Una frase larga que debe seguir dentro de la figura y no cortarse"
        Sino
            Leer dato
        FinSi
    FinPara
Sino
    Repetir
        Segun opcion Hacer
            1, 2: Escribir "dos opciones"
            3: Escribir "tres"
        FinSegun
    Hasta Que opcion = 0
FinSi
FinAlgoritmo"""
        val layouts = mutableListOf<DiagramLayout>()
        rule.setContent {
            CompositionLocalProvider(LocalDensity provides Density(1f)) {
                val measurer = rememberTextMeasurer()
                layouts.clear()
                layouts += DiagramRenderer.layoutDiagram(DiagramParser.parse(code.lines()), measurer, false, false)
                layouts += DiagramRenderer.layoutDiagram(DiagramParser.parse(code.lines()), measurer, false, true)
            }
        }
        rule.runOnIdle {
            layouts.forEach { layout ->
                assertTrue(layout.width > 0 && layout.height > 0)
                assertEquals(11, layout.hitTargets.size)
                layout.hitTargets.forEach { target ->
                    assertTrue(target.toString(), target.bounds.left >= 0 && target.bounds.top >= 0 && target.bounds.right <= layout.width && target.bounds.bottom <= layout.height)
                    assertEquals(target, layout.hitTest(target.bounds.center))
                }
                layout.hitTargets.forEachIndexed { i, a ->
                    layout.hitTargets.drop(i + 1).forEach { b -> assertFalse("Se superponen ${a.text} y ${b.text}", a.bounds.overlaps(b.bounds)) }
                }
            }
        }
    }
}
