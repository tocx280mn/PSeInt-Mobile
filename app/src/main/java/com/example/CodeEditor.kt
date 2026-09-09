package com.example

import android.graphics.Paint
import android.graphics.Typeface
import androidx.compose.foundation.Canvas
import androidx.compose.foundation.background
import androidx.compose.foundation.clickable
import androidx.compose.foundation.layout.Column
import androidx.compose.foundation.layout.heightIn
import androidx.compose.foundation.lazy.LazyColumn
import androidx.compose.foundation.lazy.itemsIndexed
import androidx.compose.foundation.lazy.rememberLazyListState
import androidx.compose.material3.Surface
import androidx.compose.material3.Text
import androidx.compose.foundation.horizontalScroll
import androidx.compose.foundation.layout.Box
import androidx.compose.foundation.layout.BoxWithConstraints
import androidx.compose.foundation.layout.Row
import androidx.compose.foundation.layout.fillMaxWidth
import androidx.compose.foundation.layout.height
import androidx.compose.foundation.layout.padding
import androidx.compose.foundation.layout.width
import androidx.compose.foundation.rememberScrollState
import androidx.compose.foundation.text.BasicTextField
import androidx.compose.foundation.text.KeyboardOptions
import androidx.compose.foundation.text.selection.LocalTextSelectionColors
import androidx.compose.foundation.text.selection.TextSelectionColors
import androidx.compose.foundation.verticalScroll
import androidx.compose.runtime.Composable
import androidx.compose.runtime.CompositionLocalProvider
import androidx.compose.runtime.LaunchedEffect
import androidx.compose.runtime.getValue
import androidx.compose.runtime.mutableStateOf
import androidx.compose.runtime.remember
import androidx.compose.runtime.setValue
import androidx.compose.ui.Modifier
import androidx.compose.ui.draw.clipToBounds
import androidx.compose.ui.draw.drawBehind
import androidx.compose.ui.geometry.Offset
import androidx.compose.ui.geometry.Size
import androidx.compose.ui.graphics.Color
import androidx.compose.ui.graphics.SolidColor
import androidx.compose.ui.graphics.nativeCanvas
import androidx.compose.ui.graphics.toArgb
import androidx.compose.ui.input.key.Key
import androidx.compose.ui.input.key.KeyEventType
import androidx.compose.ui.input.key.isShiftPressed
import androidx.compose.ui.focus.onFocusChanged
import androidx.compose.ui.input.key.key
import androidx.compose.ui.input.key.onPreviewKeyEvent
import androidx.compose.ui.input.key.type
import androidx.compose.ui.platform.LocalDensity
import androidx.compose.ui.platform.testTag
import androidx.compose.ui.semantics.contentDescription
import androidx.compose.ui.semantics.semantics
import androidx.compose.ui.text.AnnotatedString
import androidx.compose.ui.text.PlatformTextStyle
import androidx.compose.ui.text.TextLayoutResult
import androidx.compose.ui.text.TextStyle
import androidx.compose.ui.text.font.FontFamily
import androidx.compose.ui.text.input.ImeAction
import androidx.compose.ui.text.input.KeyboardCapitalization
import androidx.compose.ui.text.input.TextFieldValue
import androidx.compose.ui.text.rememberTextMeasurer
import androidx.compose.ui.unit.dp
import androidx.compose.ui.unit.sp
import androidx.compose.ui.unit.IntOffset
import androidx.compose.ui.unit.IntRect
import androidx.compose.ui.unit.IntSize
import androidx.compose.ui.unit.LayoutDirection
import androidx.compose.ui.window.Popup
import androidx.compose.ui.window.PopupPositionProvider
import androidx.compose.ui.window.PopupProperties
import kotlin.math.max

