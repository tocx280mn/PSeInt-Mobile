package com.example

/** Ordered exactly as LS_ENUM in pseint/pseint/LangSettings.h. */
internal val desktopProfileKeys = listOf(
    "force_init_vars", "force_define_vars", "force_semicolon", "allow_concatenation",
    "enable_string_functions", "word_operators", "base_zero_arrays", "allow_dinamyc_dimensions",
    "allow_resize_arrays", "overload_equal", "enable_user_functions", "lazy_syntax",
    "coloquial_conditions", "integer_only_switch", "deduce_negative_for_step", "use_nassi_shneiderman",
    "use_alternative_io_shapes", "allow_accents", "prefer_algoritmo", "prefer_funcion",
    "allow_repeat_while", "allow_for_each", "prefer_repeat_while", "protect_for_counter"
)

fun PSeIntProfile.normalized() = copy(
    allowImplicitVariables = !forceDefineVariables,
    strictTypes = forceDefineVariables,
    allowWordOperators = allowWordOperators || colloquialConditions,
    preferRepetirMientrasQue = preferRepetirMientrasQue && allowRepetirMientrasQue,
    editorFontSize = editorFontSize.coerceIn(10, 24)
)

fun PSeIntProfile.nativeFlags(): String = with(normalized()) {
    listOf(uninitializedVariables, forceDefineVariables, requireSemicolons, allowStringConcatenation,
        enableStringFunctions, allowWordOperators, base0Arrays, dynamicArrays, allowArrayResize,
        allowEqualsAssignment, allowFunctions, flexibleSyntax, colloquialConditions,
        restrictSegunToNumeric, allowOmitStep1, useNassiShneiderman, alternativeIoShapes,
        allowAccentsInVariables, preferAlgoritmo, preferFuncion, allowRepetirMientrasQue,
        enableParaCada, preferRepetirMientrasQue, protectParaCounter).joinToString("") { if (it) "1" else "0" }
}

internal fun PSeIntProfile.withNativeFlags(flags: String): PSeIntProfile {
    require(flags.length == 24 && flags.all { it == '0' || it == '1' }) { "binprofile debe contener 24 bits" }
    fun on(i: Int) = flags[i] == '1'
    return copy(uninitializedVariables=on(0), forceDefineVariables=on(1), requireSemicolons=on(2),
        allowStringConcatenation=on(3), enableStringFunctions=on(4), allowWordOperators=on(5),
        base0Arrays=on(6), dynamicArrays=on(7), allowArrayResize=on(8), allowEqualsAssignment=on(9),
        allowFunctions=on(10), flexibleSyntax=on(11), colloquialConditions=on(12),
        restrictSegunToNumeric=on(13), allowOmitStep1=on(14), useNassiShneiderman=on(15),
        alternativeIoShapes=on(16), allowAccentsInVariables=on(17), preferAlgoritmo=on(18),
        preferFuncion=on(19), allowRepetirMientrasQue=on(20), enableParaCada=on(21),
        preferRepetirMientrasQue=on(22), protectParaCounter=on(23)).normalized()
}

fun PSeIntProfile.toDesktopProfile(): String = buildString {
    appendLine("version=20230211")
    appendLine("name=${name.replace('\n', ' ').replace('\r', ' ')}")
    description.lines().forEach { appendLine("desc=$it") }
    desktopProfileKeys.zip(nativeFlags().toList()).forEach { (key, bit) -> appendLine("$key=$bit") }
    appendLine("editor_font_size=$editorFontSize")
}

fun PSeIntProfile.newProgram(name: String = "SinTitulo"): String =
    if (preferAlgoritmo) "Algoritmo $name\n    \nFinAlgoritmo" else "Proceso $name\n    \nFinProceso"
