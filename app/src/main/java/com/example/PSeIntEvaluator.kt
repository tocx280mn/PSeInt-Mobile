package com.example

import androidx.annotation.Keep
import kotlinx.coroutines.*
import java.nio.CharBuffer
import java.nio.charset.CodingErrorAction
import kotlin.coroutines.CoroutineContext

data class SyntaxResult(val isValid: Boolean, val errorMessage: String = "", val errorLine: Int = -1)

/** The same C++ parser, memory model and evaluator shipped with desktop PSeInt. */
class PSeIntEvaluator {
    fun checkSyntax(code: String, profile: PSeIntProfile, checkpoint: () -> Unit = {}): SyntaxResult {
        var firstError: SyntaxResult? = null
        val callbacks = NativeCallbacks(check = checkpoint, onDiagnostic = { line, number, text, warning ->
            if (!warning && firstError == null) firstError = SyntaxResult(false, "Error $number: $text", line)
        })
        return try {
            NativePSeInt.execute(desktopBytes(code), profile.nativeFlags().toByteArray(), false, false, callbacks)
            firstError ?: SyntaxResult(true, "Pseudocódigo correcto")
        } catch (e: CancellationException) { throw e }
        catch (e: Exception) { SyntaxResult(false, e.message ?: "No se pudo comprobar el pseudocódigo") }
        catch (e: LinkageError) { SyntaxResult(false, "No se pudo cargar el motor de PSeInt: ${e.message}") }
    }

    suspend fun evaluate(
        code: String, profile: PSeIntProfile, isStepByStep: Boolean = false,
        onStep: suspend (Int, String, Map<String, String>) -> Unit = { _, _, _ -> },
        onOutput: (String) -> Unit, onRequestInput: suspend (String) -> String, onFinish: () -> Unit
    ) {
        val context = currentCoroutineContext()
        try {
            withContext(Dispatchers.IO) {
                val callbacks = NativeCallbacks(
                    check = { context.ensureActive() },
                    onOutput = onOutput,
                    onDiagnostic = { line, number, text, warning ->
                        onOutput("${if (warning) "Advertencia" else "Error"} $number en línea $line: $text\n")
                    },
                    onInput = { name -> runBlocking(context[Job] ?: EmptyJobContext) { onRequestInput(name) } },
                    onStep = { line, values -> runBlocking(context[Job] ?: EmptyJobContext) {
                        onStep(line, "Línea $line: ${code.lineSequence().elementAtOrNull(line - 1)?.trim().orEmpty()}", values)
                    } }
                )
                NativePSeInt.execute(desktopBytes(code), profile.nativeFlags().toByteArray(), true, isStepByStep, callbacks)
            }
        } catch (e: CancellationException) { throw e }
        catch (e: Exception) { onOutput("Error: ${e.message ?: "No se pudo ejecutar"}\n") }
        catch (e: LinkageError) { onOutput("Error: no se pudo cargar el motor de PSeInt. ${e.message}\n") }
        finally { onFinish() }
    }
}

private val EmptyJobContext: CoroutineContext = kotlin.coroutines.EmptyCoroutineContext
private val desktopCharset = charset("windows-1252")
internal fun desktopBytes(text: String): ByteArray {
    val encoder = desktopCharset.newEncoder().onUnmappableCharacter(CodingErrorAction.REPORT)
        .onMalformedInput(CodingErrorAction.REPORT)
    return try {
        val result = encoder.encode(CharBuffer.wrap(text.replace("\r\n", "\n").replace('\r', '\n')))
        ByteArray(result.remaining()).also { result.get(it) }
    } catch (_: java.nio.charset.CharacterCodingException) {
        throw IllegalArgumentException("El motor de escritorio admite caracteres Windows-1252. Revisa los caracteres del documento; no se han sustituido ni perdido datos.")
    }
}

@Keep
internal object NativePSeInt {
    init {
        val hostLibrary = System.getProperty("pseint.native.library")
        if (hostLibrary == null) System.loadLibrary("pseint")
        else {
            // A separate loader per JVM sandbox; Android always loads its packaged ABI.
            val library = java.io.File(hostLibrary)
            val copy = java.io.File.createTempFile("pseint-host-", ".${library.extension}")
            library.copyTo(copy, overwrite = true)
            copy.deleteOnExit()
            System.load(copy.absolutePath)
        }
    }
    external fun execute(source: ByteArray, flags: ByteArray, run: Boolean, debug: Boolean, callbacks: NativeCallbacks, testMode: Boolean = false): Int
}

@Keep
internal class NativeCallbacks(
    private val check: () -> Unit = {},
    private val onOutput: (String) -> Unit = {},
    private val onInput: (String) -> String? = { "" },
    private val onDiagnostic: (Int, Int, String, Boolean) -> Unit = { _, _, _, _ -> },
    private val onStep: (Int, Map<String, String>) -> Unit = { _, _ -> }
) {
    fun checkpoint() = check()
    fun output(bytes: ByteArray) { check(); onOutput(bytes.toString(desktopCharset)) }
    fun input(bytes: ByteArray): ByteArray? { check(); return onInput(bytes.toString(desktopCharset).lowercase().replace('(', '[').replace(')', ']'))?.let(::desktopBytes) }
    fun diagnostic(line: Int, code: Int, bytes: ByteArray, warning: Boolean) = onDiagnostic(line, code, bytes.toString(desktopCharset), warning)
    fun step(line: Int, values: Array<ByteArray>) {
        check()
        onStep(line, values.toList().chunked(2).associate { it[0].toString(desktopCharset).lowercase() to it[1].toString(desktopCharset) })
    }
}
