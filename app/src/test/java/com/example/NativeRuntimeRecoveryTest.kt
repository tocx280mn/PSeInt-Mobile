package com.example

import kotlinx.coroutines.*
import kotlinx.coroutines.test.runTest
import org.junit.Assert.*
import org.junit.Test

class NativeRuntimeRecoveryTest {
    private suspend fun execute(code: String, profile: PSeIntProfile = PSeIntProfile.Flexible): String = buildString {
        PSeIntEvaluator().evaluate(code, profile, onOutput = ::append, onRequestInput = { "" }, onFinish = {})
    }

    @Test fun recursiveFunctionsAndRecoveryFromExcessiveRecursion() = runTest {
        val code = """
            Funcion r <- factorial(n)
                Si n <= 1 Entonces
                    r <- 1
                SiNo
                    r <- n * factorial(n-1)
                FinSi
            FinFuncion
            Algoritmo T
                Escribir factorial(10)
            FinAlgoritmo
        """.trimIndent()
        assertEquals("3628800\n", execute(code))
        assertTrue(execute(code.replace("factorial(10)","factorial(500)")).contains("Error 1004"))
        assertEquals("3628800\n", execute(code))
    }

    @Test fun shrinkingBaseZeroArrayClearsRemovedCells() = runTest {
        val code = """
            Algoritmo T
                Dimension a[3]
                a[0] <- 7
                a[2] <- 9
                Redimensionar a[2]
                Redimensionar a[3]
                Escribir a[0], ",", a[2]
            FinAlgoritmo
        """.trimIndent()
        assertEquals("7,0\n", execute(code,PSeIntProfile.Flexible.copy(base0Arrays=true)))
    }

    @Test fun resizingAnArgumentIsRejectedAsOnDesktop() = runTest {
        val code = """
            SubProceso reducir(a)
                Redimensionar a[2]
            FinSubProceso
            Algoritmo T
                Dimension a[3]
                reducir(a)
            FinAlgoritmo
        """.trimIndent()
        assertEquals("Error 223 en línea 2: No debe redimensionar un argumento.\n", execute(code))
    }

    @Test fun protectedCounterAndInitializationFollowTheProfile() = runTest {
        val code = "Algoritmo T\nPara i <- 1 Hasta 1 Hacer\ni <- 5\nFinPara\nEscribir i\nFinAlgoritmo"
        assertTrue(execute(code).contains("Error 322"))
        assertEquals("6\n", execute(code,PSeIntProfile.Flexible.copy(protectParaCounter=false)))
        val uninitialized = "Algoritmo T\nDefinir n Como Entero\nEscribir n\nFinAlgoritmo"
        assertEquals("0\n",execute(uninitialized))
        assertTrue(execute(uninitialized,PSeIntProfile.Flexible.copy(uninitializedVariables=true)).contains("Error"))
    }

    @Test fun timedWaitCanBeCancelledAndNextRunStillWorks() = runTest {
        val waiting=CompletableDeferred<Unit>()
        val job=launch {
            PSeIntEvaluator().evaluate("Algoritmo T\nEsperar 3600 Segundos\nFinAlgoritmo",PSeIntProfile.Flexible,
                isStepByStep=true, onStep={ line,_,_ -> if(line==2) waiting.complete(Unit) },
                onOutput={}, onRequestInput={""}, onFinish={})
        }
        waiting.await()
        job.cancelAndJoin()
        assertEquals("9\n",execute("Algoritmo T\nEscribir 9\nFinAlgoritmo"))
    }
}