/** Desktop-style editor. Execution and error lines are one-based; -1 clears the marker. */
@Composable
fun PSeIntCodeEditor(
    value: TextFieldValue,
    onValueChange: (TextFieldValue) -> Unit,
    modifier: Modifier = Modifier,
    fontSizeSp: Int = 14,
    profile: PSeIntProfile = PSeIntProfile.Flexible,
    showLineNumbers: Boolean = true,
    darkTheme: Boolean = false,
    backgroundColor: Color? = null,
    textColor: Color? = null,
    gutterColor: Color? = null,
    currentExecutingLine: Int = -1,
    errorLine: Int = -1,
    readOnly: Boolean = false,
    autocompleteEnabled: Boolean = true,
    callTipsEnabled: Boolean = true
) {
    val background = backgroundColor ?: if (darkTheme) Color(0xFF23252A) else Color.White
    val foreground = textColor ?: if (darkTheme) Color(0xFFF0F0F0) else Color(0xFF111111)
    val gutterBackground = gutterColor ?: if (darkTheme) Color(0xFF202227) else Color(0xFFF6F6F6)
    val lineNumberColor = if (darkTheme) Color(0xFF9DA3AD) else Color(0xFF777C83)
    val separatorColor = if (darkTheme) Color(0xFF3A3E45) else Color(0xFFDDE0E4)
    val cursorLineColor = if (darkTheme) Color(0xFF2D343E) else Color(0xFFF0F6FF)
    val executionColor = if (darkTheme) Color(0xFF514C24) else Color(0xFFFFF1B8)
    val errorColor = if (darkTheme) Color(0xFF502B30) else Color(0xFFFFE6E3)
    val accent = if (darkTheme) Color(0xFF91BEFF) else Color(0xFF175A9E)
    val density = LocalDensity.current
    val verticalScroll = rememberScrollState()
    val horizontalScroll = rememberScrollState()
    val highlighter = remember(darkTheme, profile) { PSeIntSyntaxHighlighter(darkTheme, profile) }
    val textMeasurer = rememberTextMeasurer()
    val safeFontSize = fontSizeSp.coerceIn(10, 32)
    val textStyle = TextStyle(
        color = foreground,
        fontSize = safeFontSize.sp,
        lineHeight = (safeFontSize + 7).sp,
        fontFamily = FontFamily.Monospace,
        platformStyle = PlatformTextStyle(includeFontPadding = false)
    )
    val highlighted = remember(value.text, highlighter) { highlighter.filter(AnnotatedString(value.text)).text }
    val measuredText = textMeasurer.measure(highlighted, textStyle, softWrap = false)
    var actualLayout by remember { mutableStateOf<TextLayoutResult?>(null) }
    // Use actual glyph baselines for the gutter, so zoom and Android font scaling cannot drift.
    val textLayout = actualLayout?.takeIf { it.layoutInput.text.text == value.text && it.layoutInput.style == textStyle }
        ?: measuredText
    val lineCount = value.text.count { it == '\n' } + 1
    val cursorLine = value.text.take(value.selection.end.coerceIn(0, value.text.length)).count { it == '\n' }
    val linePaint = remember { Paint(Paint.ANTI_ALIAS_FLAG).apply { typeface = Typeface.MONOSPACE; textAlign = Paint.Align.RIGHT } }
    val verticalPadding = 10.dp
    val horizontalPadding = 12.dp
    val lineNumberWidth = with(density) { (safeFontSize.sp.toPx() * 0.65f * max(2, lineCount.toString().length)).toDp() } + 22.dp
    var focused by remember { mutableStateOf(false) }
    var typingValue by remember { mutableStateOf<TextFieldValue?>(null) }
    var dismissedValue by remember { mutableStateOf<TextFieldValue?>(null) }
    LaunchedEffect(value.text, value.selection) {
        if (typingValue?.text != value.text || typingValue?.selection != value.selection) typingValue = null
    }
    val assisting = focused && !readOnly && typingValue?.text == value.text &&
        typingValue?.selection == value.selection && dismissedValue != value
    val completions = remember(value, profile, autocompleteEnabled, assisting) {
        if (autocompleteEnabled && assisting) editorCompletions(value, profile) else emptyList()
    }
    val callTip = remember(value, profile, callTipsEnabled, assisting) {
        if (callTipsEnabled && assisting) editorCallTip(value, profile) else null
    }
    var selectedCompletion by remember(completions) { mutableStateOf(0) }
    val completionScroll = rememberLazyListState()
    LaunchedEffect(selectedCompletion, completions) {
        if (completions.isNotEmpty()) completionScroll.scrollToItem(selectedCompletion.coerceIn(completions.indices))
    }
    fun acceptCompletion(completion: EditorCompletion) {
        val next = applyEditorCompletion(value, completion)
        typingValue = next
        dismissedValue = null
        onValueChange(next)
    }

    CompositionLocalProvider(LocalTextSelectionColors provides TextSelectionColors(accent, accent.copy(alpha = 0.23f))) {
        BoxWithConstraints(modifier.background(background).clipToBounds()) {
            val gutterWidth = if (showLineNumbers) lineNumberWidth else 0.dp
            val viewportWidth = (maxWidth - gutterWidth).coerceAtLeast(1.dp)
            val contentWidth = maxOf(viewportWidth, with(density) { measuredText.size.width.toDp() } + horizontalPadding * 2 + 8.dp)
            val contentHeight = maxOf(maxHeight, with(density) { measuredText.size.height.toDp() } + verticalPadding * 2 + 24.dp)
            val viewportHeightPx = with(density) { maxHeight.toPx() }
            val paddingPx = with(density) { verticalPadding.toPx() }

            // Search, undo, and command insertion can move the caret while the field is unfocused.
            LaunchedEffect(value.selection, measuredText.size, viewportHeightPx, viewportWidth) {
                val cursor = measuredText.getCursorRect(value.selection.end.coerceIn(0, value.text.length))
                val top = cursor.top + paddingPx
                val bottom = cursor.bottom + paddingPx
                when {
                    top < verticalScroll.value -> verticalScroll.scrollTo((top - paddingPx).toInt().coerceAtLeast(0))
                    bottom > verticalScroll.value + viewportHeightPx -> verticalScroll.scrollTo((bottom - viewportHeightPx + paddingPx).toInt().coerceAtLeast(0))
                }
                val viewportWidthPx = with(density) { viewportWidth.toPx() }
                val inset = with(density) { horizontalPadding.toPx() }
                val left = cursor.left + inset
                val right = cursor.right + inset
                when {
                    left < horizontalScroll.value -> horizontalScroll.scrollTo((left - inset).toInt().coerceAtLeast(0))
                    right > horizontalScroll.value + viewportWidthPx - inset -> horizontalScroll.scrollTo((right - viewportWidthPx + inset).toInt().coerceAtLeast(0))
                }
            }

            LaunchedEffect(currentExecutingLine, errorLine, measuredText.lineCount, viewportHeightPx) {
                val markedLine = if (currentExecutingLine > 0) currentExecutingLine else errorLine
                if (markedLine in 1..measuredText.lineCount) {
                    val top = measuredText.getLineTop(markedLine - 1) + paddingPx
                    val bottom = measuredText.getLineBottom(markedLine - 1) + paddingPx
                    if (top < verticalScroll.value || bottom > verticalScroll.value + viewportHeightPx) {
                        verticalScroll.animateScrollTo((top - viewportHeightPx * 0.3f).toInt().coerceAtLeast(0))
                    }
                }
            }

            Row(Modifier.fillMaxWidth().verticalScroll(verticalScroll).height(contentHeight)) {
                if (showLineNumbers) {
                    Canvas(Modifier.width(gutterWidth).height(contentHeight).background(gutterBackground)) {
                        linePaint.textSize = safeFontSize.sp.toPx()
                        val firstVisible = textLayout.getLineForVerticalPosition((verticalScroll.value - paddingPx).coerceAtLeast(0f))
                        val lastVisible = textLayout.getLineForVerticalPosition(verticalScroll.value + viewportHeightPx)
                        for (line in firstVisible..lastVisible) {
                            val marked = line + 1 == currentExecutingLine || line + 1 == errorLine
                            val markerColor = if (line + 1 == errorLine) Color(0xFFD34238) else Color(0xFFD49D12)
                            if (marked) {
                                drawRect(markerColor, Offset(0f, paddingPx + textLayout.getLineTop(line)), Size(3.dp.toPx(), textLayout.getLineBottom(line) - textLayout.getLineTop(line)))
                            }
                            linePaint.color = (if (marked) markerColor else if (line == cursorLine) accent else lineNumberColor).toArgb()
                            drawContext.canvas.nativeCanvas.drawText((line + 1).toString(), size.width - 10.dp.toPx(), paddingPx + textLayout.getLineBaseline(line), linePaint)
                        }
                        drawLine(separatorColor, Offset(size.width - 1f, 0f), Offset(size.width - 1f, size.height))
                    }
                }
                Box(Modifier.weight(1f).horizontalScroll(horizontalScroll)) {
                    BasicTextField(
                        value = value,
                        onValueChange = {
                            val next = autoIndentEditorChange(value, it)
                            typingValue = if (next.text != value.text) next else null
                            dismissedValue = null
                            onValueChange(next)
                        },
                        readOnly = readOnly,
                        textStyle = textStyle,
                        keyboardOptions = KeyboardOptions(capitalization = KeyboardCapitalization.None, autoCorrectEnabled = false, imeAction = ImeAction.Default),
                        visualTransformation = highlighter,
                        cursorBrush = SolidColor(foreground),
                        onTextLayout = { actualLayout = it },
                        modifier = Modifier
                            .width(contentWidth)
                            .height(contentHeight)
                            .testTag("code_editor")
                            .semantics { contentDescription = "Editor de pseudocódigo" }
                            .onFocusChanged { focused = it.isFocused }
                            .onPreviewKeyEvent {
                                if (readOnly || it.type != KeyEventType.KeyDown) false
                                else when {
                                    completions.isNotEmpty() && !it.isShiftPressed && (it.key == Key.Tab || it.key == Key.Enter) -> {
                                        acceptCompletion(completions[selectedCompletion]); true
                                    }
                                    completions.isNotEmpty() && (it.key == Key.DirectionDown || it.key == Key.DirectionUp) -> {
                                        val direction = if (it.key == Key.DirectionDown) 1 else -1
                                        selectedCompletion = (selectedCompletion + direction).mod(completions.size); true
                                    }
                                    (completions.isNotEmpty() || callTip != null) && it.key == Key.Escape -> {
                                        dismissedValue = value; true
                                    }
                                    it.key == Key.Tab -> {
                                        typingValue = null
                                        onValueChange(indentEditorSelection(value, unindent = it.isShiftPressed)); true
                                    }
                                    else -> false
                                }
                            }
                            .padding(horizontal = horizontalPadding, vertical = verticalPadding)
                            .drawBehind {
                                fun markLine(line: Int, color: Color) {
                                    if (line in 0 until textLayout.lineCount) {
                                        val top = textLayout.getLineTop(line)
                                        drawRect(color, Offset(-horizontalPadding.toPx(), top), Size(size.width + horizontalPadding.toPx() * 2, textLayout.getLineBottom(line) - top))
                                    }
                                }
                                if (value.selection.collapsed) markLine(cursorLine, cursorLineColor)
                                markLine(errorLine - 1, errorColor)
                                markLine(currentExecutingLine - 1, executionColor)
                            }
                    )
                }
            }
            if (completions.isNotEmpty() || callTip != null) {
                val caret = textLayout.getCursorRect(value.selection.end.coerceIn(0, value.text.length))
                val x = with(density) { (gutterWidth + horizontalPadding).toPx() } + caret.left - horizontalScroll.value
                val top = paddingPx + caret.top - verticalScroll.value
                val bottom = paddingPx + caret.bottom - verticalScroll.value
                if (bottom >= 0 && top <= viewportHeightPx) Popup(
                    popupPositionProvider = object : PopupPositionProvider {
                        override fun calculatePosition(anchorBounds: IntRect, windowSize: IntSize, layoutDirection: LayoutDirection, popupContentSize: IntSize): IntOffset {
                            val left = (anchorBounds.left + x.toInt()).coerceIn(0, (windowSize.width - popupContentSize.width).coerceAtLeast(0))
                            val below = anchorBounds.top + bottom.toInt()
                            val y = if (below + popupContentSize.height <= minOf(anchorBounds.bottom, windowSize.height)) below
                                else anchorBounds.top + top.toInt() - popupContentSize.height
                            return IntOffset(left, y.coerceIn(0, (windowSize.height - popupContentSize.height).coerceAtLeast(0)))
                        }
                    },
                    onDismissRequest = { dismissedValue = value },
                    properties = PopupProperties(focusable = false, dismissOnBackPress = false)
                ) {
                    Surface(
                        modifier = Modifier.width(minOf(280.dp, maxWidth - 8.dp)).testTag("editor_assistance"),
                        color = background, contentColor = foreground,
                        border = androidx.compose.foundation.BorderStroke(1.dp, separatorColor), shadowElevation = 4.dp
                    ) {
                        Column {
                            if (completions.isNotEmpty()) LazyColumn(Modifier.heightIn(max = 176.dp).testTag("completion_list"), state = completionScroll) {
                                itemsIndexed(completions, key = { _, item -> item.label }) { index, completion ->
                                    Text(
                                        text = completion.label, fontFamily = FontFamily.Monospace, fontSize = safeFontSize.sp,
                                        modifier = Modifier.fillMaxWidth()
                                            .background(if (index == selectedCompletion) accent.copy(alpha = .18f) else Color.Transparent)
                                            .clickable { acceptCompletion(completion) }
                                            .padding(horizontal = 10.dp, vertical = 10.dp)
                                            .testTag("completion_${completion.label}")
                                    )
                                }
                            }
                            if (callTip != null) Text(callTip, fontFamily = FontFamily.Monospace, fontSize = 12.sp,
                                modifier = Modifier.background(accent.copy(alpha = .08f)).fillMaxWidth().padding(8.dp).testTag("editor_call_tip"))
                        }
                    }
                }
            }
        }
    }
}
