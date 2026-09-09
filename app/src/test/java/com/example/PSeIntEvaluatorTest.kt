package com.example

import kotlinx.coroutines.test.runTest
import kotlinx.coroutines.CancellationException
import kotlinx.coroutines.CompletableDeferred
import kotlinx.coroutines.cancelAndJoin
import kotlinx.coroutines.launch
import org.junit.Assert.assertEquals
import org.junit.Assert.assertTrue
import org.junit.Test

/**
 * Tests de fidelidad del intérprete PSeInt frente al comportamiento
 * de PSeInt escritorio. Verifican las correcciones de bugs aplicadas.
 */
class PSeIntEvaluatorTest {

    private suspend fun runCode(code: String, profile: PSeIntProfile = PSeIntProfile.Flexible): List<String> {
        val evaluator = PSeIntEvaluator()
        val outputs = mutableListOf<String>()
        evaluator.evaluate(
            code = code,
            profile = profile,
            isStepByStep = false,
            onOutput = { outputs.add(it) },
            onRequestInput = { "0" },
            onFinish = {}
        )
        return outputs
    }

    @Test
    fun igualdad_logica_igual_Falso_debe_dar_FALSO() = runTest {
        val out = runCode("""
            Algoritmo T
                Escribir Verdadero == Falso
            FinAlgoritmo
        """.trimIndent())
        assertEquals(listOf("FALSO\n").joinToString(""), out.joinToString(""))
    }

    @Test
    fun igualdad_logica_distinto_Falso_debe_dar_VERDADERO() = runTest {
        val out = runCode("""
            Algoritmo T
                Escribir Verdadero != Falso
            FinAlgoritmo
        """.trimIndent())
        assertEquals(listOf("VERDADERO\n").joinToString(""), out.joinToString(""))
    }

    @Test
    fun Para_descendente_deduce_paso_segun_perfil() = runTest {
        val out = runCode("""
            Algoritmo T
                Para i <- 10 Hasta 1 Hacer
                    Escribir i
                FinPara
            FinAlgoritmo
        """.trimIndent())
        // Flexible deduce -1, como LS_DEDUCE_NEGATIVE_FOR_STEP del escritorio.
        assertEquals((10 downTo 1).joinToString("") { "$it\n" }, out.joinToString(""))
    }

    @Test
    fun Para_descendente_con_Con_Paso_menos1_itera() = runTest {
        val out = runCode("""
            Algoritmo T
                Para i <- 3 Hasta 1 Con Paso -1 Hacer
                    Escribir i
                FinPara
            FinAlgoritmo
        """.trimIndent())
        assertEquals(listOf("3\n", "2\n", "1\n").joinToString(""), out.joinToString(""))
    }

    @Test
    fun Para_ascendente_estandar_itera() = runTest {
        val out = runCode("""
            Algoritmo T
                Para i <- 1 Hasta 3 Hacer
                    Escribir i
                FinPara
            FinAlgoritmo
        """.trimIndent())
        assertEquals(listOf("1\n", "2\n", "3\n").joinToString(""), out.joinToString(""))
    }

    @Test
    fun Escribir_con_funcion_y_comas_dentro_de_parentesis() = runTest {
        val out = runCode("""
            Algoritmo T
                Escribir subcadena("hola mundo",1,4)
            FinAlgoritmo
        """.trimIndent())
        assertEquals(listOf("hola\n").joinToString(""), out.joinToString(""))
    }

    @Test
    fun Escribir_con_multiples_argumentos() = runTest {
        val out = runCode("""
            Algoritmo T
                Definir x Como Entero
                x <- 5
                Escribir "val=", x
            FinAlgoritmo
        """.trimIndent())
        assertEquals(listOf("val=5\n").joinToString(""), out.joinToString(""))
    }

    @Test
    fun Segun_con_dos_puntos_dentro_de_cadena() = runTest {
        val out = runCode("""
            Algoritmo T
                Definir x Como Entero
                x <- 1
                Segun x Hacer
                    1:
                        Escribir "hora:12"
                    De Otro Modo:
                        Escribir "otro"
                FinSegun
            FinAlgoritmo
        """.trimIndent())
        assertEquals(listOf("hora:12\n").joinToString(""), out.joinToString(""))
    }

    @Test
    fun subproceso_pasa_escalares_por_valor() = runTest {
        val out = runCode("""
            SubProceso f(x)
                x <- 999
            FinSubProceso
            Algoritmo T
                Definir x Como Entero
                x <- 100
                f(5)
                Escribir x
            FinAlgoritmo
        """.trimIndent())
        // x del llamador no debe modificarse (pass-by-value).
        assertEquals(listOf("100\n").joinToString(""), out.joinToString(""))
    }

