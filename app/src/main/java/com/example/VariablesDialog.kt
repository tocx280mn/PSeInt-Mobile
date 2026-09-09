package com.example

import androidx.compose.foundation.layout.*
import androidx.compose.foundation.rememberScrollState
import androidx.compose.foundation.verticalScroll
import androidx.compose.material3.*
import androidx.compose.runtime.*
import androidx.compose.ui.Modifier
import androidx.compose.ui.graphics.Color
import androidx.compose.ui.text.font.FontWeight
import androidx.compose.ui.unit.dp
import androidx.compose.ui.unit.sp

@Composable
fun VariablesListDialog(code: String, onDismiss: () -> Unit) {
    // Simple regex to extract variables from 'Definir' statements
    val variables = remember(code) {
        val vars = mutableListOf<String>()
        val regex = Regex("(?i)Definir\\s+(.+?)\\s+Como\\s+(.+)")
        code.lines().forEach { line ->
            val match = regex.find(line.trim())
            if (match != null) {
                val names = match.groupValues[1]
                val type = match.groupValues[2]
                names.split(",").forEach { name ->
                    vars.add("${name.trim()} : $type")
                }
            }
        }
        vars
    }

    AlertDialog(
        onDismissRequest = onDismiss,
        title = { Text("Índice de Variables", fontWeight = FontWeight.Bold) },
        text = {
            Column(
                modifier = Modifier
                    .fillMaxWidth()
                    .heightIn(max = 300.dp)
                    .verticalScroll(rememberScrollState())
            ) {
                if (variables.isEmpty()) {
                    Text("No se encontraron variables definidas explícitamente.", color = Color.Gray, fontSize = 13.sp)
                } else {
                    variables.forEach { v ->
                        Text("• $v", fontSize = 14.sp, modifier = Modifier.padding(vertical = 4.dp))
                    }
                }
            }
        },
        confirmButton = {
            TextButton(onClick = onDismiss) { Text("Cerrar") }
        }
    )
}

