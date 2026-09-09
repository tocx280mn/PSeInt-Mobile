package com.example

import androidx.compose.ui.geometry.CornerRadius
import androidx.compose.ui.geometry.Offset
import androidx.compose.ui.geometry.Rect
import androidx.compose.ui.geometry.Size
import androidx.compose.ui.graphics.Color
import androidx.compose.ui.graphics.Path
import androidx.compose.ui.graphics.drawscope.DrawScope
import androidx.compose.ui.graphics.drawscope.Stroke
import androidx.compose.ui.graphics.drawscope.withTransform
import androidx.compose.ui.text.TextLayoutResult
import androidx.compose.ui.text.TextMeasurer
import androidx.compose.ui.text.TextStyle
import androidx.compose.ui.text.drawText
import androidx.compose.ui.text.font.FontFamily
import androidx.compose.ui.text.style.TextAlign
import androidx.compose.ui.unit.Constraints
import androidx.compose.ui.unit.sp
import kotlin.math.*

data class DiagramHitTarget(val bounds: Rect, val lineIndex: Int, val text: String)
class DiagramLayout(
    val width: Float,
    val height: Float,
    val hitTargets: List<DiagramHitTarget>,
    val draw: DrawScope.() -> Unit
) {
    fun hitTest(point: Offset): DiagramHitTarget? = hitTargets.lastOrNull { it.bounds.contains(point) }
}

/** Measured once, then reused for the viewport, hit testing and PNG export. */
object DiagramRenderer {
    fun layoutDiagram(nodes: List<DiagramAstNode>, textMeasurer: TextMeasurer, isDark: Boolean, isNassiShneiderman: Boolean, alternativeIo: Boolean = false): DiagramLayout {
        val builder = Builder(textMeasurer, isDark, alternativeIo)
        val visible = nodes.filterNot { it is DiagramAstNode.Process && it.text.startsWith("Definir ", true) }
        val block = if (isNassiShneiderman) builder.nassiSequence(visible) else builder.sequence(visible)
        val padding = 28f
        val drawing = block.shifted(padding, padding)
        return DiagramLayout(block.width + padding * 2, block.height + padding * 2, drawing.targets) {
            drawing.operations.forEach { it(this) }
        }
    }

    private enum class Shape { Rectangle, Parallelogram, Diamond, Ellipse, Terminal, Triangle, ManualInput, Display }
    private class Block(val width: Float, val height: Float, val axis: Float = width / 2) {
        val operations = mutableListOf<DrawScope.() -> Unit>()
        val targets = mutableListOf<DiagramHitTarget>()
        fun add(other: Block, x: Float = 0f, y: Float = 0f) {
            operations += {
                withTransform({ translate(x, y) }) { other.operations.forEach { it(this) } }
            }
            targets += other.targets.map { it.copy(bounds = it.bounds.translate(Offset(x, y))) }
        }
        fun shifted(x: Float, y: Float) = Block(width, height, axis).also { it.add(this, x, y) }
    }

