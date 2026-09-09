package com.example

import android.app.Application
import androidx.compose.runtime.*
import androidx.compose.ui.text.TextRange
import androidx.compose.ui.text.input.TextFieldValue
import androidx.lifecycle.AndroidViewModel
import androidx.lifecycle.viewModelScope
import kotlinx.coroutines.*
import kotlinx.coroutines.channels.Channel

/** Owns the document and execution so rotating the phone cannot discard either. */
class WorkspaceViewModel(application: Application) : AndroidViewModel(application) {
    val prefs = application.getSharedPreferences("pseint_prefs", Application.MODE_PRIVATE)
    var showOnboarding by mutableStateOf(prefs.getBoolean("first_run", true))
        private set
    var editor by mutableStateOf(TextFieldValue(prefs.getString("draft_code", WELCOME_CODE) ?: WELCOME_CODE))
        private set
    var fileName by mutableStateOf(prefs.getString("draft_name", "Bienvenida.psc") ?: "Bienvenida.psc")
        private set
    var fileUri by mutableStateOf(prefs.getString("draft_uri", null))
        private set
    private var savedCode by mutableStateOf(prefs.getString("saved_code", "") ?: "")
    val isModified get() = editor.text != savedCode
    private val undoStack = mutableStateListOf<TextFieldValue>()
    private val redoStack = mutableStateListOf<TextFieldValue>()
    val canUndo get() = undoStack.isNotEmpty()
    val canRedo get() = redoStack.isNotEmpty()
    var tab by mutableStateOf(AppTab.Editor)
    var theme by mutableStateOf(AppTheme.entries.firstOrNull { it.name == prefs.getString("selected_theme", "") } ?: AppTheme.LightClassic)
    var fontSize by mutableIntStateOf(prefs.getInt("font_size", 14).coerceIn(10, 24))
    var lineNumbers by mutableStateOf(prefs.getBoolean("show_line_numbers", true))
    var autocomplete by mutableStateOf(prefs.getBoolean("autocomplete", true))
        private set
    var callTips by mutableStateOf(prefs.getBoolean("call_tips", true))
        private set
    var diagramType by mutableStateOf(DiagramType.entries.firstOrNull { it.name == prefs.getString("diagram_type", "") } ?: DiagramType.Classic)
    var profile by mutableStateOf(PSeIntProfile.loadAllProfiles(application).firstOrNull {
        it.name == prefs.getString("selected_profile", "Flexible")
    } ?: PSeIntProfile.Flexible)
    var syntax by mutableStateOf(SyntaxResult(true, "Comprobando pseudocódigo…"))
        private set
    private var syntaxJob: Job? = null
    var output by mutableStateOf(listOf<String>())
        private set
    var running by mutableStateOf(false)
        private set
    var debugging by mutableStateOf(false)
        private set
    var paused by mutableStateOf(false)
        private set
    var executingLine by mutableIntStateOf(-1)
        private set
    var explanation by mutableStateOf("")
        private set
    var variables by mutableStateOf(mapOf<String, String>())
        private set
    var inputVariable by mutableStateOf<String?>(null)
        private set
    var executionStatus by mutableStateOf("Listo para ejecutar")
        private set
    private var runJob: Job? = null
    private var inputRequest: CompletableDeferred<String>? = null
    private var steps = Channel<Unit>(Channel.CONFLATED)

    init {
        prefs.getString("selected_profile_data", null)?.let { saved ->
            runCatching { PSeIntProfile.importProfileFromString(saved) }.getOrNull()?.let { profile = it.normalized() }
        }
        validate()
    }

    fun edit(value: TextFieldValue) {
        if (value.text != editor.text) {
            undoStack.add(editor.copy(composition = null))
            if (undoStack.size > 100) undoStack.removeAt(0)
            redoStack.clear()
            editor = value
            persistDraft()
            validate()
        } else editor = value
    }

    fun undo() {
        if (canUndo) {
            redoStack.add(editor.copy(composition = null))
            editor = undoStack.removeAt(undoStack.lastIndex)
            persistDraft(); validate()
        }
    }

    fun redo() {
        if (canRedo) {
            undoStack.add(editor.copy(composition = null))
            editor = redoStack.removeAt(redoStack.lastIndex)
            persistDraft(); validate()
        }
    }

    fun openDocument(text: String, name: String, uri: String? = null, saved: Boolean = true) {
        stop()
        editor = TextFieldValue(text.removePrefix("\uFEFF"))
        fileName = name; fileUri = uri
        savedCode = if (saved) editor.text else ""
        undoStack.clear(); redoStack.clear()
        output = emptyList(); variables = emptyMap(); explanation = ""
        tab = AppTab.Editor
        persistDraft(); validate()
    }

    fun markSaved(uri: String, name: String, text: String) {
        fileUri = uri; fileName = name; savedCode = text
        persistDraft()
    }

    private fun persistDraft() {
        prefs.edit().putString("draft_code", editor.text).putString("draft_name", fileName)
            .putString("draft_uri", fileUri).putString("saved_code", savedCode).apply()
    }