    @Test
    fun subproceso_pasa_arreglos_por_referencia() = runTest {
        val out = runCode("""
            SubProceso llenar(a)
                Definir i Como Entero
                Para i <- 1 Hasta 3 Hacer
                    a[i] <- i * 10
                FinPara
            FinSubProceso
            Algoritmo T
                Dimension v[3]
                llenar(v)
                Escribir v[1], v[2], v[3]
            FinAlgoritmo
        """.trimIndent())
        // Las mutaciones al arreglo dentro del subproceso deben verse fuera.
        // PSeInt concatena args de Escribir sin separador.
        assertEquals(listOf("102030\n").joinToString(""), out.joinToString(""))
    }

    @Test
    fun azar_con_argumento_invalido_da_error_claro() = runTest {
        val out = runCode("""
            Algoritmo T
                Escribir azar(0)
            FinAlgoritmo
        """.trimIndent())
        val msg = out.joinToString("")
        assertTrue("Deberia dar error de argumento positivo, obtuvo: $msg", msg.contains("306"))
    }

    @Test
    fun acceso_a_arreglo_fuera_de_rango_da_error() = runTest {
        val out = runCode("""
            Algoritmo T
                Dimension a[5]
                a[100] <- 9
            FinAlgoritmo
        """.trimIndent())
        val msg = out.joinToString("")
        assertTrue("Deberia dar error de indice fuera de rango, obtuvo: $msg", msg.contains("rango"))
    }

    @Test
    fun comparacion_numerica_y_de_cadena() = runTest {
        val out = runCode("""
            Algoritmo T
                Escribir 5 == 5
                Escribir "hola" == "hola"
                Escribir 5 != 6
            FinAlgoritmo
        """.trimIndent())
        assertEquals(listOf("VERDADERO\n", "VERDADERO\n", "VERDADERO\n").joinToString(""), out.joinToString(""))
    }

    @Test
    fun comentarios_respetan_urls_y_separadores_dentro_de_cadenas() = runTest {
        val out = runCode("""
            Algoritmo Cadenas
                Escribir "https://pseint.org/a;b:c", ' // texto' // comentario real
                Escribir "sin saltar" Sin Saltar
                Escribir " fin"
            FinAlgoritmo
        """.trimIndent())
        assertEquals(listOf("https://pseint.org/a;b:c // texto\n", "sin saltar", " fin\n").joinToString(""), out.joinToString(""))
    }

    @Test
    fun instrucciones_con_punto_y_coma_comparten_linea_sin_perder_contenido() = runTest {
        val out = runCode("""
            Algoritmo Varios
                a <- 2; b <- 3; Escribir a + b;
            FinAlgoritmo
        """.trimIndent())
        assertEquals(listOf("5\n").joinToString(""), out.joinToString(""))
    }

    @Test
    fun syntax_rechaza_cadenas_numeros_y_expresiones_invalidas_en_su_linea() {
        val evaluator = PSeIntEvaluator()
        listOf("Escribir \"sin cerrar", "x <- 1.2.3", "Escribir (2 +)", "Leer a[1,]", "Escribir 1,").forEach { instruction ->
            val result = evaluator.checkSyntax("Algoritmo T\n$instruction\nFinAlgoritmo", PSeIntProfile.Flexible)
            assertTrue("Aceptó $instruction", !result.isValid)
            assertEquals(2, result.errorLine)
        }
    }

    @Test
    fun syntax_rechaza_sino_suelto_y_duplicado_y_algoritmos_anidados() {
        val evaluator = PSeIntEvaluator()
        listOf(
            "Algoritmo T\nSino\nFinAlgoritmo",
            "Algoritmo T\nSi Verdadero Entonces\nSino\nSino\nFinSi\nFinAlgoritmo",
            "Algoritmo T\nAlgoritmo Otro\nFinAlgoritmo\nFinAlgoritmo",
            "Algoritmo T\nSi Entonces\nFinSi\nFinAlgoritmo",
            "Algoritmo T\nEscrbir 1\nFinAlgoritmo"
        ).forEach { code -> assertTrue("Aceptó $code", !evaluator.checkSyntax(code, PSeIntProfile.Flexible).isValid) }
    }

    @Test
    fun syntax_comprueba_leer_estricto_y_no_confunde_mayusculas() = runTest {
        val evaluator = PSeIntEvaluator()
        val missing = evaluator.checkSyntax("Algoritmo T\nLeer nombre;\nFinAlgoritmo", PSeIntProfile.Estricto)
        assertTrue(missing.isValid)
        assertTrue(runCode("Algoritmo T\nLeer nombre;\nFinAlgoritmo", PSeIntProfile.Estricto).joinToString("").contains("Error 208 en línea 2"))
        val defined = evaluator.checkSyntax("Algoritmo T\nDefinir Nombre Como Caracter;\nLeer NOMBRE;\nFinAlgoritmo", PSeIntProfile.Estricto)
        assertTrue(defined.errorMessage, defined.isValid)
    }

