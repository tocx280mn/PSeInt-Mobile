package com.example

import android.content.Intent
import android.widget.Toast
import androidx.compose.foundation.Canvas
import androidx.compose.foundation.background
import androidx.compose.foundation.border
import androidx.compose.foundation.layout.*
import androidx.compose.foundation.rememberScrollState
import androidx.compose.foundation.shape.GenericShape
import androidx.compose.foundation.shape.RoundedCornerShape
import androidx.compose.foundation.verticalScroll
import androidx.compose.material.icons.Icons
import androidx.compose.material.icons.filled.Share
import androidx.compose.material3.*
import androidx.compose.runtime.Composable
import androidx.compose.ui.Alignment
import androidx.compose.ui.Modifier
import androidx.compose.ui.draw.clip
import androidx.compose.ui.graphics.Color
import androidx.compose.ui.platform.LocalContext
import androidx.compose.ui.text.font.FontFamily
import androidx.compose.ui.text.font.FontWeight
import androidx.compose.ui.text.style.TextAlign
import androidx.compose.ui.unit.dp
import androidx.compose.ui.unit.sp

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
    val context = LocalContext.current
    val lines = code.lines().map { it.trim() }.filter { it.isNotEmpty() && !it.startsWith("//") }

    Column(
        modifier = Modifier
            .fillMaxSize()
            .background(Color(0xFF222226))
            .verticalScroll(rememberScrollState())
            .padding(16.dp),
        horizontalAlignment = Alignment.CenterHorizontally
    ) {
        // PSDraw Title Bar & Export Button
        Row(
            modifier = Modifier
                .fillMaxWidth()
                .padding(bottom = 14.dp),
            horizontalArrangement = Arrangement.SpaceBetween,
            verticalAlignment = Alignment.CenterVertically
        ) {
            Text(
                text = if (isNassiShneiderman) "PSDraw - Diagrama Nassi-Shneiderman" else "PSDraw - Diagrama de Flujo Clásico",
                color = Color(0xFFFFD54F),
                fontSize = 13.sp,
                fontWeight = FontWeight.Bold,
                fontFamily = FontFamily.Monospace
            )
            Button(
                onClick = {
                    val shareIntent = Intent(Intent.ACTION_SEND).apply {
                        type = "text/plain"
                        putExtra(Intent.EXTRA_SUBJECT, "Diagrama de Flujo PSeInt")
                        putExtra(Intent.EXTRA_TEXT, "Diagrama de Flujo PSeInt:\n\n${lines.joinToString("\n")}")
                    }
                    context.startActivity(Intent.createChooser(shareIntent, "Exportar Diagrama"))
                },
                colors = ButtonDefaults.buttonColors(containerColor = Color(0xFF1976D2)),
                contentPadding = PaddingValues(horizontal = 12.dp, vertical = 4.dp),
                shape = RoundedCornerShape(6.dp)
            ) {
                Icon(Icons.Filled.Share, contentDescription = null, tint = Color.White, modifier = Modifier.size(14.dp))
                Spacer(Modifier.width(4.dp))
                Text("Exportar", fontSize = 11.sp, color = Color.White)
            }
        }

        if (isNassiShneiderman) {
            // Nassi-Shneiderman Container Block (Exact PSeInt Desktop Look)
            Column(
                modifier = Modifier
                    .fillMaxWidth()
                    .border(2.dp, Color(0xFF64B5F6), RoundedCornerShape(4.dp))
                    .background(Color(0xFF1E1E24))
            ) {
                for (line in lines) {
                    val lowerLine = line.lowercase()
                    val bgColor = when {
                        lowerLine.startsWith("algoritmo") || lowerLine.startsWith("proceso") -> Color(0xFF1B5E20)
                        lowerLine.startsWith("finalgoritmo") || lowerLine.startsWith("finproceso") -> Color(0xFFB71C1C)
                        lowerLine.startsWith("si ") || lowerLine.startsWith("mientras ") -> Color(0xFF4A148C)
                        lowerLine.startsWith("leer") -> Color(0xFFAD1457)
                        lowerLine.startsWith("escribir") -> Color(0xFF004D40)
                        else -> Color(0xFF0D47A1)
                    }

                    Box(
                        modifier = Modifier
                            .fillMaxWidth()
                            .border(1.dp, Color(0xFF444454))
                            .background(bgColor.copy(alpha = 0.75f))
                            .padding(12.dp)
                    ) {
                        Text(
                            text = line,
                            color = Color.White,
                            fontFamily = FontFamily.Monospace,
                            fontSize = 13.sp,
                            fontWeight = FontWeight.Bold
                        )
                    }
                }
            }
        } else {
            // PSDraw Classic Flowchart Nodes (Exact Desktop Colors & Shapes)
            for ((index, line) in lines.withIndex()) {
                val lowerLine = line.lowercase()

                val (nodeType, text, bgColor, strokeColor, textColor) = when {
                    lowerLine.startsWith("algoritmo") || lowerLine.startsWith("proceso") -> 
                        Quintuple("StartEnd", line.removePrefix("Algoritmo").removePrefix("Proceso").trim(), Color(0xFF222226), Color(0xFFFFD54F), Color(0xFFFFD54F))
                    lowerLine.startsWith("finalgoritmo") || lowerLine.startsWith("finproceso") -> 
                        Quintuple("StartEnd", "FinAlgoritmo", Color(0xFF222226), Color(0xFFFFD54F), Color(0xFFFFD54F))
                    lowerLine.startsWith("si ") || lowerLine.startsWith("mientras ") || lowerLine.startsWith("repetir") || lowerLine.startsWith("para ") -> 
                        Quintuple("Decision", line, Color(0xFF0D47A1), Color(0xFF64B5F6), Color.White)
                    lowerLine.startsWith("sino") || lowerLine.startsWith("finsi") || lowerLine.startsWith("finmientras") || lowerLine.startsWith("finpara") || lowerLine.startsWith("hasta que") -> 
                        Quintuple("Flow", line, Color.Transparent, Color(0xFF9E9E9E), Color(0xFFA0A0A0))
                    lowerLine.startsWith("leer") -> 
                        Quintuple("Input", line, Color(0xFFAD1457), Color(0xFFF48FB1), Color.White)
                    lowerLine.startsWith("escribir") || lowerLine.startsWith("mostrar") || lowerLine.startsWith("imprimir") -> 
                        Quintuple("Output", line, Color(0xFF004D40), Color(0xFF80CBC4), Color.White)
                    else -> 
                        Quintuple("Process", line, Color(0xFF0D47A1), Color(0xFFFFD54F), Color(0xFFFFD54F))
                }

                DiagramNodeCard(nodeType, text, bgColor, strokeColor, textColor)

                if (index < lines.size - 1) {
                    Box(
                        modifier = Modifier
                            .width(2.dp)
                            .height(22.dp)
                            .background(Color(0xFFE53935))
                    )
                }
            }
        }
    }
}

@Composable
fun DiagramNodeCard(type: String, text: String, bgColor: Color, strokeColor: Color, textColor: Color) {
    val shape = when (type) {
        "StartEnd" -> RoundedCornerShape(24.dp)
        "Input", "Output" -> ParallelogramShape
        "Decision" -> DiamondShape
        else -> RoundedCornerShape(4.dp)
    }

    val paddingV = if (type == "Decision") 14.dp else 10.dp
    val paddingH = if (type == "Input" || type == "Output") 26.dp else 22.dp

    if (type == "Flow") {
        Text(
            text = text,
            color = textColor,
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
                .background(bgColor)
                .border(2.dp, strokeColor, shape)
                .padding(horizontal = paddingH, vertical = paddingV),
            contentAlignment = Alignment.Center
        ) {
            Text(
                text = text,
                color = textColor,
                fontFamily = FontFamily.Monospace,
                fontSize = 13.sp,
                fontWeight = FontWeight.Bold,
                textAlign = TextAlign.Center
            )
        }
    }
}

private data class Quintuple<A, B, C, D, E>(val first: A, val second: B, val third: C, val fourth: D, val fifth: E)
