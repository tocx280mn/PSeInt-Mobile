package com.example

import kotlinx.coroutines.delay
import kotlin.math.*
import kotlin.random.Random

class PSeIntEvaluator {

    private val variables = mutableMapOf<String, String>()
    private val declaredVariables = mutableSetOf<String>()
    private val arrays = mutableMapOf<String, MutableList<String>>()

    suspend fun evaluate(
        code: String,
        profile: PSeIntProfile = PSeIntProfile.Flexible,
        onOutput: (String) -> Unit,
        onRequestInput: suspend (String) -> String,
        onFinish: () -> Unit
    ) {
        variables.clear()
        declaredVariables.clear()
        arrays.clear()

        val rawLines = code.lines()
        val cleanLines = rawLines.map { line ->
            val commentIdx = line.indexOf("//")
            if (commentIdx != -1) line.substring(0, commentIdx).trim() else line.trim()
        }

        try {
            executeBlock(cleanLines, profile, onOutput, onRequestInput)
        } catch (e: Exception) {
            onOutput("Error de ejecución: ${e.message}")
        } finally {
            onFinish()
        }
    }

    private suspend fun executeBlock(
        lines: List<String>,
        profile: PSeIntProfile,
        onOutput: (String) -> Unit,
        onRequestInput: suspend (String) -> String
    ) {
        var i = 0
        while (i < lines.size) {
            val line = lines[i]
            if (line.isEmpty()) {
                i++
                continue
            }

            val lowerLine = line.lowercase().replace(";", "")

            when {
                lowerLine.startsWith("algoritmo ") || lowerLine.startsWith("proceso ") -> {
                    // Header line
                }
                lowerLine == "finalgoritmo" || lowerLine == "finproceso" -> {
                    // Footer line
                }
                lowerLine.startsWith("definir ") -> {
                    // Definir var1, var2 Como Tipo
                    val parts = line.substring(8).split("como", "Como", "COMO")
                    if (parts.size >= 1) {
                        val varsStr = parts[0]
                        val varNames = varsStr.split(",").map { it.trim() }
                        for (v in varNames) {
                            if (v.isNotEmpty()) {
                                declaredVariables.add(v)
                                variables[v] = "0"
                            }
                        }
                    }
                }
                lowerLine.startsWith("dimension ") || lowerLine.startsWith("dimensión ") -> {
                    // Dimension var[size]
                    val dimStr = line.substring(10).trim()
                    val bracketStart = dimStr.indexOf('[')
                    val bracketEnd = dimStr.indexOf(']')
                    if (bracketStart != -1 && bracketEnd != -1) {
                        val arrName = dimStr.substring(0, bracketStart).trim()
                        val sizeExpr = dimStr.substring(bracketStart + 1, bracketEnd).trim()
                        val size = evaluateExpression(sizeExpr).toIntOrNull() ?: 10
                        arrays[arrName] = MutableList(size) { "0" }
                        declaredVariables.add(arrName)
                    }
                }
                lowerLine.startsWith("escribir ") || lowerLine.startsWith("mostrar ") || lowerLine.startsWith("imprimir ") -> {
                    val keyword = when {
                        lowerLine.startsWith("escribir ") -> "escribir "
                        lowerLine.startsWith("mostrar ") -> "mostrar "
                        else -> "imprimir "
                    }
                    val content = line.substring(keyword.length).replace(";", "").trim()
                    val outStr = formatPrintContent(content)
                    onOutput(outStr)
                }
                lowerLine.startsWith("leer ") -> {
                    val varName = line.substring(5).replace(";", "").trim()
                    if (profile.forceDefineVariables && !declaredVariables.contains(varName)) {
                        onOutput("Error de Sintaxis (Perfil ${profile.name}): La variable '$varName' debe ser definida antes de leerla.")
                        return
                    }
                    val inputVal = onRequestInput(varName)
                    variables[varName] = inputVal
                    declaredVariables.add(varName)
                }
                lowerLine.startsWith("si ") -> {
                    // Si condicion Entonces ... Sino ... FinSi
                    val entoncesIdx = lowerLine.indexOf("entonces")
                    val conditionStr = if (entoncesIdx != -1) {
                        line.substring(3, entoncesIdx).trim()
                    } else {
                        line.substring(3).trim()
                    }

                    val (thenBlock, elseBlock, nextIndex) = extractSiBlocks(lines, i)
                    val condResult = evaluateCondition(conditionStr)

                    if (condResult) {
                        executeBlock(thenBlock, profile, onOutput, onRequestInput)
                    } else if (elseBlock.isNotEmpty()) {
                        executeBlock(elseBlock, profile, onOutput, onRequestInput)
                    }
                    i = nextIndex
                }
                lowerLine.startsWith("mientras ") -> {
                    // Mientras condicion Hacer ... FinMientras
                    val hacerIdx = lowerLine.indexOf("hacer")
                    val conditionStr = if (hacerIdx != -1) {
                        line.substring(9, hacerIdx).trim()
                    } else {
                        line.substring(9).trim()
                    }

                    val (bodyBlock, nextIndex) = extractBlock(lines, i, "mientras", "finmientras")

                    var loopCount = 0
                    while (evaluateCondition(conditionStr) && loopCount < 1000) {
                        executeBlock(bodyBlock, profile, onOutput, onRequestInput)
                        delay(10)
                        loopCount++
                    }
                    i = nextIndex
                }
                lowerLine.startsWith("repetir") -> {
                    val (bodyBlock, nextIndex, conditionStr) = extractRepetirBlock(lines, i)
                    var loopCount = 0
                    do {
                        executeBlock(bodyBlock, profile, onOutput, onRequestInput)
                        delay(10)
                        loopCount++
                    } while (!evaluateCondition(conditionStr) && loopCount < 1000)
                    i = nextIndex
                }
                lowerLine.startsWith("para ") -> {
                    // Para i<-1 Hasta 10 Con Paso 1 Hacer ... FinPara
                    val (headerInfo, bodyBlock, nextIndex) = parseParaHeader(line, lines, i)
                    if (headerInfo != null) {
                        val (varName, startVal, endVal, stepVal) = headerInfo
                        var current = startVal
                        var loopCount = 0
                        while ((stepVal > 0 && current <= endVal) || (stepVal < 0 && current >= endVal)) {
                            if (loopCount > 1000) break
                            variables[varName] = current.toString()
                            declaredVariables.add(varName)
                            executeBlock(bodyBlock, profile, onOutput, onRequestInput)
                            current += stepVal
                            delay(10)
                            loopCount++
                        }
                    }
                    i = nextIndex
                }
                line.contains("<-") || line.contains("=") -> {
                    val sep = if (line.contains("<-")) "<-" else "="
                    val parts = line.split(sep, limit = 2)
                    if (parts.size == 2) {
                        val target = parts[0].trim()
                        val expr = parts[1].replace(";", "").trim()

                        if (profile.forceDefineVariables && !declaredVariables.contains(target) && !target.contains("[")) {
                            onOutput("Error (Perfil ${profile.name}): Variable '$target' no definida.")
                            return
                        }

                        val resultVal = evaluateExpression(expr)

                        if (target.contains("[")) {
                            val arrName = target.substring(0, target.indexOf('[')).trim()
                            val idxExpr = target.substring(target.indexOf('[') + 1, target.indexOf(']')).trim()
                            val idx = evaluateExpression(idxExpr).toIntOrNull() ?: 0
                            val list = arrays[arrName]
                            if (list != null && idx >= 0 && idx < list.size) {
                                list[idx] = resultVal
                            }
                        } else {
                            variables[target] = resultVal
                            declaredVariables.add(target)
                        }
                    }
                }
            }
            delay(15)
            i++
        }
    }