    @Test
    fun sintaxis_invalida_no_ejecuta_parcialmente_el_programa() = runTest {
        val out = runCode("Algoritmo T\nEscribir 1\nEscrbir 2\nFinAlgoritmo")
        assertEquals(1, out.size)
        assertTrue(out.single(), out.single().contains("línea 3"))
        assertTrue(out.single().contains("Error 106"))
    }

    @Test
    fun segun_anidado_e_instrucciones_dentro_de_etiquetas() = runTest {
        val out = runCode("""
            Algoritmo T
                Segun 1 Hacer
                    1:
                        Segun 2 Hacer
                            1: Escribir "incorrecto"
                            2: Escribir "interior"
                            De Otro Modo: Escribir "incorrecto"
                        FinSegun
                        Escribir "exterior"
                    2: Escribir "incorrecto"
                FinSegun
                Escribir "fin"
            FinAlgoritmo
        """.trimIndent())
        assertEquals(listOf("interior\n", "exterior\n", "fin\n").joinToString(""), out.joinToString(""))
    }

    @Test
    fun repetir_con_cierre_terminado_en_punto_y_coma() = runTest {
        val out = runCode("""
            Algoritmo T
                x <- 0
                Repetir
                    x <- x + 1
                Hasta Que x = 3;
                Escribir x
            FinAlgoritmo
        """.trimIndent())
        assertEquals(listOf("3\n").joinToString(""), out.joinToString(""))
    }

    @Test
    fun leer_matriz_y_texto_conserva_indices_y_ceros_iniciales() = runTest {
        val values = ArrayDeque(listOf("007", "42"))
        val requested = mutableListOf<String>()
        val output = mutableListOf<String>()
        PSeIntEvaluator().evaluate("""
            Algoritmo T
                Definir codigo Como Caracter;
                Definir matriz Como Entero;
                Dimension matriz[2, 2];
                Leer codigo, matriz[0, 1];
                Escribir codigo, ":", matriz[0, 1];
            FinAlgoritmo
        """.trimIndent(), PSeIntProfile.Estricto, onOutput = output::add,
            onRequestInput = { requested.add(it); values.removeFirst() }, onFinish = {})
        assertEquals(listOf("codigo", "matriz[0,1]"), requested)
        assertEquals(listOf("> > 007:42\n").joinToString(""), output.joinToString(""))
    }

    @Test
    fun valores_por_defecto_respetan_tipos_declarados() = runTest {
        val out = runCode("""
            Algoritmo T
                Definir texto Como Caracter
                Definir listo Como Logico
                Escribir "[", texto, "]", listo
            FinAlgoritmo
        """.trimIndent())
        assertEquals(listOf("[]FALSO\n").joinToString(""), out.joinToString(""))
    }

    @Test
    fun argumentos_por_referencia_actualizan_variable_y_celda_del_llamador() = runTest {
        val out = runCode("""
            SubProceso incrementar(valor Por Referencia)
                valor <- valor + 1
            FinSubProceso
            Algoritmo T
                x <- 5
                Dimension a[2]
                a[1] <- 9
                incrementar(x)
                incrementar(a[1])
                Escribir x, ",", a[1]
            FinAlgoritmo
        """.trimIndent())
        assertEquals(listOf("6,10\n").joinToString(""), out.joinToString(""))
    }

    @Test
    fun funcion_con_retorno_y_llamada_sin_parentesis() = runTest {
        val out = runCode("""
            Funcion resultado <- doble(numero)
                resultado <- numero * 2
            FinFuncion
            SubProceso saludar
                Escribir "hola"
            FinSubProceso
            Algoritmo T
                saludar
                Escribir doble(4)
            FinAlgoritmo
        """.trimIndent())
        assertEquals(listOf("hola\n", "8\n").joinToString(""), out.joinToString(""))
    }

    @Test
    fun operaciones_invalidas_producen_errores_con_linea() = runTest {
        listOf("Escribir 1 / 0", "Escribir 1 MOD 0", "Escribir rc(-1)", "Escribir subcadena(\"hola\")", "Escribir ConvertirANumero(\"abc\")").forEach { instruction ->
            val out = runCode("Algoritmo T\n$instruction\nFinAlgoritmo")
            assertEquals(1, out.size)
            assertTrue(out.single(), out.single().startsWith("Error ") && out.single().contains("línea 2"))
        }
    }

