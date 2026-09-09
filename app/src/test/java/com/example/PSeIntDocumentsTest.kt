package com.example

import android.app.Application
import android.content.ContentProvider
import android.content.ContentValues
import android.content.Intent
import android.database.MatrixCursor
import android.net.Uri
import android.os.Bundle
import android.os.ParcelFileDescriptor
import android.provider.OpenableColumns
import androidx.test.core.app.ApplicationProvider
import org.junit.Assert.*
import org.junit.Before
import org.junit.Test
import org.junit.runner.RunWith
import org.robolectric.RobolectricTestRunner
import org.robolectric.annotation.Config
import org.robolectric.shadows.ShadowContentResolver
import java.io.File

@RunWith(RobolectricTestRunner::class)
@Config(sdk = [36])
class PSeIntDocumentsTest {
    private val app get() = ApplicationProvider.getApplicationContext<Application>()
    private val uri = Uri.parse("content://psc-test/document/code")
    private lateinit var provider: TestProvider
    @Before fun setup() {
        provider = TestProvider(File(app.cacheDir, "document-test.psc"))
        provider.attachInfo(app, android.content.pm.ProviderInfo().apply { authority = "psc-test" })
        ShadowContentResolver.registerProviderInternal("psc-test", provider)
    }
    @Test fun createContractPreservesPscInsteadOfRequestingText() {
        val intent = CreatePSeIntDocument().createIntent(app, pscFileName("Programa.psc.txt"))
        assertEquals(Intent.ACTION_CREATE_DOCUMENT, intent.action)
        assertEquals("application/octet-stream", intent.type)
        assertEquals("Programa.psc", intent.getStringExtra(Intent.EXTRA_TITLE))
        assertTrue(intent.hasCategory(Intent.CATEGORY_OPENABLE))
    }
    @Test fun validatesFinalExtensionAndNormalizesLegacySaveNames() {
        listOf("Algoritmo.psc", "Mi programa.PSC", "ejercicio.v2.psc").forEach { assertTrue(isPSeIntFile(it)) }
        listOf(null, "", ".psc", "a.psc.txt", "a.txt", "psc", "a.psc ").forEach { assertFalse(isPSeIntFile(it)) }
        assertEquals("Programa.psc", pscFileName("Programa.psc.txt"))
        assertEquals("Programa.psc", pscFileName("Programa.txt"))
        assertEquals("Algoritmo.psc", pscFileName(""))
    }
    @Test fun rejectsOtherFilesBeforeReadingEvenWithTextMime() {
        provider.name = "documento.psc.txt"
        provider.file.writeText("Escribir 1")
        val error = runCatching { readPSeIntDocument(app.contentResolver, uri) }.exceptionOrNull()
        assertTrue(error is IllegalArgumentException)
        assertTrue(error!!.message!!.contains(".psc"))
        assertEquals(0, provider.opens)
        provider.name = "documento.PSC"
        assertEquals("Escribir 1" to "documento.PSC", readPSeIntDocument(app.contentResolver, uri))
    }
    @Test fun saveTruncatesAndRoundTripsUnicodeWithPscName() {
        provider.file.writeText("contenido viejo mucho más largo que el nuevo")
        val source = "Escribir \"niño\""
        val saved = writePSeIntDocument(app.contentResolver, uri, source, allowRename = false)
        assertEquals(uri to "Programa.psc", saved)
        assertEquals(source, provider.file.readText())
        assertEquals(source to "Programa.psc", readPSeIntDocument(app.contentResolver, saved.first))
    }
    @Test fun providerAddedTxtIsRenamedBeforeWritingUsingReturnedUri() {
        provider.name = "Programa.psc.txt"
        val saved = writePSeIntDocument(app.contentResolver, uri, "Escribir 2", allowRename = true)
        assertEquals("Programa.psc", saved.second)
        assertEquals(provider.renamedUri, saved.first)
        assertEquals(provider.renamedUri, provider.lastOpened)
        assertEquals("Escribir 2", provider.file.readText())
    }
    @Test fun failedRenameOrInvalidExistingFileNeverOverwritesContent() {
        provider.name = "Programa.txt"
        provider.file.writeText("original")
        assertTrue(runCatching { writePSeIntDocument(app.contentResolver, uri, "nuevo", allowRename = false) }.isFailure)
        provider.renameSupported = false
        assertTrue(runCatching { writePSeIntDocument(app.contentResolver, uri, "nuevo", allowRename = true) }.isFailure)
        assertEquals("original", provider.file.readText())
        assertEquals(0, provider.opens)
    }

    private class TestProvider(val file: File) : ContentProvider() {
        var name = "Programa.psc"
        var opens = 0
        var renameSupported = true
        var lastOpened: Uri? = null
        val renamedUri = Uri.parse("content://psc-test/document/renamed")
        override fun onCreate() = true
        override fun getType(uri: Uri) = "text/plain"
        override fun query(uri: Uri, projection: Array<out String>?, selection: String?, selectionArgs: Array<out String>?, sortOrder: String?) =
            MatrixCursor(arrayOf(OpenableColumns.DISPLAY_NAME)).apply { addRow(arrayOf(name)) }
        override fun openFile(uri: Uri, mode: String): ParcelFileDescriptor {
            opens++; lastOpened = uri
            return ParcelFileDescriptor.open(file, ParcelFileDescriptor.parseMode(mode))
        }
        override fun call(method: String, arg: String?, extras: Bundle?): Bundle {
            check(renameSupported) { "Proveedor sin soporte de renombrado" }
            name = requireNotNull(extras?.getString(OpenableColumns.DISPLAY_NAME))
            return Bundle().apply { putParcelable("uri", renamedUri) }
        }
        override fun insert(uri: Uri, values: ContentValues?): Uri? = null
        override fun delete(uri: Uri, selection: String?, selectionArgs: Array<out String>?) = 0
        override fun update(uri: Uri, values: ContentValues?, selection: String?, selectionArgs: Array<out String>?) = 0
    }
}