    private class Builder(val measurer: TextMeasurer, val dark: Boolean, val alternativeIo: Boolean) {
        val gap = 32f
        val arrow = if (dark) Color(0xFFFF8888) else Color(0xFFCC0000)
        val ink = if (dark) Color(0xFFFAFAFA) else Color(0xFF111111)
        val paper = if (dark) Color(0xFF333333) else Color(0xFFFAFAFA)
        val ioArrow = if (dark) Color(0xFFA9A9FA) else Color(0xFF000080)
        // Palette and border transformation from psdraw3/Global.cpp::SetColors.
        fun base(node: DiagramAstNode): Color = when (node) {
            is DiagramAstNode.Input -> Color(0xFFFFE5F2)
            is DiagramAstNode.Output -> Color(0xFFE5FFE5)
            is DiagramAstNode.If, is DiagramAstNode.Switch -> Color(0xFFD8E5FF)
            is DiagramAstNode.While, is DiagramAstNode.For -> Color(0xFFF2CCFF)
            is DiagramAstNode.DoWhile -> Color(0xFFCCB2FF)
            is DiagramAstNode.Terminator -> Color(0xFFFFF2CC)
            else -> Color(0xFFFFFFD8)
        }
        fun fill(node: DiagramAstNode): Color {
            val color = base(node)
            return if (!dark) color else Color(color.red.pow(4) * .1f + paper.red * .9f,
                color.green.pow(4) * .1f + paper.green * .9f, color.blue.pow(4) * .1f + paper.blue * .9f)
        }
        fun border(node: DiagramAstNode): Color {
            val c = base(node)
            return if (dark) Color(c.red.pow(4), c.green.pow(4), c.blue.pow(4))
            else Color(c.red.pow(7) * .75f, c.green.pow(7) * .75f, c.blue.pow(7) * .75f)
        }
        fun label(text: String, width: Int = 270, color: Color = ink): TextLayoutResult = measurer.measure(
            text, TextStyle(fontFamily = FontFamily.Monospace, fontSize = 14.sp, lineHeight = 19.sp, color = color, textAlign = TextAlign.Center),
            constraints = Constraints(maxWidth = width.coerceAtLeast(32))
        )
        fun labelAt(block: Block, text: String, center: Offset, width: Int = 270, color: Color = ink) {
            val measured = label(text, width, color)
            block.operations += { drawText(measured, topLeft = Offset(center.x - measured.size.width / 2, center.y - measured.size.height / 2)) }
        }
        fun connector(block: Block, points: List<Offset>, head: Boolean = true, color: Color = arrow) {
            if (points.size < 2) return
            block.operations += {
                points.zipWithNext().forEach { (a, b) -> drawLine(color, a, b, 1.5f) }
                if (head) {
                    val last = points.last()
                    val previous = points.dropLast(1).lastOrNull { (last - it).getDistance() > 1f }
                    if (previous != null) {
                        val direction = (last - previous) / (last - previous).getDistance()
                        val normal = Offset(-direction.y, direction.x)
                        val path = Path().apply {
                            moveTo(last.x, last.y)
                            val left = last - direction * 7f + normal * 3.3f
                            val right = last - direction * 7f - normal * 3.3f
                            lineTo(left.x, left.y); lineTo(right.x, right.y); close()
                        }
                        drawPath(path, color)
                    }
                }
            }
        }
        fun clean(node: DiagramAstNode): String {
            val text = node.text.trim().trimEnd(';')
            return when (node) {
                is DiagramAstNode.Terminator -> if (node.isStart) text.replace(Regex("(?i)^(algoritmo|proceso|funcion|subproceso)\\s*"), "").ifBlank { "Inicio" } else "Fin"
                is DiagramAstNode.Input -> text.replace(Regex("(?i)^leer\\s*"), "")
                is DiagramAstNode.Output -> text.replace(Regex("(?i)^(escribir|mostrar|imprimir)\\s*"), "")
                is DiagramAstNode.If -> text.replace(Regex("(?i)^si\\s*|\\s+entonces$"), "")
                is DiagramAstNode.While -> text.replace(Regex("(?i)^mientras\\s*|\\s+hacer$"), "")
                is DiagramAstNode.For -> text.replace(Regex("(?i)^para\\s*|\\s+hacer$"), "").replace("<-", "←")
                is DiagramAstNode.DoWhile -> text.replace(Regex("(?i)^(hasta|mientras)\\s+que\\s*"), "")
                is DiagramAstNode.Switch -> text.replace(Regex("(?i)^seg[uú]n\\s*|\\s+hacer$"), "")
                else -> text.replace("<-", "←")
            }
        }
        fun shape(node: DiagramAstNode, kind: Shape, text: String = clean(node)): Block {
            val measured = label(text)
            val diamond = kind == Shape.Diamond
            val width = max(if (diamond) 130f else 110f, measured.size.width * (if (diamond) 2f else 1f) + if (diamond) 36f else 52f)
            val height = max(if (diamond) 72f else 40f, measured.size.height * (if (diamond) 2f else 1f) + if (diamond) 24f else 22f)
            val block = Block(width, height)
            block.targets += DiagramHitTarget(Rect(0f, 0f, width, height), node.lineIndex, node.text)
            block.operations += {
                val bounds = Size(width, height)
                val color = fill(node); val outline = border(node)
                when (kind) {
                    Shape.Rectangle -> { drawRect(color, size = bounds); drawRect(outline, size = bounds, style = Stroke(1.4f)) }
                    Shape.Ellipse -> { drawOval(color, size = bounds); drawOval(outline, size = bounds, style = Stroke(1.4f)) }
                    Shape.Terminal -> {
                        drawRoundRect(color, size = bounds, cornerRadius = CornerRadius(height / 2))
                        drawRoundRect(outline, size = bounds, cornerRadius = CornerRadius(height / 2), style = Stroke(1.4f))
                    }
                    else -> {
                        val path = Path().apply {
                            when (kind) {
                                Shape.Diamond -> { moveTo(width / 2, 0f); lineTo(width, height / 2); lineTo(width / 2, height); lineTo(0f, height / 2) }
                                Shape.Triangle -> { moveTo(0f, 0f); lineTo(width, 0f); lineTo(width / 2, height) }
                                Shape.ManualInput -> { moveTo(0f, 10f); lineTo(width, 0f); lineTo(width, height); lineTo(0f, height) }
                                Shape.Display -> {
                                    moveTo(24f, 0f); lineTo(width - 25f, 0f)
                                    cubicTo(width + 8f, 0f, width + 8f, height, width - 25f, height)
                                    lineTo(24f, height); lineTo(0f, height / 2)
                                }
                                else -> { moveTo(0f, 0f); lineTo(width - 18f, 0f); lineTo(width, height); lineTo(18f, height) }
                            }
                            close()
                        }
                        drawPath(path, color); drawPath(path, outline, style = Stroke(1.4f))
                    }
                }
                drawText(measured, topLeft = Offset((width - measured.size.width) / 2, (height - measured.size.height) / 2 - if (kind == Shape.Triangle) 7f else 0f))
            }
            if (!alternativeIo && (node is DiagramAstNode.Input || node is DiagramAstNode.Output)) {
                val points = listOf(Offset(width - 26f, 9f), Offset(width - 10f, 9f))
                connector(block, if (node is DiagramAstNode.Input) points.reversed() else points, color = ioArrow)
            }
            return block
        }