    private fun formatPrintContent(content: String): String {
        val sb = StringBuilder()
        var inQuotes = false
        var currentToken = StringBuilder()
        val tokens = mutableListOf<String>()

        for (char in content) {
            if (char == '"') {
                inQuotes = !inQuotes
                currentToken.append(char)
            } else if (char == ',' && !inQuotes) {
                tokens.add(currentToken.toString().trim())
                currentToken = StringBuilder()
            } else {
                currentToken.append(char)
            }
        }
        if (currentToken.isNotEmpty()) {
            tokens.add(currentToken.toString().trim())
        }

        for (token in tokens) {
            if (token.startsWith("\"") && token.endsWith("\"")) {
                sb.append(token.removeSurrounding("\""))
            } else {
                sb.append(evaluateExpression(token))
            }
        }
        return sb.toString()
    }

    private fun evaluateCondition(conditionStr: String): Boolean {
        val cleanCond = conditionStr.replace(";", "").trim()
        val eqOps = listOf("==", "=", "<>", "!=", "<=", ">=", "<", ">")
        for (op in eqOps) {
            if (cleanCond.contains(op)) {
                val parts = cleanCond.split(op, limit = 2)
                val left = evaluateExpression(parts[0].trim())
                val right = evaluateExpression(parts[1].trim())

                val leftNum = left.toDoubleOrNull()
                val rightNum = right.toDoubleOrNull()

                return if (leftNum != null && rightNum != null) {
                    when (op) {
                        "==", "=" -> leftNum == rightNum
                        "<>", "!=" -> leftNum != rightNum
                        "<=" -> leftNum <= rightNum
                        ">=" -> leftNum >= rightNum
                        "<" -> leftNum < rightNum
                        ">" -> leftNum > rightNum
                        else -> false
                    }
                } else {
                    when (op) {
                        "==", "=" -> left.equals(right, ignoreCase = true)
                        "<>", "!=" -> !left.equals(right, ignoreCase = true)
                        else -> false
                    }
                }
            }
        }
        val num = evaluateExpression(cleanCond).toDoubleOrNull()
        return num != null && num != 0.0
    }

