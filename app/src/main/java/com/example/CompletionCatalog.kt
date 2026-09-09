package com.example

import java.util.Locale

/** Accents are interchangeable in language keywords, but remain significant in user identifiers. */
internal fun String.foldCompletion(): String = lowercase(Locale.ROOT)
    .replace('á', 'a').replace('é', 'e').replace('í', 'i').replace('ó', 'o').replace('ú', 'u').replace('ü', 'u')

internal data class CompletionWord(
    val label: String,
    val insertion: String = "$label ",
    val contexts: Set<String> = emptySet(),
    val expression: Boolean = false,
    val aliases: List<String> = emptyList(),
    val identifier: Boolean = false
) {
    val spellings get() = listOf(label) + aliases
}

internal data class CompletionFunction(val name: String, val arguments: String, val stringFunction: Boolean = false, val aliases: List<String> = emptyList(), val noArguments: Boolean = false)

/** Every name registered by FuncsManager::LoadPredefs, plus the desktop call-tip spellings. */
internal val completionFunctions = listOf(
    CompletionFunction("RC", "expresión numérica no negativa"),
    CompletionFunction("Raiz", "expresión numérica no negativa", aliases = listOf("Raíz")),
    CompletionFunction("Abs", "expresión numérica"), CompletionFunction("Ln", "expresión numérica positiva"),
    CompletionFunction("Exp", "expresión numérica"),
    CompletionFunction("Sen", "ángulo en radianes", aliases = listOf("Sin")),
    CompletionFunction("ASen", "número entre -1 y 1", aliases = listOf("ASin")),
    CompletionFunction("ACos", "número entre -1 y 1"), CompletionFunction("Cos", "ángulo en radianes"),
    CompletionFunction("Tan", "ángulo en radianes"), CompletionFunction("ATan", "expresión numérica"),
    CompletionFunction("Azar", "límite superior (excluido)"), CompletionFunction("Aleatorio", "mínimo, máximo"),
    CompletionFunction("Trunc", "expresión numérica"), CompletionFunction("Redon", "expresión numérica"),
    CompletionFunction("ConvertirANumero", "cadena", true, listOf("ConvertirANúmero")),
    CompletionFunction("ConvertirATexto", "número", true), CompletionFunction("Longitud", "cadena", true),
    CompletionFunction("Subcadena", "cadena, posición inicial, posición final", true),
    CompletionFunction("Mayusculas", "cadena", true, listOf("Mayúsculas")),
    CompletionFunction("Minusculas", "cadena", true, listOf("Minúsculas")),
    CompletionFunction("Concatenar", "cadena, cadena", true),
    CompletionFunction("FechaActual", "sin argumentos; fecha AAAAMMDD", noArguments = true),
    CompletionFunction("HoraActual", "sin argumentos; hora HHMMSS", noArguments = true),
    CompletionFunction("PI", "sin argumentos; constante π", noArguments = true),
    CompletionFunction("Euler", "sin argumentos; constante e", noArguments = true)
)

private val completionTypes = linkedMapOf(
    "Caracter" to listOf("Carácter", "Cadena", "Cadenas", "Texto", "Textos", "Caracteres"),
    "Entero" to listOf("Entera", "Enteros", "Enteras"),
    "Logico" to listOf("Lógico", "Logica", "Lógica", "Logicos", "Lógicos", "Logicas", "Lógicas"),
    "Real" to listOf("Reales", "Número", "Numeros", "Números", "Numerica", "Numérica", "Numericas", "Numéricas", "Numerico", "Numérico", "Numericos", "Numéricos")
)

