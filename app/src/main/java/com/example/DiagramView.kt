package com.example

import android.content.Intent
import androidx.compose.foundation.background
import androidx.compose.foundation.border
import androidx.compose.foundation.clickable
import androidx.compose.foundation.horizontalScroll
import androidx.compose.foundation.layout.*
import androidx.compose.foundation.rememberScrollState
import androidx.compose.foundation.shape.GenericShape
import androidx.compose.foundation.shape.RoundedCornerShape
import androidx.compose.foundation.verticalScroll
import androidx.compose.material.icons.Icons
import androidx.compose.material.icons.filled.Add
import androidx.compose.material.icons.filled.Palette
import androidx.compose.material.icons.filled.Share
import androidx.compose.material3.*
import androidx.compose.runtime.*
import androidx.compose.ui.Alignment
import androidx.compose.ui.Modifier
import androidx.compose.ui.draw.clip
import androidx.compose.ui.graphics.Color
import androidx.compose.ui.platform.LocalContext
import androidx.compose.ui.res.painterResource
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
fun DiagramView(
    code: String,
    onCodeChanged: (String) -> Unit = {},
    isNassiShneiderman: Boolean = false
) {
    val context = LocalContext.current
    var isDarkDiagramBg by remember { mutableStateOf(true) }
    val lines = code.lines().map { it.trim() }.filter { it.isNotEmpty() }

    val diagramBg = if (isDarkDiagramBg) Color(0xFF1E1E24) else Color(0xFFF4F4F6)
    val textColorDefault = if (isDarkDiagramBg) Color.White else Color(0xFF111111)

    val insertNodeInDiagram: (String) -> Unit = { nodeText ->
        val clean = code.trim()
        val endIdx = clean.lowercase().lastIndexOf("finalgoritmo")
        if (endIdx != -1) {
            val before = clean.substring(0, endIdx).trimEnd()
            onCodeChanged("$before\n    $nodeText\nFinAlgoritmo")
        } else {
            onCodeChanged("$clean\n$nodeText")
        }
    }

    Row(modifier = Modifier.fillMaxSize()) {
        // Main Diagram Canvas Area
        Column(
            modifier = Modifier
                .weight(1f)
                .fillMaxHeight()
                .background(diagramBg)
                .verticalScroll(rememberScrollState())
                .padding(16.dp),
            horizontalAlignment = Alignment.CenterHorizontally
        ) {
            // PSDraw Title Bar & Controls
            Row(
                modifier = Modifier
                    .fillMaxWidth()
                    .padding(bottom = 12.dp),
                horizontalArrangement = Arrangement.SpaceBetween,
                verticalAlignment = Alignment.CenterVertically
            ) {
                Text(
                    text = if (isNassiShneiderman) "PSDraw - Nassi-Shneiderman" else "PSDraw - Diagrama Clásico",
                    color = if (isDarkDiagramBg) Color(0xFFFFD54F) else Color(0xFF1565C0),
                    fontSize = 13.sp,
                    fontWeight = FontWeight.Bold,
                    fontFamily = FontFamily.Monospace
                )
                Row(horizontalArrangement = Arrangement.spacedBy(6.dp)) {
                    // Toggle Canvas Dark/Light Background
                    IconButton(
                        onClick = { isDarkDiagramBg = !isDarkDiagramBg },
                        modifier = Modifier.size(32.dp).background(Color(0xFF333344), RoundedCornerShape(6.dp))
                    ) {
                        Icon(Icons.Filled.Palette, contentDescription = "Cambiar Fondo", tint = Color.White, modifier = Modifier.size(16.dp))
                    }
                    // Export Diagram Button
                    Button(
                        onClick = {
                            val shareIntent = Intent(Intent.ACTION_SEND).apply {
                                type = "text/plain"
                                putExtra(Intent.EXTRA_SUBJECT, "Diagrama de Flujo PSeInt")
                                putExtra(Intent.EXTRA_TEXT, "Diagrama PSDraw PSeInt:\n\n${lines.joinToString("\n")}")
                            }
                            context.startActivity(Intent.createChooser(shareIntent, "Exportar Diagrama"))
                        },
                        colors = ButtonDefaults.buttonColors(containerColor = Color(0xFF1976D2)),
                        contentPadding = PaddingValues(horizontal = 10.dp, vertical = 4.dp),
                        shape = RoundedCornerShape(6.dp)
                    ) {
                        Icon(Icons.Filled.Share, contentDescription = null, tint = Color.White, modifier = Modifier.size(14.dp))
                        Spacer(Modifier.width(4.dp))
                        Text("Exportar", fontSize = 11.sp, color = Color.White)
                    }
                }
            }

            if (isNassiShneiderman) {
                Column(
                    modifier = Modifier
                        .fillMaxWidth()
                        .border(2.dp, Color(0xFF64B5F6), RoundedCornerShape(4.dp))
                        .background(if (isDarkDiagramBg) Color(0xFF181820) else Color.White)
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
                                .background(bgColor.copy(alpha = 0.8f))
                                .padding(10.dp)
                        ) {
                            Text(
                                text = line,
                                color = Color.White,
                                fontFamily = FontFamily.Monospace,
                                fontSize = 12.sp,
                                fontWeight = FontWeight.Bold
                            )
                        }
                    }
                }
            } else {
                // PSDraw Classic Flowchart Nodes
                for ((index, line) in lines.withIndex()) {
                    val lowerLine = line.lowercase()

                    val (nodeType, text, bgColor, strokeColor, nodeTextColor) = when {
                        lowerLine.startsWith("algoritmo") || lowerLine.startsWith("proceso") -> 
                            Quintuple("StartEnd", line.removePrefix("Algoritmo").removePrefix("Proceso").trim(), if (isDarkDiagramBg) Color(0xFF222226) else Color.White, Color(0xFFFFB300), if (isDarkDiagramBg) Color(0xFFFFD54F) else Color(0xFFE65100))
                        lowerLine.startsWith("finalgoritmo") || lowerLine.startsWith("finproceso") -> 
                            Quintuple("StartEnd", "FinAlgoritmo", if (isDarkDiagramBg) Color(0xFF222226) else Color.White, Color(0xFFFFB300), if (isDarkDiagramBg) Color(0xFFFFD54F) else Color(0xFFE65100))
                        lowerLine.startsWith("si ") || lowerLine.startsWith("mientras ") || lowerLine.startsWith("repetir") || lowerLine.startsWith("para ") -> 
                            Quintuple("Decision", line, Color(0xFF0D47A1), Color(0xFF64B5F6), Color.White)
                        lowerLine.startsWith("sino") || lowerLine.startsWith("finsi") || lowerLine.startsWith("finmientras") || lowerLine.startsWith("finpara") || lowerLine.startsWith("hasta que") -> 
                            Quintuple("Flow", line, Color.Transparent, Color(0xFF9E9E9E), if (isDarkDiagramBg) Color(0xFFA0A0A0) else Color(0xFF666666))
                        lowerLine.startsWith("leer") -> 
                            Quintuple("Input", line, Color(0xFFAD1457), Color(0xFFF48FB1), Color.White)
                        lowerLine.startsWith("escribir") || lowerLine.startsWith("mostrar") || lowerLine.startsWith("imprimir") -> 
                            Quintuple("Output", line, Color(0xFF004D40), Color(0xFF80CBC4), Color.White)
                        else -> 
                            Quintuple("Process", line, Color(0xFF0D47A1), Color(0xFFFFD54F), Color(0xFFFFD54F))
                    }

                    DiagramNodeCard(nodeType, text, bgColor, strokeColor, nodeTextColor)

                    if (index < lines.size - 1) {
                        Box(
                            modifier = Modifier
                                .width(2.dp)
                                .height(20.dp)
                                .background(Color(0xFFE53935))
                        )
                    }
                }
            }
        }

        // Visual Programming Toolbox (Right Panel Matching Screenshot 172905.png)
        Column(
            modifier = Modifier
                .width(96.dp)
                .fillMaxHeight()
                .background(Color(0xFF14141A))
                .border(1.dp, Color(0xFF2A2A38))
                .verticalScroll(rememberScrollState())
                .padding(4.dp),
            horizontalAlignment = Alignment.CenterHorizontally
        ) {
            Text(
                text = "PROGRAMAR\nDIAGRAMA",
                color = Color(0xFFFFD54F),
                fontSize = 9.sp,
                fontWeight = FontWeight.Bold,
                textAlign = TextAlign.Center,
                modifier = Modifier.padding(vertical = 6.dp)
            )

            PSDrawShapeToolButton("Asignar", R.drawable.asignar) { insertNodeInDiagram("variable <- expresion") }
            PSDrawShapeToolButton("Escribir", R.drawable.escribir) { insertNodeInDiagram("Escribir \"\"") }
            PSDrawShapeToolButton("Leer", R.drawable.leer) { insertNodeInDiagram("Leer variable") }
            PSDrawShapeToolButton("Si-Entonces", R.drawable.si) { insertNodeInDiagram("Si condicion Entonces\n\t// acciones\nFinSi") }
            PSDrawShapeToolButton("Según", R.drawable.segun) { insertNodeInDiagram("Segun variable Hacer\n\topcion1:\n\t\t// acciones\nFinSegun") }
            PSDrawShapeToolButton("Mientras", R.drawable.mientras) { insertNodeInDiagram("Mientras condicion Hacer\n\t// acciones\nFinMientras") }
            PSDrawShapeToolButton("Repetir", R.drawable.repetir) { insertNodeInDiagram("Repetir\n\t// acciones\nHasta Que condicion") }
            PSDrawShapeToolButton("Para", R.drawable.para) { insertNodeInDiagram("Para i<-1 Hasta 10 Con Paso 1 Hacer\n\t// acciones\nFinPara") }
        }
    }
}

@Composable
fun PSDrawShapeToolButton(label: String, iconRes: Int, onClick: () -> Unit) {
    Column(
        horizontalAlignment = Alignment.CenterHorizontally,
        verticalArrangement = Arrangement.Center,
        modifier = Modifier
            .fillMaxWidth()
            .padding(vertical = 3.dp)
            .background(Color(0xFF1E1E28), RoundedCornerShape(6.dp))
            .border(1.dp, Color(0xFF3B3B4F), RoundedCornerShape(6.dp))
            .clickable(onClick = onClick)
            .padding(6.dp)
    ) {
        Image(
            painter = painterResource(id = iconRes),
            contentDescription = label,
            modifier = Modifier.size(22.dp)
        )
        Spacer(modifier = Modifier.height(2.dp))
        Text(label, fontSize = 8.sp, fontWeight = FontWeight.Bold, color = Color.White, textAlign = TextAlign.Center)
    }
}

private data class Quintuple<A, B, C, D, E>(val first: A, val second: B, val third: C, val fourth: D, val fifth: E)
