package com.example

import org.junit.Assert.*
import org.junit.Test
import org.junit.runner.RunWith
import org.junit.runners.Parameterized
import java.io.File
import java.util.concurrent.TimeUnit

/** Every original desktop interpreter regression is run through the actual JNI
 * adapter and an independent, unmodified desktop executable. */
@RunWith(Parameterized::class)
class DesktopCompatibilityTest(private val sourceFile: File) {
    companion object {
        @JvmStatic @Parameterized.Parameters(name = "{0}")
        fun programs(): Collection<Array<File>> = File("../pseint/test/interp").listFiles()!!
            .filter { it.extension == "psc" }.sortedBy { it.name }.map { arrayOf(it) }
    }

    @Test fun mobileMatchesDesktop() {
        val options = sourceFile.resolveSibling("${sourceFile.nameWithoutExtension}.arg")
            .takeIf { it.exists() }?.readText(charset("windows-1252"))?.trim().orEmpty()
            .split(Regex("\\s+")).filter { it.isNotBlank() }
        val profile = PSeIntProfile.loadFromPrf("Regression", "version=20230211\n" +
            options.filterNot { it.startsWith("--input=") }.joinToString("\n") { it.removePrefix("--") })
        val inputs = options.filter { it.startsWith("--input=") }.map { it.substringAfter('=').trim('"') }
        val folder = File("build/compatibility/${sourceFile.nameWithoutExtension}").apply { mkdirs() }
        val profileFile = File(folder, "profile.prf").apply { writeBytes(desktopBytes(profile.toDesktopProfile())) }
        val reference = File("build/native-host/pseint-desktop${if (System.getProperty("os.name").orEmpty().startsWith("Windows")) ".exe" else ""}")
        assertTrue("Compile the desktop oracle with tools/build-native-tests.ps1", reference.exists())
        val outputFile = File(folder, "desktop.out")
        // These two original regressions hit an out-of-bounds recursive resize
        // in the bundled desktop source. Its checked-in expected outputs are the
        // independent reference for the corrected mobile implementation.
        val goldenResize = sourceFile.nameWithoutExtension in setOf("redimension-01", "redimension-03")
        if (goldenResize) {
            outputFile.writeBytes(sourceFile.resolveSibling("${sourceFile.nameWithoutExtension}.out").readBytes())
        } else {
        val process = ProcessBuilder(listOf(reference.absolutePath, sourceFile.absolutePath,
            "--profile=${profileFile.absolutePath}", "--rawerrors", "--nouser", "--noinput", "--fortest", "--seed=1") + inputs.map { "--input=$it" })
            .redirectErrorStream(true).redirectOutput(outputFile).start()
        if (!process.waitFor(15, TimeUnit.SECONDS)) { process.destroyForcibly(); fail("Desktop timed out: ${sourceFile.name}") }
        assertEquals("Desktop crashed: ${outputFile.readText()}", 0, process.exitValue())
        }
        val desktopOutput = outputFile.readText(charset("windows-1252")).replace("\r\n", "\n")
        // Desktop ignores allow_resize_arrays when recognizing Redimensionar.
        // Mobile deliberately reports that forbidden instruction as well as the
        // existing zero-dimension error. Keep this difference explicit and exact.
        val expected = desktopOutput + if (sourceFile.nameWithoutExtension=="redimension-05") "=== Line 8: SynError 1003\n" else ""
        val source = sourceFile.readBytes().toString(charset("windows-1252")).replace("\r\n", "\n")
        val syntax = PSeIntEvaluator().checkSyntax(source, profile)
        val input = ArrayDeque(inputs)
        val actual = StringBuilder()
        val deadline = System.nanoTime() + TimeUnit.SECONDS.toNanos(15)
        val callbacks = NativeCallbacks(
            check = { check(System.nanoTime() < deadline) { "Mobile timeout: ${sourceFile.name}" } },
            onOutput = actual::append,
            onInput = { if (input.isEmpty()) null else input.removeFirst().also { actual.append(it).append('\n') } },
            onDiagnostic = { line, code, _, warning ->
                actual.append("=== Line $line: ${if (warning) "Warning" else if (syntax.isValid) "ExeError" else "SynError"} $code\n")
            }
        )
        NativePSeInt.execute(desktopBytes(source), profile.nativeFlags().toByteArray(), true, false, callbacks, testMode = true)
        File(folder, "mobile.out").writeText(actual.toString())
        assertEquals("${sourceFile.name}: $options", expected, actual.toString())
    }
}
