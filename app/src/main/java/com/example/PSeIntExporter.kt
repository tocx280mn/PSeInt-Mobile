package com.example

enum class ExportLanguage(val displayName: String, val extension: String) {
    Python("Python 3", "py"),
    Cpp("C++", "cpp"),
    Java("Java", "java"),
    JavaScript("JavaScript (Node/Browser)", "js"),
    CSharp("C#", "cs"),
    PHP("PHP", "php"),
    C("C", "c")
}

object PSeIntExporter {

    fun exportCode(code: String, targetLanguage: ExportLanguage): String {
        val lines = code.lines().map { it.trim() }
        val sb = StringBuilder()

        when (targetLanguage) {
            ExportLanguage.Python -> exportToPython(lines, sb)
            ExportLanguage.Cpp -> exportToCpp(lines, sb)
            ExportLanguage.Java -> exportToJava(lines, sb)
            ExportLanguage.JavaScript -> exportToJS(lines, sb)
            ExportLanguage.CSharp -> exportToCSharp(lines, sb)
            ExportLanguage.PHP -> exportToPHP(lines, sb)
            ExportLanguage.C -> exportToC(lines, sb)
        }

        return sb.toString()
    }

    private fun exportToPython(lines: List<String>, sb: StringBuilder) {
        sb.append("# Codigo generado por PSeInt Mobile (Exportador a Python)\n\n")
        var indent = ""

        for (line in lines) {
            val lower = line.lowercase()
            when {
                lower.startsWith("algoritmo ") || lower.startsWith("proceso ") -> {
                    val name = line.split(" ").getOrNull(1) ?: "principal"
                    sb.append("def main():\n")
                    indent = "    "
                }
                lower == "finalgoritmo" || lower == "finproceso" -> {
                    sb.append("\nif __name__ == '__main__':\n    main()\n")
                }
                lower.startsWith("escribir ") || lower.startsWith("mostrar ") -> {
                    val content = line.substring(line.indexOf(' ') + 1).replace(";", "").replace("<-", "=")
                    sb.append("${indent}print($content)\n")
                }
                lower.startsWith("leer ") -> {
                    val varName = line.substring(5).replace(";", "").trim()
                    sb.append("${indent}$varName = input()\n")
                }
                lower.startsWith("si ") -> {
                    val cond = line.substring(3, line.lowercase().indexOf("entonces")).trim()
                    val pyCond = cond.replace("=", "==").replace("<==", "<=").replace(">==", ">=")
                    sb.append("${indent}if $pyCond:\n")
                    indent += "    "
                }
                lower == "sino" -> {
                    indent = indent.dropLast(4)
                    sb.append("${indent}else:\n")
                    indent += "    "
                }
                lower == "finsi" || lower == "finmientras" || lower == "finpara" -> {
                    if (indent.length >= 4) indent = indent.dropLast(4)
                }
                lower.startsWith("mientras ") -> {
                    val cond = line.substring(9, line.lowercase().indexOf("hacer")).trim()
                    sb.append("${indent}while $cond:\n")
                    indent += "    "
                }
                line.contains("<-") -> {
                    val parts = line.split("<-")
                    sb.append("${indent}${parts[0].trim()} = ${parts[1].replace(";", "").trim()}\n")
                }
                else -> {
                    if (line.isNotEmpty() && !lower.startsWith("definir ")) {
                        sb.append("${indent}$line\n")
                    }
                }
            }
        }
    }

    private fun exportToCpp(lines: List<String>, sb: StringBuilder) {
        sb.append("// Codigo generado por PSeInt Mobile (Exportador C++)\n")
        sb.append("#include <iostream>\n#include <string>\nusing namespace std;\n\nint main() {\n")

        for (line in lines) {
            val lower = line.lowercase()
            when {
                lower.startsWith("escribir ") -> {
                    val content = line.substring(9).replace(";", "").replace(",", " << ")
                    sb.append("    cout << $content << endl;\n")
                }
                lower.startsWith("leer ") -> {
                    val varName = line.substring(5).replace(";", "").trim()
                    sb.append("    cin >> $varName;\n")
                }
                lower.startsWith("definir ") -> {
                    sb.append("    // $line\n")
                }
                line.contains("<-") -> {
                    val parts = line.split("<-")
                    sb.append("    auto ${parts[0].trim()} = ${parts[1].replace(";", "").trim()};\n")
                }
            }
        }
        sb.append("    return 0;\n}\n")
    }