/** Catalog from mxSource::SetAutocompletion, Keywords.cpp and SynCheck::GetColoquialConditions. */
internal fun completionWords(profile: PSeIntProfile): List<CompletionWord> = buildList {
    fun words(vararg labels: String) { labels.forEach { add(CompletionWord(it)) } }
    fun word(label: String, vararg aliases: String) { add(CompletionWord(label, aliases = aliases.toList())) }
    fun context(contexts: Set<String>, label: String, vararg aliases: String) {
        add(CompletionWord(label, contexts = contexts, aliases = aliases.toList()))
    }
    val subprograms = setOf("funcion", "subproceso", "subalgoritmo")
    // Mostrar, Imprimir e Informar are lazy-syntax aliases in the desktop editor.
    // Keep their contextual continuations out of profiles where the aliases are disabled.
    val output = if (profile.flexibleSyntax) setOf("escribir", "mostrar", "imprimir", "informar") else setOf("escribir")
    val ending = if (profile.requireSemicolons) ";" else " "
    words("Proceso", "FinAlgoritmo", "FinProceso", "Escribir", "Leer", "Definir", "Dimension", "Dimensionar",
        "Si", "Entonces", "Sino", "FinSi", "Mientras", "Hacer", "FinMientras", "Para", "FinPara", "Repetir", "Segun", "FinSegun", "Esperar")
    word("Hasta Que", "HastaQue")
    word("De Otro Modo:", "DeOtroModo", "De Otro Modo")
    word("Esperar Tecla", "EsperarTecla", "Esperar Una Tecla")
    word("Borrar Pantalla", "BorrarPantalla")
    word("Limpiar Pantalla", "LimpiarPantalla")
    if (profile.allowFunctions) {
        word("Algoritmo")
        words("Funcion", "SubProceso", "SubAlgoritmo", "FinFuncion", "FinSubProceso", "FinSubAlgoritmo")
        context(subprograms, "Por Valor", "Por Copia")
        context(subprograms, "Por Referencia")
        if (profile.flexibleSyntax) words("Fin Funcion", "Fin SubProceso", "Fin SubAlgoritmo")
    }
    if (profile.flexibleSyntax) {
        words("Mostrar", "Imprimir", "Informar", "Caso", "Opcion", "Fin Si", "Fin Mientras", "Fin Para", "Fin Segun", "Fin Algoritmo", "Fin Proceso")
        word("Si Es", "SiEs")
        context(setOf("para"), "Desde")
    }
    if (profile.allowArrayResize) { words("Redimensionar"); word("Redimension", "Redimensión") }
    if (profile.enableParaCada) {
        word("Para Cada", "ParaCada")
        context(setOf("para"), "Cada")
        context(setOf("@foreach"), "De", "En")
    }
    if (profile.allowRepetirMientrasQue) word("Mientras Que", "MientrasQue")
    context(setOf("para"), "Hasta")
    context(setOf("para"), "Con Paso", "ConPaso")
    context(setOf("para", "paracada", "mientras", "segun"), "Hacer")
    context(setOf("si"), "Entonces")
    context(output, "Sin Saltar", "Sin Bajar", "SinSaltar", "SinBajar")
    context(setOf("esperar"), "Tecla", "Una Tecla")
    context(setOf("esperar"), "Segundos", "Segundo")
    context(setOf("esperar"), "Milisegundos", "Milisegundo")
    for ((type, aliases) in completionTypes) {
        add(CompletionWord("Como $type", "Como $type$ending", setOf("definir"), aliases = aliases.map { "Como $it" }))
        if (profile.flexibleSyntax) {
            add(CompletionWord("Es $type", "Es $type$ending", setOf("@declaration"), aliases = aliases.map { "Es $it" }))
            val plural = when (type) { "Caracter" -> "Caracteres"; "Real" -> "Reales"; else -> "${type}s" }
            add(CompletionWord("Son $plural", "Son $plural$ending", setOf("@declaration"), aliases = (aliases + type).map { "Son $it" }))
        }
    }
    if (profile.colloquialConditions) {
        fun condition(label: String, vararg aliases: String) = context(setOf("@condition"), label, *aliases)
        condition("Es Cero")
        condition("Es Distinto De", "Es Distinto A", "Es Distinta De", "Es Distinta A")
        condition("Es Divisible Por")
        condition("Es Entero", "Es Entera")
        condition("Es Igual A", "Es Igual Que")
        condition("Es Impar")
        condition("Es Mayor O Igual A", "Es Mayor O Igual Que", "Es Igual O Mayor A", "Es Igual O Mayor Que")
        condition("Es Mayor Que", "Es Mayor A")
        condition("Es Menor O Igual A", "Es Menor O Igual Que", "Es Igual O Menor A", "Es Igual O Menor Que")
        condition("Es Menor Que", "Es Menor A")
        condition("Es Multiplo De", "Es Múltiplo De")
        condition("Es Negativo", "Es Negativa")
        condition("Es Par")
        condition("Es Positivo", "Es Positiva")
        condition("Es")
    }
    completionFunctions.filter { !it.stringFunction || profile.enableStringFunctions }.forEach {
        val insertion = when { it.name in setOf("PI", "Euler") -> it.name; it.noArguments -> "${it.name}()"; else -> "${it.name}(" }
        add(CompletionWord(it.name, insertion, expression = true, aliases = it.aliases))
    }
    listOf("Verdadero", "Falso").forEach { add(CompletionWord(it, it, expression = true)) }
    if (profile.allowWordOperators || profile.colloquialConditions) {
        listOf("Y", "O", "No", "MOD").forEach { add(CompletionWord(it, expression = true)) }
    }
}

