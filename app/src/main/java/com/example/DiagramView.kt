package com.example

import androidx.compose.foundation.Canvas
import androidx.compose.foundation.background
import androidx.compose.foundation.border
import androidx.compose.foundation.layout.*
import androidx.compose.foundation.rememberScrollState
import androidx.compose.foundation.shape.GenericShape
import androidx.compose.foundation.shape.RoundedCornerShape
import androidx.compose.foundation.verticalScroll
import androidx.compose.material3.Text
import androidx.compose.runtime.Composable
import androidx.compose.ui.Alignment
import androidx.compose.ui.Modifier
import androidx.compose.ui.draw.clip
import androidx.compose.ui.graphics.Color
import androidx.compose.ui.text.font.FontFamily
import androidx.compose.ui.text.font.FontWeight
import androidx.compose.ui.text.style.TextAlign
import androidx.compose.ui.unit.dp
import androidx.compose.ui.unit.sp
import com.example.ui.theme.*

// Parallelogram shape for IO (Leer / Escribir)
val ParallelogramShape = GenericShape { size, _ ->
    val skew = size.width * 0.15f
    moveTo(skew, 0f)
    lineTo(size.width, 0f)
    lineTo(size.width - skew, size.height)
    lineTo(0f, size.height)
    close()
}

// Diamond shape for Decisions (Si / Mientras)
val DiamondShape = GenericShape { size, _ ->
    moveTo(size.width / 2f, 0f)
    lineTo(size.width, size.height / 2f)
    lineTo(size.width / 2f, size.height)
    lineTo(0f, size.height / 2f)
    close()
}

@Composable
fun DiagramView(code: String, isNassiShneiderman: Boolean = false) {
    val lines = code.lines().map { it.trim() }.filter { it.isNotEmpty() && !it.startsWith("//") }

    Column(
        modifier = Modifier
            .fillMaxSize()
            .background(Color(0xFF181820))
            .verticalScroll(rememberScrollState())
            .padding(20.dp),
        horizontalAlignment = Alignment.CenterHorizontally
    ) {
        // Diagram Header Banner
        Text(
            text = if (isNassiShneiderman) "DIAGRAMA NASSI-SHNEIDERMAN" else "DIAGRAMA DE FLUJO CLÁSICO",
            color = Color(0xFF8AB4F8),
            fontSize = 12.sp,
            fontWeight = FontWeight.Bold,
            letterSpacing = 1.sp,
            modifier = Modifier.padding(bottom = 16.dp)
        )

        if (isNassiShneiderman) {
            // Nassi-Shneiderman Container Block
            Column(
                modifier = Modifier
                    .fillMaxWidth()
                    .border(2.dp, Color(0xFF4C8DF6), RoundedCornerShape(4.dp))
                    .background(Color(0xFF22222E))
            ) {
                for (line in lines) {
                    val lowerLine = line.lowercase()
                    val bgColor = when {
                        lowerLine.startsWith("algoritmo") || lowerLine.startsWith("proceso") -> Color(0xFF1B5E20)
                        lowerLine.startsWith("finalgoritmo") || lowerLine.startsWith("finproceso") -> Color(0xFFB71C1C)
                        lowerLine.startsWith("si ") || lowerLine.startsWith("mientras ") -> Color(0xFF4A148C)
                        lowerLine.startsWith("leer") || lowerLine.startsWith("escribir") -> Color(0xFF004D40)
                        else -> Color(0xFF0D47A1)
                    }

                    Box(
                        modifier = Modifier
                            .fillMaxWidth()
                            .border(1.dp, Color(0xFF444454))
                            .background(bgColor.copy(alpha = 0.6f))
                            .padding(12.dp)
                    ) {
                        Text(
                            text = line,
                            color = Color.White,
                            fontFamily = FontFamily.Monospace,
                            fontSize = 13.sp,
                            fontWeight = FontWeight.SemiBold
                        )
                    }
                }
            }
        } else {
            // Classic Flowchart Nodes
            for ((index, line) in lines.withIndex()) {
                val lowerLine = line.lowercase()

                val (nodeType, text, bgColor, strokeColor) = when {
                    lowerLine.startsWith("algoritmo") || lowerLine.startsWith("proceso") -> 
                        Quadruple("StartEnd", line.removePrefix("Algoritmo").removePrefix("Proceso").trim(), Color(0xFF2E7D32), Color(0xFF81C784))
                    lowerLine.startsWith("finalgoritmo") || lowerLine.startsWith("finproceso") -> 
                        Quadruple("StartEnd", "FinAlgoritmo", Color(0xFFC62828), Color(0xFFEF9A9A))
                    lowerLine.startsWith("si ") || lowerLine.startsWith("mientras ") || lowerLine.startsWith("repetir") || lowerLine.startsWith("para ") -> 
                        Quadruple("Decision", line, Color(0xFF7B1FA2), Color(0xFFCE93D8))
                    lowerLine.startsWith("sino") || lowerLine.startsWith("finsi") || lowerLine.startsWith("finmientras") || lowerLine.startsWith("finpara") || lowerLine.startsWith("hasta que") -> 
                        Quadruple("Flow", line, Color.Transparent, Color(0xFF9E9E9E))
                    lowerLine.startsWith("leer") -> 
                        Quadruple("Input", line, Color(0xFFE65100), Color(0xFFFFB74D))
                    lowerLine.startsWith("escribir") || lowerLine.startsWith("mostrar") || lowerLine.startsWith("imprimir") -> 
                        Quadruple("Output", line, Color(0xFF00695C), Color(0xFF80CBC4))
                    else -> 
                        Quadruple("Process", line, Color(0xFF1565C0), Color(0xFF90CAF9))
                }

                DiagramNodeCard(nodeType, text, bgColor, strokeColor)

                if (index < lines.size - 1) {
                    Box(
                        modifier = Modifier
                            .width(2.dp)
                            .height(20.dp)
                            .background(Color(0xFF8AB4F8))
                    )
                }
            }
        }
    }
}

@Composable
fun DiagramNodeCard(type: String, text: String, bgColor: Color, strokeColor: Color) {
    val shape = when (type) {
        "StartEnd" -> RoundedCornerShape(24.dp)
        "Input", "Output" -> ParallelogramShape
        "Decision" -> DiamondShape
        else -> RoundedCornerShape(6.dp)
    }

    val paddingV = if (type == "Decision") 14.dp else 10.dp
    val paddingH = if (type == "Input" || type == "Output") 26.dp else 20.dp

    if (type == "Flow") {
        Text(
            text = text,
            color = Color(0xFFA0A0A0),
            fontFamily = FontFamily.Monospace,
            fontSize = 11.sp,
            fontWeight = FontWeight.Normal,
            textAlign = TextAlign.Center,
            modifier = Modifier.padding(vertical = 2.dp)
        )
    } else {
        Box(
            modifier = Modifier
                .clip(shape)
                .background(bgColor.copy(alpha = 0.85f))
                .border(2.dp, strokeColor, shape)
                .padding(horizontal = paddingH, vertical = paddingV),
            contentAlignment = Alignment.Center
        ) {
            Text(
                text = text,
                color = Color.White,
                fontFamily = FontFamily.Monospace,
                fontSize = 13.sp,
                fontWeight = FontWeight.Bold,
                textAlign = TextAlign.Center
            )
        }
    }
}

private data class Quadruple<A, B, C, D>(val first: A, val second: B, val third: C, val fourth: D)
