package com.example

import org.junit.Assert.*
import org.junit.Test
import org.junit.runner.RunWith
import org.junit.runners.Parameterized
import java.io.File
import java.util.concurrent.TimeUnit

@RunWith(Parameterized::class)
class DesktopProfilesTest(private val profileFile: File) {
    companion object {
        @JvmStatic @Parameterized.Parameters(name="{0}")
        fun profiles(): Collection<Array<File>> = File("src/main/assets/profiles").listFiles()!!
            .filter { it.isFile }.sortedBy { it.name }.map { arrayOf(it) }
    }
    @Test fun allFlagsMatchDesktopLoader() {
        val executable = File("build/native-host/pseint-desktop${if (System.getProperty("os.name").orEmpty().startsWith("Windows")) ".exe" else ""}")
        val process = ProcessBuilder(executable.absolutePath,"--profile-flags",profileFile.absolutePath).redirectErrorStream(true).start()
        assertTrue("Profile loader timed out", process.waitFor(5,TimeUnit.SECONDS))
        val expected = process.inputStream.bufferedReader().readText().trim()
        assertEquals(0, process.exitValue())
        val profile = PSeIntProfile.loadFromPrf(profileFile.name, decodePSeIntDocument(profileFile.readBytes()))
        assertEquals(profileFile.name, expected, profile.nativeFlags())
        when(profileFile.name) {
            "Flexible" -> assertEquals(expected, PSeIntProfile.Flexible.nativeFlags())
            "Estricto" -> assertEquals(expected, PSeIntProfile.Estricto.nativeFlags())
        }
    }
}
