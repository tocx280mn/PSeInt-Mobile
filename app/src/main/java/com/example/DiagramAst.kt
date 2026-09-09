package com.example

sealed class DiagramAstNode {
    abstract val text: String
    abstract val lineIndex: Int

    /** A source line number is always zero based in the unfiltered editor document. */
    data class Terminator(
        override val text: String,
        override val lineIndex: Int,
        val isStart: Boolean
    ) : DiagramAstNode()

    data class Process(override val text: String, override val lineIndex: Int) : DiagramAstNode()
    data class Input(override val text: String, override val lineIndex: Int) : DiagramAstNode()
    data class Output(override val text: String, override val lineIndex: Int) : DiagramAstNode()
    data class If(
        override val text: String,
        override val lineIndex: Int,
        val trueBranch: List<DiagramAstNode>,
        val falseBranch: List<DiagramAstNode>
    ) : DiagramAstNode()
    data class While(
        override val text: String,
        override val lineIndex: Int,
        val body: List<DiagramAstNode>
    ) : DiagramAstNode()
    data class DoWhile(
        override val text: String,
        override val lineIndex: Int,
        val body: List<DiagramAstNode>,
        val startLineIndex: Int = lineIndex,
        val continueWhileTrue: Boolean = false
    ) : DiagramAstNode()
    data class For(
        override val text: String,
        override val lineIndex: Int,
        val body: List<DiagramAstNode>
    ) : DiagramAstNode()
    data class Switch(
        override val text: String,
        override val lineIndex: Int,
        val cases: List<Pair<String, List<DiagramAstNode>>>,
        val defaultBranch: List<DiagramAstNode>?,
        val caseLineIndices: List<Int> = emptyList(),
        val defaultLineIndex: Int = lineIndex
    ) : DiagramAstNode()
}
