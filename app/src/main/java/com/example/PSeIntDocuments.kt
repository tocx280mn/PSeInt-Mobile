package com.example

import android.content.ContentResolver
import android.net.Uri
import android.provider.DocumentsContract
import android.provider.OpenableColumns
import androidx.activity.result.contract.ActivityResultContracts

/** A generic binary MIME avoids a provider appending .txt to the unknown .psc extension. */
internal class CreatePSeIntDocument : ActivityResultContracts.CreateDocument("application/octet-stream") {
    override fun createIntent(context: android.content.Context, input: String): android.content.Intent =
        super.createIntent(context, pscFileName(input)).addCategory(android.content.Intent.CATEGORY_OPENABLE)
}

internal fun isPSeIntFile(name: String?): Boolean =
    name != null && name.length > 4 && name.endsWith(".psc", ignoreCase = true)

internal fun pscFileName(name: String): String {
    val clean = name.substringAfterLast('/').substringAfterLast('\\').trim().ifEmpty { "Algoritmo" }
    if (isPSeIntFile(clean)) return clean
    val withoutTxt = if (clean.endsWith(".txt", true)) clean.dropLast(4) else clean
    return if (isPSeIntFile(withoutTxt)) withoutTxt else withoutTxt.substringBeforeLast('.', withoutTxt).ifEmpty { "Algoritmo" } + ".psc"
}

internal fun documentDisplayName(resolver: ContentResolver, uri: Uri): String? =
    resolver.query(uri, arrayOf(OpenableColumns.DISPLAY_NAME), null, null, null)?.use { cursor ->
        if (cursor.moveToFirst()) cursor.getString(cursor.getColumnIndexOrThrow(OpenableColumns.DISPLAY_NAME)) else null
    }

internal fun readPSeIntDocument(resolver: ContentResolver, uri: Uri): Pair<String, String> {
    val name = documentDisplayName(resolver, uri)
    require(isPSeIntFile(name)) { "Solo se pueden abrir archivos .psc. Selecciona un archivo de PSeInt." }
    val bytes = resolver.openInputStream(uri)?.use { it.readBytes() } ?: error("No se pudo leer el archivo")
    return decodePSeIntDocument(bytes) to requireNotNull(name)
}

/** Check the actual provider name before writing or marking the draft as saved. */
internal fun writePSeIntDocument(resolver: ContentResolver, uri: Uri, text: String, allowRename: Boolean): Pair<Uri, String> {
    var target = uri
    var name = documentDisplayName(resolver, target) ?: error("No se pudo comprobar el nombre del archivo")
    if (!isPSeIntFile(name) && allowRename) {
        target = DocumentsContract.renameDocument(resolver, target, pscFileName(name))
            ?: error("No se pudo conservar la extensión .psc. Usa Guardar como con un nombre terminado en .psc.")
        name = documentDisplayName(resolver, target) ?: error("No se pudo comprobar el nombre del archivo")
    }
    require(isPSeIntFile(name)) { "El archivo debe terminar en .psc. Usa Archivo > Guardar como." }
    (resolver.openOutputStream(target, "wt") ?: error("No se pudo escribir el archivo")).use {
        it.write(text.toByteArray(Charsets.UTF_8))
    }
    return target to name
}