    @Test
    fun arreglos_rechazan_dimension_invalida_y_numero_incorrecto_de_indices() = runTest {
        listOf("Dimension a[-1]", "Dimension a[2]\na[1.5] <- 2", "Dimension a[2,2]\nEscribir a[1]").forEach { body ->
            val out = runCode("Algoritmo T\n$body\nFinAlgoritmo")
            assertTrue(out.toString(), out.any { it.startsWith("Error ") })
        }
    }

    @Test
    fun redimensionar_preserva_solo_celdas_dentro_del_nuevo_tamano() = runTest {
        val out = runCode("""
            Algoritmo T
                Dimension a[3]
                a[1] <- 8
                a[3] <- 9
                Redimensionar a[2]
                Redimensionar a[3]
                Escribir a[1], a[3]
            FinAlgoritmo
        """.trimIndent())
        assertEquals(listOf("80\n").joinToString(""), out.joinToString(""))
    }

    @Test
    fun operadores_cortocircuitan_y_negacion_respeta_precedencia_de_escritorio() = runTest {
        val out = runCode("""
            Algoritmo T
                Escribir Falso Y (1 / 0 = 1)
                Escribir Verdadero O (1 / 0 = 1)
                Escribir NO 1 = 2
                Escribir -2 ^ 2
                Escribir "A" = "a"
                Escribir "a" < "b"
            FinAlgoritmo
        """.trimIndent())
        assertEquals(listOf("FALSO\n", "VERDADERO\n", "VERDADERO\n", "-4\n", "FALSO\n", "VERDADERO\n").joinToString(""), out.joinToString(""))
    }

    @Test
    fun aleatorio_usa_dos_limites_inclusivos_y_redon_aleja_empates_del_cero() = runTest {
        val out = runCode("""
            Algoritmo T
                Escribir Aleatorio(7, 7)
                Escribir Redon(-2.5)
            FinAlgoritmo
        """.trimIndent())
        assertEquals(listOf("7\n", "-3\n").joinToString(""), out.joinToString(""))
    }

    @Test
    fun para_con_paso_cero_se_puede_cancelar() = runTest {
        var steps = 0
        var cancelled = false
        try {
            PSeIntEvaluator().evaluate("Algoritmo T\nPara i <- 1 Hasta 3 Con Paso 0 Hacer\nFinPara\nFinAlgoritmo", PSeIntProfile.Flexible,
                isStepByStep = true, onStep = { _, _, _ -> if (++steps == 10) throw CancellationException("Detenido") },
                onOutput = {}, onRequestInput = { "" }, onFinish = {})
        } catch (_: CancellationException) { cancelled = true }
        assertTrue(cancelled)
        assertEquals(10, steps)
    }

    @Test
    fun cancelar_entrada_propaga_cancelacion_y_finaliza_una_vez() = runTest {
        val output = mutableListOf<String>()
        var finishes = 0
        var cancelled = false
        try {
            PSeIntEvaluator().evaluate("Algoritmo T\nLeer dato\nFinAlgoritmo", PSeIntProfile.Flexible,
                onOutput = output::add, onRequestInput = { throw CancellationException("Detenido") }, onFinish = { finishes++ })
        } catch (_: CancellationException) { cancelled = true }
        assertTrue(cancelled)
        assertEquals("> ", output.joinToString(""))
        assertEquals(1, finishes)
    }

    @Test
    fun cancelar_bucle_activo_no_imprime_error_interno() = runTest {
        val firstOutput = CompletableDeferred<Unit>()
        val output = mutableListOf<String>()
        var finishes = 0
        val job = launch {
            PSeIntEvaluator().evaluate("Algoritmo T\nMientras Verdadero Hacer\nEscribir 1\nFinMientras\nFinAlgoritmo", PSeIntProfile.Flexible,
                onOutput = { output.add(it); firstOutput.complete(Unit) }, onRequestInput = { "" }, onFinish = { finishes++ })
        }
        firstOutput.await()
        job.cancelAndJoin()
        assertTrue(output.isNotEmpty()); assertTrue(output.none { it.startsWith("Error") })
        assertEquals(1, finishes)
    }

    @Test
    fun perfil_importado_respeta_inicializacion_y_formas_originales() {
        val profile = PSeIntProfile.loadFromPrf("Prueba", "force_init_vars=1\nallow_resize_arrays=1\nuse_alternative_io_shapes=1")
        assertTrue(profile.uninitializedVariables)
        assertTrue(profile.allowArrayResize)
        assertTrue(profile.alternativeIoShapes)
    }
}