        fun sequence(nodes: List<DiagramAstNode>): Block {
            val visible = nodes.filterNot { it is DiagramAstNode.Process && it.text.startsWith("Definir ", true) }
            if (visible.isEmpty()) return Block(48f, 16f).also { connector(it, listOf(Offset(24f, 0f), Offset(24f, 16f)), false) }
            val children = visible.map(::classic)
            val left = children.maxOf { it.axis }
            val right = children.maxOf { it.width - it.axis }
            val spacings = visible.zipWithNext().map { (a, b) -> if ((a is DiagramAstNode.Terminator && !a.isStart) || (b is DiagramAstNode.Terminator && b.isStart)) 64f else gap }
            val block = Block(left + right, children.sumOf { it.height.toDouble() }.toFloat() + spacings.sum(), left)
            var y = 0f
            children.forEachIndexed { i, child ->
                block.add(child, left - child.axis, y)
                y += child.height
                if (i < children.lastIndex) {
                    if (spacings[i] == gap) connector(block, listOf(Offset(left, y), Offset(left, y + gap)))
                    y += spacings[i]
                }
            }
            return block
        }

        fun classic(node: DiagramAstNode): Block = when (node) {
            is DiagramAstNode.If -> decision(node)
            is DiagramAstNode.While -> loop(node, node.body)
            is DiagramAstNode.For -> loop(node, node.body)
            is DiagramAstNode.DoWhile -> repeat(node)
            is DiagramAstNode.Switch -> switch(node)
            is DiagramAstNode.Terminator -> shape(node, Shape.Terminal)
            is DiagramAstNode.Input -> shape(node, if (alternativeIo) Shape.ManualInput else Shape.Parallelogram)
            is DiagramAstNode.Output -> shape(node, if (alternativeIo) Shape.Display else Shape.Parallelogram)
            else -> shape(node, Shape.Rectangle)
        }

        fun decision(node: DiagramAstNode.If): Block {
            val header = shape(node, Shape.Diamond)
            val yes = sequence(node.trueBranch); val no = sequence(node.falseBranch)
            val separation = max(48f, header.width + 48f - 2 * min(yes.width - yes.axis, no.axis))
            val axis = yes.width + separation / 2
            val bodyY = header.height + gap
            val joinY = bodyY + max(yes.height, no.height) + gap
            val block = Block(yes.width + no.width + separation, joinY, axis)
            block.add(header, axis - header.axis)
            block.add(yes, 0f, bodyY); block.add(no, yes.width + separation, bodyY)
            val right = yes.width + separation + no.axis
            connector(block, listOf(Offset(axis - header.width / 2, header.height / 2), Offset(yes.axis, header.height / 2), Offset(yes.axis, bodyY)))
            connector(block, listOf(Offset(axis + header.width / 2, header.height / 2), Offset(right, header.height / 2), Offset(right, bodyY)))
            labelAt(block, "V", Offset(yes.axis + 10, header.height / 2 + 14), color = arrow)
            labelAt(block, "F", Offset(right - 10, header.height / 2 + 14), color = arrow)
            connector(block, listOf(Offset(yes.axis, bodyY + yes.height), Offset(yes.axis, joinY), Offset(axis, joinY)), false)
            connector(block, listOf(Offset(right, bodyY + no.height), Offset(right, joinY), Offset(axis, joinY)), false)
            return block
        }