/** Includes disabled language features so they cannot leak back in as inferred variable names. */
internal val completionReservedNames: Set<String> by lazy {
    (completionWords(PSeIntProfile.Flexible).flatMap { it.spellings }.filter { word -> word.all { it.isLetter() || it == '_' } } +
        completionTypes.keys + completionTypes.values.flatten() + listOf("Como", "Son"))
        .map { it.foldCompletion() }.toSet()
}

internal fun completionInstructionHelp(command: String, profile: PSeIntProfile): String? = when (command.foldCompletion()) {
    "algoritmo", "proceso" -> "nombre del algoritmo"
    "finalgoritmo", "finproceso" -> "fin del algoritmo"
    "escribir" -> "una o más expresiones, separadas por comas"
    "mostrar", "imprimir", "informar" -> if (profile.flexibleSyntax) "una o más expresiones, separadas por comas" else null
    "leer" -> "una o más variables, separadas por comas"
    "definir", "como" -> "variables Como Entero, Real, Logico o Caracter"
    "dimension", "dimensionar" -> "arreglo[tamaño]; separa las dimensiones con comas"
    "redimension", "redimensionar" -> if (profile.allowArrayResize) "arreglo[nuevo tamaño]" else null
    "si" -> "condición Entonces"
    "entonces" -> "acciones por verdadero"
    "sino" -> "acciones por falso"
    "finsi" -> "fin de la condición"
    "mientras" -> "condición Hacer"
    "finmientras" -> "fin del ciclo Mientras"
    "repetir" -> "acciones; finaliza con Hasta Que condición" + if (profile.allowRepetirMientrasQue) " o Mientras Que condición" else ""
    "hasta", "hastaque", "que" -> "condición, expresión lógica"
    "mientrasque" -> if (profile.allowRepetirMientrasQue) "condición para continuar repitiendo" else null
    "para" -> "variable <- inicio Hasta final [Con Paso paso] Hacer"
    "paracada", "cada" -> if (profile.enableParaCada) "elemento De arreglo Hacer" else null
    "desde" -> if (profile.flexibleSyntax) "valor inicial" else null
    "paso", "conpaso" -> "valor del paso"
    "hacer" -> "acciones del ciclo o de la selección"
    "finpara" -> "fin del ciclo Para"
    "segun" -> if (profile.restrictSegunToNumeric) "expresión numérica entera Hacer" else "expresión de control Hacer"
    "opcion", "sies", "caso" -> if (profile.flexibleSyntax) "posible valor de la expresión de control, seguido de :" else null
    "deotromodo" -> "acciones cuando no coincide ninguna opción"
    "finsegun" -> "fin de la selección"
    "esperar" -> "Tecla o cantidad Segundos / Milisegundos"
    "esperartecla", "tecla" -> "pausa hasta recibir una tecla"
    "segundo", "segundos", "milisegundo", "milisegundos" -> "unidad del tiempo de espera"
    "borrar", "limpiar", "borrarpantalla", "limpiarpantalla" -> "limpia la consola"
    "funcion", "subproceso", "subalgoritmo" -> if (profile.allowFunctions) "[retorno <-] nombre(argumentos)" else null
    "finfuncion", "finsubproceso", "finsubalgoritmo" -> if (profile.allowFunctions) "fin del subprograma" else null
    "por" -> if (profile.allowFunctions) "Valor / Copia o Referencia" else null
    "es", "son" -> if (profile.flexibleSyntax) "tipo de la variable: Entero, Real, Logico o Caracter" else if (profile.colloquialConditions) "comparación o propiedad del valor" else null
    else -> null
}
