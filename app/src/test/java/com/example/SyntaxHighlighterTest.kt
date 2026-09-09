package com.example

import org.junit.Assert.assertEquals
import org.junit.Assert.assertFalse
import org.junit.Assert.assertTrue
import org.junit.Test

class SyntaxHighlighterTest {
    @Test fun urlInsideStringDoesNotStartComment() {
        val source = "Escribir \"https://ejemplo.test\" // comentario"
        val tokens = tokenizePSeInt(source)
        assertEquals(listOf(SyntaxTokenKind.Keyword, SyntaxTokenKind.String, SyntaxTokenKind.Comment), tokens.map { it.kind })
        assertEquals("// comentario", source.substring(tokens.last().start, tokens.last().end))
    }

    @Test fun stringsInsideCommentsStayComments() {
        val tokens = tokenizePSeInt("// Escribir \"texto\" 123")
        assertEquals(1, tokens.size)
        assertEquals(SyntaxTokenKind.Comment, tokens.single().kind)
    }

    @Test fun supportsSingleQuotesAndAccentedKeywords() {
        val source = "Según opción Hacer\nEscribir 'sí // literal'"
        val tokens = tokenizePSeInt(source)
        assertTrue(tokens.any { source.substring(it.start, it.end) == "Según" && it.kind == SyntaxTokenKind.Keyword })
        assertEquals(SyntaxTokenKind.String, tokens.last().kind)
        assertFalse(tokens.any { it.kind == SyntaxTokenKind.Comment })
    }

    @Test fun unfinishedStringStopsAtLineBoundary() {
        val source = "Escribir \"sin cerrar\nLeer x"
        val tokens = tokenizePSeInt(source)
        assertEquals(source.indexOf('\n'), tokens.first { it.kind == SyntaxTokenKind.String }.end)
        assertTrue(tokens.any { source.substring(it.start, it.end) == "Leer" && it.kind == SyntaxTokenKind.Keyword })
    }

    @Test fun scientificNumberAndIdentifierAreNotSplitIntoKeywords() {
        val source = "sinoValor <- 2.5e-3 + abs(-2)"
        val tokens = tokenizePSeInt(source)
        assertFalse(tokens.any { it.start == 0 })
        assertTrue(tokens.any { it.kind == SyntaxTokenKind.Number && source.substring(it.start, it.end) == "2.5e-3" })
        assertTrue(tokens.any { it.kind == SyntaxTokenKind.Function && source.substring(it.start, it.end) == "abs" })
    }
}