        fun loop(node: DiagramAstNode, body: List<DiagramAstNode>): Block {
            val header = shape(node, if (node is DiagramAstNode.For) Shape.Ellipse else Shape.Diamond)
            val content = sequence(body)
            val left = max(content.axis, header.width / 2) + 44f
            val right = max(content.width - content.axis, header.width / 2) + 44f
            val bodyY = 26f + header.height + gap
            val returnY = bodyY + content.height + gap
            val exitY = returnY + 24f
            val block = Block(left + right, exitY, left)
            block.add(header, left - header.axis, 26f)
            block.add(content, left - content.axis, bodyY)
            connector(block, listOf(Offset(left, 0f), Offset(left, 26f)))
            connector(block, listOf(Offset(left, 26f + header.height), Offset(left, bodyY)))
            connector(block, listOf(Offset(left, bodyY + content.height), Offset(left, returnY), Offset(12f, returnY), Offset(12f, 12f), Offset(left, 12f)))
            connector(block, listOf(Offset(left + header.width / 2, 26f + header.height / 2), Offset(block.width - 12f, 26f + header.height / 2), Offset(block.width - 12f, exitY), Offset(left, exitY)), false)
            labelAt(block, "V", Offset(left + 12f, 26f + header.height + 15f), color = arrow)
            labelAt(block, "F", Offset(block.width - 25f, 26f + header.height / 2 - 14f), color = arrow)
            return block
        }

        fun repeat(node: DiagramAstNode.DoWhile): Block {
            val header = shape(node, Shape.Diamond)
            val content = sequence(node.body)
            val left = max(content.axis, header.width / 2) + 44f
            val right = max(content.width - content.axis, header.width / 2) + 20f
            val conditionY = 24f + content.height + gap
            val block = Block(left + right, conditionY + header.height + gap, left)
            block.add(content, left - content.axis, 24f)
            block.add(header, left - header.axis, conditionY)
            connector(block, listOf(Offset(left, 0f), Offset(left, 24f)))
            connector(block, listOf(Offset(left, 24f + content.height), Offset(left, conditionY)))
            connector(block, listOf(Offset(left - header.width / 2, conditionY + header.height / 2), Offset(12f, conditionY + header.height / 2), Offset(12f, 12f), Offset(left, 12f)))
            connector(block, listOf(Offset(left, conditionY + header.height), Offset(left, block.height)), false)
            labelAt(block, if (node.continueWhileTrue) "V" else "F", Offset(25f, conditionY + header.height / 2 - 14f), color = arrow)
            labelAt(block, if (node.continueWhileTrue) "F" else "V", Offset(left + 12f, block.height - 16f), color = arrow)
            return block
        }

        fun switch(node: DiagramAstNode.Switch): Block {
            val header = shape(node, Shape.Triangle)
            val branches = node.cases + listOf("De otro modo" to (node.defaultBranch ?: emptyList()))
            val bodies = branches.map { sequence(it.second) }
            val widths = bodies.mapIndexed { index, block -> max(2 * max(block.axis, block.width - block.axis), label(branches[index].first).size.width + 30f) }
            val total = max(header.width + 40f, widths.sum() + gap * (widths.size - 1))
            val axis = total / 2
            val splitY = header.height + gap
            val bodyY = splitY + 38f
            val joinY = bodyY + bodies.maxOf { it.height } + gap
            val block = Block(total, joinY, axis)
            block.add(header, axis - header.axis)
            connector(block, listOf(Offset(axis, header.height), Offset(axis, splitY)), false)
            var x = (total - widths.sum() - gap * (widths.size - 1)) / 2
            bodies.forEachIndexed { index, child ->
                val branchAxis = x + widths[index] / 2
                val labelText = branches[index].first
                connector(block, listOf(Offset(axis, splitY), Offset(branchAxis, splitY), Offset(branchAxis, bodyY)))
                labelAt(block, labelText, Offset(branchAxis, splitY - 13f), widths[index].toInt())
                block.add(child, branchAxis - child.axis, bodyY)
                connector(block, listOf(Offset(branchAxis, bodyY + child.height), Offset(branchAxis, joinY), Offset(axis, joinY)), false)
                x += widths[index] + gap
            }
            return block
        }