    fun selectProfile(value: PSeIntProfile) {
        profile = value.normalized()
        fontSize = profile.editorFontSize
        diagramType = if (profile.useNassiShneiderman) DiagramType.NassiShneiderman else DiagramType.Classic
        prefs.edit().putString("selected_profile", profile.name).putString("selected_profile_data", profile.toDesktopProfile())
            .putInt("font_size", fontSize).putString("diagram_type", diagramType.name).apply()
        validate()
    }

    fun reopenOnboarding() { showOnboarding = true }

    fun setAutocompleteEnabled(enabled: Boolean) {
        autocomplete = enabled
        prefs.edit().putBoolean("autocomplete", enabled).apply()
    }

    fun setCallTipsEnabled(enabled: Boolean) {
        callTips = enabled
        prefs.edit().putBoolean("call_tips", enabled).apply()
    }

    fun finishOnboarding() {
        prefs.edit().putBoolean("first_run", false).apply()
        showOnboarding = false
    }

    fun validate() {
        syntaxJob?.cancel()
        val source = editor.text
        val selected = profile
        syntaxJob = viewModelScope.launch {
            delay(250)
            syntax = withContext(Dispatchers.Default) {
                val context = currentCoroutineContext()
                PSeIntEvaluator().checkSyntax(source, selected) { context.ensureActive() }
            }
        }
    }

    fun goToLine(line: Int) {
        val offset = editor.text.lines().take((line - 1).coerceAtLeast(0)).sumOf { it.length + 1 }.coerceAtMost(editor.text.length)
        editor = editor.copy(selection = TextRange(offset), composition = null)
        tab = AppTab.Editor
    }

    private fun appendOutput(message: String) {
        if (message.contains('\u000C')) output = listOf(message.substringAfterLast('\u000C'))
        else output = (output + message).takeLast(2000)
    }

    fun run(stepByStep: Boolean = false) {
        if (running) return
        val source = editor.text
        val selected = profile
        syntaxJob?.cancel()
        output = emptyList(); variables = emptyMap(); explanation = ""
        running = true; debugging = stepByStep; paused = stepByStep
        executionStatus = if (stepByStep) "Ejecución paso a paso" else "Ejecutando…"
        tab = AppTab.Terminal
        steps.close(); steps = Channel(Channel.CONFLATED)
        val stepChannel = steps
        runJob = viewModelScope.launch {
            val messages = Channel<String>(128)
            val outputJob = launch { for (message in messages) appendOutput(message) }
            try {
                val checked = withContext(Dispatchers.Default) {
                    val context = currentCoroutineContext()
                    PSeIntEvaluator().checkSyntax(source, selected) { context.ensureActive() }
                }
                syntax = checked
                if (!checked.isValid) {
                    executionStatus = "Pseudocódigo con errores"
                    goToLine(checked.errorLine)
                    return@launch
                }
                withContext(Dispatchers.Default) {
                    val executionContext = currentCoroutineContext()
                    PSeIntEvaluator().evaluate(
                        code = source, profile = selected, isStepByStep = stepByStep,
                        onStep = { line, detail, values ->
                            withContext(Dispatchers.Main) {
                                executingLine = line; explanation = detail; variables = values
                            }
                            if (withContext(Dispatchers.Main) { paused }) stepChannel.receive()
                            else delay(350)
                        },
                        onOutput = { message -> runBlocking(executionContext[Job]!!) { messages.send(message) } },
                        onRequestInput = { name ->
                            val request = CompletableDeferred<String>()
                            withContext(Dispatchers.Main) { inputRequest = request; inputVariable = name }
                            try { request.await() } finally {
                                withContext(NonCancellable + Dispatchers.Main) { inputVariable = null; inputRequest = null }
                            }
                        },
                        onFinish = {}
                    )
                }
                messages.close(); outputJob.join()
                executionStatus = if (output.any { it.startsWith("Error", true) }) "Ejecución con errores" else "Ejecución finalizada"
            } catch (_: CancellationException) {
                executionStatus = "Ejecución detenida"
            } catch (e: Exception) {
                appendOutput("Error: ${e.message ?: "No se pudo ejecutar el programa"}")
                executionStatus = "Ejecución con errores"
            } finally {
                messages.close()
                running = false; debugging = false; paused = false; executingLine = -1
                inputRequest?.cancel(); inputRequest = null; inputVariable = null
            }
        }
    }

    fun submitInput(value: String) {
        if (inputRequest?.complete(value) == true) appendOutput("$value\n")
    }

    fun nextStep() { if (running && debugging && paused) steps.trySend(Unit) }
    fun togglePause() { paused = !paused; if (!paused) steps.trySend(Unit) }
    fun stop() { runJob?.cancel() }

    companion object {
        val EMPTY_CODE = "Algoritmo SinTitulo\n    \nFinAlgoritmo"
        val WELCOME_CODE = """Algoritmo Bienvenida
    // Tu primer algoritmo en PSeInt
    Definir nombre Como Caracter;

    Escribir "¿Cómo te llamas?";
    Leer nombre;
    Escribir "Hola, ", nombre;
FinAlgoritmo"""
    }
}