    private fun exportToJava(lines: List<String>, sb: StringBuilder) {
        sb.append("// Codigo generado por PSeInt Mobile (Exportador Java)\n")
        sb.append("import java.util.Scanner;\n\npublic class Principal {\n    public static void main(String[] args) {\n        Scanner scanner = new Scanner(System.in);\n")

        for (line in lines) {
            val lower = line.lowercase()
            when {
                lower.startsWith("escribir ") -> {
                    val content = line.substring(9).replace(";", "").replace(",", " + ")
                    sb.append("        System.out.println($content);\n")
                }
                lower.startsWith("leer ") -> {
                    val varName = line.substring(5).replace(";", "").trim()
                    sb.append("        String $varName = scanner.nextLine();\n")
                }
                line.contains("<-") -> {
                    val parts = line.split("<-")
                    sb.append("        Object ${parts[0].trim()} = ${parts[1].replace(";", "").trim()};\n")
                }
            }
        }
        sb.append("    }\n}\n")
    }

    private fun exportToJS(lines: List<String>, sb: StringBuilder) {
        sb.append("// Codigo generado por PSeInt Mobile (Exportador JavaScript)\n\n")

        for (line in lines) {
            val lower = line.lowercase()
            when {
                lower.startsWith("escribir ") -> {
                    val content = line.substring(9).replace(";", "")
                    sb.append("console.log($content);\n")
                }
                lower.startsWith("leer ") -> {
                    val varName = line.substring(5).replace(";", "").trim()
                    sb.append("let $varName = prompt(\"Ingrese $varName:\");\n")
                }
                line.contains("<-") -> {
                    val parts = line.split("<-")
                    sb.append("let ${parts[0].trim()} = ${parts[1].replace(";", "").trim()};\n")
                }
            }
        }
    }

    private fun exportToCSharp(lines: List<String>, sb: StringBuilder) {
        sb.append("// Codigo generado por PSeInt Mobile (Exportador C#)\nusing System;\n\nclass Program {\n    static void Main() {\n")
        for (line in lines) {
            val lower = line.lowercase()
            when {
                lower.startsWith("escribir ") -> {
                    val content = line.substring(9).replace(";", "").replace(",", " + ")
                    sb.append("        Console.WriteLine($content);\n")
                }
                lower.startsWith("leer ") -> {
                    val varName = line.substring(5).replace(";", "").trim()
                    sb.append("        string $varName = Console.ReadLine();\n")
                }
                line.contains("<-") -> {
                    val parts = line.split("<-")
                    sb.append("        var ${parts[0].trim()} = ${parts[1].replace(";", "").trim()};\n")
                }
            }
        }
        sb.append("    }\n}\n")
    }

    private fun exportToPHP(lines: List<String>, sb: StringBuilder) {
        sb.append("<?php\n// Codigo generado por PSeInt Mobile (Exportador PHP)\n\n")
        for (line in lines) {
            val lower = line.lowercase()
            when {
                lower.startsWith("escribir ") -> {
                    val content = line.substring(9).replace(";", "")
                    sb.append("echo $content . \"\\n\";\n")
                }
                line.contains("<-") -> {
                    val parts = line.split("<-")
                    sb.append("\$${parts[0].trim()} = ${parts[1].replace(";", "").trim()};\n")
                }
            }
        }
    }

    private fun exportToC(lines: List<String>, sb: StringBuilder) {
        sb.append("/* Codigo generado por PSeInt Mobile (Exportador C) */\n#include <stdio.h>\n\nint main() {\n")
        for (line in lines) {
            val lower = line.lowercase()
            when {
                lower.startsWith("escribir ") -> {
                    val content = line.substring(9).replace(";", "")
                    sb.append("    printf(\"%s\\n\", $content);\n")
                }
            }
        }
        sb.append("    return 0;\n}\n")
    }
}