    private fun evaluateExpression(expr: String): String {
        val trimmed = expr.trim()

        if (trimmed.startsWith("\"") && trimmed.endsWith("\"")) {
            return trimmed.removeSurrounding("\"")
        }

        if (variables.containsKey(trimmed)) {
            return variables[trimmed] ?: "0"
        }

        val lower = trimmed.lowercase()
        when {
            lower.startsWith("rc(") || lower.startsWith("raiz(") -> {
                val arg = trimmed.substring(trimmed.indexOf('(') + 1, trimmed.lastIndexOf(')')).trim()
                val valNum = evaluateExpression(arg).toDoubleOrNull() ?: 0.0
                return sqrt(valNum).toString()
            }
            lower.startsWith("abs(") -> {
                val arg = trimmed.substring(trimmed.indexOf('(') + 1, trimmed.lastIndexOf(')')).trim()
                val valNum = evaluateExpression(arg).toDoubleOrNull() ?: 0.0
                return abs(valNum).toString()
            }
            lower.startsWith("trunc(") -> {
                val arg = trimmed.substring(trimmed.indexOf('(') + 1, trimmed.lastIndexOf(')')).trim()
                val valNum = evaluateExpression(arg).toDoubleOrNull() ?: 0.0
                return truncate(valNum).toLong().toString()
            }
            lower.startsWith("redon(") -> {
                val arg = trimmed.substring(trimmed.indexOf('(') + 1, trimmed.lastIndexOf(')')).trim()
                val valNum = evaluateExpression(arg).toDoubleOrNull() ?: 0.0
                return Math.round(valNum).toString()
            }
            lower.startsWith("azar(") || lower.startsWith("aleatorio(") -> {
                val arg = trimmed.substring(trimmed.indexOf('(') + 1, trimmed.lastIndexOf(')')).trim()
                val maxVal = evaluateExpression(arg).toIntOrNull() ?: 100
                return Random.nextInt(0, maxVal).toString()
            }
        }

        // Simple math solver (+, -, *, /)
        val num = trimmed.toDoubleOrNull()
        if (num != null) return if (num % 1.0 == 0.0) num.toLong().toString() else num.toString()

        for (op in listOf("+", "-", "*", "/", "%")) {
            if (trimmed.contains(op)) {
                val lastIdx = trimmed.lastIndexOf(op)
                if (lastIdx > 0 && lastIdx < trimmed.length - 1) {
                    val left = evaluateExpression(trimmed.substring(0, lastIdx)).toDoubleOrNull() ?: 0.0
                    val right = evaluateExpression(trimmed.substring(lastIdx + 1)).toDoubleOrNull() ?: 0.0
                    val res = when (op) {
                        "+" -> left + right
                        "-" -> left - right
                        "*" -> left * right
                        "/" -> if (right != 0.0) left / right else 0.0
                        "%" -> left % right
                        else -> 0.0
                    }
                    return if (res % 1.0 == 0.0) res.toLong().toString() else res.toString()
                }
            }
        }

        return trimmed
    }

