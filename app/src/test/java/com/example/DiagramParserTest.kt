package com.example

import org.junit.Assert.*
import org.junit.Test

class DiagramParserTest {
    @Test fun preservesOriginalLinesAndStringContents() {
        val nodes = DiagramParser.parse(listOf("Algoritmo T", "", "// comentario", "Escribir \"https://ejemplo/a;b:c\";", "FinAlgoritmo"))
        assertEquals(3, nodes.size)
        assertTrue(nodes.first() is DiagramAstNode.Terminator)
        assertEquals(3, nodes[1].lineIndex)
        assertEquals("Escribir \"https://ejemplo/a;b:c\"", nodes[1].text)
        assertFalse((nodes.last() as DiagramAstNode.Terminator).isStart)
    }
    @Test fun repeatTargetsConditionInsteadOfOpeningLine() {
        val loop = DiagramParser.parse(listOf("Repetir", "x <- x + 1", "Hasta Que x = 3"))[0] as DiagramAstNode.DoWhile
        assertEquals(2, loop.lineIndex)
        assertEquals(0, loop.startLineIndex)
        assertEquals(1, loop.body.size)
    }
    @Test fun repeatWhileConditionKeepsItsLoopDirection() {
        val loop = DiagramParser.parse(listOf("Repetir", "x <- x + 1", "Mientras Que x < 3"))[0] as DiagramAstNode.DoWhile
        assertEquals(2, loop.lineIndex)
        assertTrue(loop.continueWhileTrue)
        assertEquals(1, loop.body.size)
    }
    @Test fun nestedSelectionAndInlineCasesAreRetained() {
        val nodes = DiagramParser.parse("""Segun x Hacer
1, 2:
    Segun y Hacer
        -1: Escribir "sí: no; //"
        De Otro Modo: Escribir "otro"
    FinSegun
3: Escribir "tres"
De Otro Modo: Escribir "fin"
FinSegun""".lines())
        val selection = nodes.single() as DiagramAstNode.Switch
        assertEquals(listOf("1, 2", "3"), selection.cases.map { it.first })
        val nested = selection.cases.first().second.single() as DiagramAstNode.Switch
        assertEquals("-1", nested.cases.single().first)
        assertEquals(3, nested.cases.single().second.single().lineIndex)
        assertEquals("Escribir \"sí: no; //\"", nested.cases.single().second.single().text)
        assertEquals(1, selection.defaultBranch!!.size)
    }
    @Test fun forIsNotMisclassifiedAsWhileAndIncompleteBlocksTerminate() {
        val nodes = DiagramParser.parse("Algoritmo T\nPara i <- 1 Hasta 3 Hacer\nSi x Entonces\nEscribir i\nFinPara\nFinAlgoritmo".lines())
        assertTrue(nodes[1] is DiagramAstNode.For)
        assertTrue(nodes.last() is DiagramAstNode.Terminator)
    }
}