        fun nassiMinWidth(node: DiagramAstNode): Float = when (node) {
            is DiagramAstNode.If -> max(260f, nassiWidth(node.trueBranch) + nassiWidth(node.falseBranch))
            is DiagramAstNode.Switch -> max(260f, (node.cases.map { it.second } + listOf(node.defaultBranch ?: emptyList())).sumOf { nassiWidth(it).toDouble() }.toFloat())
            is DiagramAstNode.While -> max(250f, nassiWidth(node.body) + 24f)
            is DiagramAstNode.For -> max(250f, nassiWidth(node.body) + 24f)
            is DiagramAstNode.DoWhile -> max(250f, nassiWidth(node.body) + 24f)
            else -> max(160f, label(clean(node)).size.width + 30f)
        }
        fun nassiWidth(nodes: List<DiagramAstNode>) = nodes.maxOfOrNull(::nassiMinWidth) ?: 120f
        fun nassiSequence(nodes: List<DiagramAstNode>, requestedWidth: Float = nassiWidth(nodes)): Block {
            val children = nodes.filterNot { it is DiagramAstNode.Process && it.text.startsWith("Definir ", true) }.map { nassi(it, requestedWidth) }
            if (children.isEmpty()) return Block(requestedWidth, 36f)
            val block = Block(requestedWidth, children.sumOf { it.height.toDouble() }.toFloat())
            var y = 0f
            children.forEach { block.add(it, 0f, y); y += it.height }
            return block
        }
        fun frame(block: Block, node: DiagramAstNode, bounds: Rect, hit: Boolean = true) {
            block.operations += {
                drawRect(fill(node), bounds.topLeft, bounds.size)
                drawRect(border(node), bounds.topLeft, bounds.size, style = Stroke(1.3f))
            }
            if (hit) block.targets += DiagramHitTarget(bounds, node.lineIndex, node.text)
        }
        fun nassi(node: DiagramAstNode, width: Float): Block {
            val body = when (node) {
                is DiagramAstNode.While -> node.body
                is DiagramAstNode.For -> node.body
                is DiagramAstNode.DoWhile -> node.body
                else -> null
            }
            if (body != null) {
                val content = nassiSequence(body, width - 24f)
                val text = if (node is DiagramAstNode.DoWhile) node.text else node.text.replace(Regex("(?i)\\s+hacer$"), "")
                val headerHeight = max(42f, label(text, (width - 24f).toInt()).size.height + 18f)
                val block = Block(width, content.height + headerHeight)
                frame(block, node, Rect(0f, 0f, width, block.height), false)
                val top = if (node is DiagramAstNode.DoWhile) content.height else 0f
                frame(block, node, Rect(0f, top, width, top + headerHeight))
                labelAt(block, text, Offset(width / 2, top + headerHeight / 2), (width - 20f).toInt())
                block.add(content, 24f, if (node is DiagramAstNode.DoWhile) 0f else headerHeight)
                block.operations += { drawLine(border(node), Offset(24f, 0f), Offset(24f, block.height), 1.3f) }
                return block
            }
            if (node is DiagramAstNode.If || node is DiagramAstNode.Switch) {
                val branches = if (node is DiagramAstNode.If) listOf("V" to node.trueBranch, "F" to node.falseBranch)
                    else (node as DiagramAstNode.Switch).cases + listOf("Otro" to (node.defaultBranch ?: emptyList()))
                val minimums = branches.map { nassiWidth(it.second) }
                val totalMin = minimums.sum()
                val widths = minimums.map { width * it / totalMin }
                val children = branches.mapIndexed { i, branch -> nassiSequence(branch.second, widths[i]) }
                val headerHeight = max(78f, label(clean(node), (width / 2).toInt()).size.height + 48f)
                val height = headerHeight + children.maxOf { it.height }
                val block = Block(width, height)
                frame(block, node, Rect(0f, 0f, width, height), false)
                frame(block, node, Rect(0f, 0f, width, headerHeight))
                val junction = Offset(width / 2, headerHeight - 27f)
                connector(block, listOf(Offset(0f, 0f), junction, Offset(width, 0f)), false, border(node))
                labelAt(block, clean(node), Offset(width / 2, (headerHeight - 27f) / 2), (width / 2).toInt())
                var x = 0f
                children.forEachIndexed { i, child ->
                    labelAt(block, branches[i].first, Offset(x + widths[i] / 2, headerHeight - 14f), (widths[i] - 8f).toInt())
                    block.add(child, x, headerHeight)
                    if (i > 0) connector(block, listOf(Offset(x, headerHeight - 27f), Offset(x, height)), false, border(node))
                    x += widths[i]
                }
                return block
            }
            val text = clean(node)
            val height = max(42f, label(text, (width - 24f).toInt()).size.height + 20f)
            val block = Block(width, height)
            frame(block, node, Rect(0f, 0f, width, height))
            labelAt(block, text, Offset(width / 2, height / 2), (width - 24f).toInt())
            return block
        }
    }
}