    private fun extractSiBlocks(lines: List<String>, startIdx: Int): Triple<List<String>, List<String>, Int> {
        val thenBlock = mutableListOf<String>()
        val elseBlock = mutableListOf<String>()
        var inElse = false
        var depth = 1
        var i = startIdx + 1

        while (i < lines.size) {
            val line = lines[i].trim()
            val lower = line.lowercase()

            if (lower.startsWith("si ")) depth++
            if (lower == "finsi") {
                depth--
                if (depth == 0) return Triple(thenBlock, elseBlock, i)
            }

            if (depth == 1 && lower == "sino") {
                inElse = true
                i++
                continue
            }

            if (inElse) elseBlock.add(line) else thenBlock.add(line)
            i++
        }

        return Triple(thenBlock, elseBlock, i)
    }

    private fun extractBlock(lines: List<String>, startIdx: Int, openKw: String, closeKw: String): Pair<List<String>, Int> {
        val block = mutableListOf<String>()
        var depth = 1
        var i = startIdx + 1

        while (i < lines.size) {
            val line = lines[i].trim()
            val lower = line.lowercase()

            if (lower.startsWith(openKw)) depth++
            if (lower == closeKw) {
                depth--
                if (depth == 0) return Pair(block, i)
            }

            block.add(line)
            i++
        }
        return Pair(block, i)
    }

    private fun extractRepetirBlock(lines: List<String>, startIdx: Int): Triple<List<String>, Int, String> {
        val block = mutableListOf<String>()
        var i = startIdx + 1
        var condStr = "falso"

        while (i < lines.size) {
            val line = lines[i].trim()
            val lower = line.lowercase()

            if (lower.startsWith("hasta que ")) {
                condStr = line.substring("hasta que ".length).trim()
                return Triple(block, i, condStr)
            }
            block.add(line)
            i++
        }
        return Triple(block, i, condStr)
    }

    private fun parseParaHeader(headerLine: String, lines: List<String>, startIdx: Int): Triple<ParaInfo?, List<String>, Int> {
        // Para i<-1 Hasta 10 Con Paso 1 Hacer
        val bodyBlock = mutableListOf<String>()
        var i = startIdx + 1
        var depth = 1

        while (i < lines.size) {
            val line = lines[i].trim()
            val lower = line.lowercase()

            if (lower.startsWith("para ")) depth++
            if (lower == "finpara") {
                depth--
                if (depth == 0) break
            }
            bodyBlock.add(line)
            i++
        }

        try {
            val headerLower = headerLine.lowercase()
            val hastaIdx = headerLower.indexOf("hasta")
            val hacerIdx = headerLower.indexOf("hacer")

            if (hastaIdx != -1) {
                val assignPart = headerLine.substring(4, hastaIdx).trim()
                val sep = if (assignPart.contains("<-")) "<-" else "="
                val parts = assignPart.split(sep)
                val varName = parts[0].trim()
                val startVal = evaluateExpression(parts[1].trim()).toIntOrNull() ?: 1

                var stepVal = 1
                val pasoIdx = headerLower.indexOf("con paso")
                val endPartStr = if (pasoIdx != -1) {
                    val stepPart = headerLine.substring(pasoIdx + 8, if (hacerIdx != -1) hacerIdx else headerLine.length).trim()
                    stepVal = evaluateExpression(stepPart).toIntOrNull() ?: 1
                    headerLine.substring(hastaIdx + 5, pasoIdx).trim()
                } else {
                    headerLine.substring(hastaIdx + 5, if (hacerIdx != -1) hacerIdx else headerLine.length).trim()
                }

                val endVal = evaluateExpression(endPartStr).toIntOrNull() ?: 10
                return Triple(ParaInfo(varName, startVal, endVal, stepVal), bodyBlock, i)
            }
        } catch (e: Exception) {
            // Ignore parse errors
        }

        return Triple(null, bodyBlock, i)
    }

    private data class ParaInfo(val varName: String, val startVal: Int, val endVal: Int, val stepVal: Int)
}
