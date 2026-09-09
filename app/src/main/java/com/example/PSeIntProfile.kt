package com.example

import android.content.Context

data class PSeIntProfile(
    var name: String,
    var description: String,
    var allowImplicitVariables: Boolean = true,
    var requireSemicolons: Boolean = false,
    var allowEqualsAssignment: Boolean = true,
    var forceDefineVariables: Boolean = false,
    var strictTypes: Boolean = false,
    var editorFontSize: Int = 14,
    // Nuevas configuraciones requeridas
    var uninitializedVariables: Boolean = false,
    var allowStringConcatenation: Boolean = false,
    var enableStringFunctions: Boolean = false,
    var allowWordOperators: Boolean = false,
    var base0Arrays: Boolean = false,
    var dynamicArrays: Boolean = false,
    var allowArrayResize: Boolean = false,
    var allowFunctions: Boolean = false,
    var flexibleSyntax: Boolean = false,
    var colloquialConditions: Boolean = false,
    var restrictSegunToNumeric: Boolean = false,
    var allowOmitStep1: Boolean = false,
    var useNassiShneiderman: Boolean = false,
    var alternativeIoShapes: Boolean = false,
    var allowAccentsInVariables: Boolean = false,
    var preferAlgoritmo: Boolean = false,
    var preferFuncion: Boolean = false,
    var allowRepetirMientrasQue: Boolean = false,
    var enableParaCada: Boolean = false,
    var preferRepetirMientrasQue: Boolean = false,
    var protectParaCounter: Boolean = false
) {
    companion object {
        val Flexible = PSeIntProfile(
            name = "Flexible",
            description = "Perfil con sintaxis relajada. No exige declarar variables ni usar punto y coma.",
            allowImplicitVariables = true,
            requireSemicolons = false,
            allowEqualsAssignment = true,
            forceDefineVariables = false,
            strictTypes = false,
            allowStringConcatenation = true,
            enableStringFunctions = true,
            allowWordOperators = true,
            dynamicArrays = true,
            allowArrayResize = true,
            allowFunctions = true,
            flexibleSyntax = true,
            allowAccentsInVariables = true,
            allowOmitStep1 = true,
            allowRepetirMientrasQue = true,
            colloquialConditions = true,
            preferAlgoritmo = true,
            preferFuncion = true,
            enableParaCada = true,
            protectParaCounter = true
        )

        val Estricto = PSeIntProfile(
            name = "Estricto",
            description = "Exige declarar e inicializar variables, usar ; y asignar con <-. Arreglos en base 0.",
            allowImplicitVariables = false,
            requireSemicolons = true,
            allowEqualsAssignment = false,
            forceDefineVariables = true,
            strictTypes = true,
            uninitializedVariables = true,
            base0Arrays = true,
            enableStringFunctions = true,
            allowWordOperators = true,
            allowArrayResize = true,
            allowFunctions = true,
            restrictSegunToNumeric = true,
            protectParaCounter = true
        )

        fun loadAllProfiles(context: Context): List<PSeIntProfile> {
            val list = mutableListOf<PSeIntProfile>()
            list.add(Flexible)
            list.add(Estricto)
            try {
                val files = context.assets.list("profiles") ?: emptyArray()
                for (fileName in files) {
                    if (fileName in setOf("icons", "Flexible", "Estricto", "Personalizado")) continue
                    runCatching {
                        val bytes = context.assets.open("profiles/$fileName").use { it.readBytes() }
                        list.add(loadFromPrf(fileName, decodePSeIntDocument(bytes)))
                    }
                }
            } catch (e: Exception) {
                e.printStackTrace()
            }
            // Añadir el perfil Personalizado del usuario (cargado desde prefs) para que aparezca en el selector
            try {
                val custom = loadCustomProfile(context)
                list.add(custom.copy(description = if (custom.description.isNotEmpty()) custom.description else "Perfil personalizado por el usuario"))
            } catch (e: Exception) {
                e.printStackTrace()
                list.add(PSeIntProfile(name = "Personalizado", description = "Perfil personalizado por el usuario"))
            }
            return list.distinctBy { it.name.lowercase() }.sortedBy { it.name.lowercase() }
        }

        fun saveCustomProfile(context: Context, profile: PSeIntProfile) {
            val prefs = context.getSharedPreferences("custom_profile_prefs", Context.MODE_PRIVATE)
            prefs.edit().apply {
                putBoolean("forceDefineVariables", profile.forceDefineVariables)
                putBoolean("allowEqualsAssignment", profile.allowEqualsAssignment)
                putBoolean("requireSemicolons", profile.requireSemicolons)
                putBoolean("strictTypes", profile.strictTypes)
                putBoolean("allowImplicitVariables", profile.allowImplicitVariables)
                putInt("editorFontSize", profile.editorFontSize)
                putBoolean("uninitializedVariables", profile.uninitializedVariables)
                putBoolean("allowStringConcatenation", profile.allowStringConcatenation)
                putBoolean("enableStringFunctions", profile.enableStringFunctions)
                putBoolean("allowWordOperators", profile.allowWordOperators)
                putBoolean("base0Arrays", profile.base0Arrays)
                putBoolean("dynamicArrays", profile.dynamicArrays)
                putBoolean("allowArrayResize", profile.allowArrayResize)
                putBoolean("allowFunctions", profile.allowFunctions)
                putBoolean("flexibleSyntax", profile.flexibleSyntax)
                putBoolean("colloquialConditions", profile.colloquialConditions)
                putBoolean("restrictSegunToNumeric", profile.restrictSegunToNumeric)
                putBoolean("allowOmitStep1", profile.allowOmitStep1)
                putBoolean("useNassiShneiderman", profile.useNassiShneiderman)
                putBoolean("alternativeIoShapes", profile.alternativeIoShapes)
                putBoolean("allowAccentsInVariables", profile.allowAccentsInVariables)
                putBoolean("preferAlgoritmo", profile.preferAlgoritmo)
                putBoolean("preferFuncion", profile.preferFuncion)
                putBoolean("allowRepetirMientrasQue", profile.allowRepetirMientrasQue)
                putBoolean("enableParaCada", profile.enableParaCada)
                putBoolean("preferRepetirMientrasQue", profile.preferRepetirMientrasQue)
                putBoolean("protectParaCounter", profile.protectParaCounter)
                apply()
            }
        }

        fun loadCustomProfile(context: Context): PSeIntProfile {
            val prefs = context.getSharedPreferences("custom_profile_prefs", Context.MODE_PRIVATE)
            return PSeIntProfile(
                name = "Personalizado",
                description = "Perfil con reglas de sintaxis personalizadas por el usuario.",
                forceDefineVariables = prefs.getBoolean("forceDefineVariables", true),
                allowEqualsAssignment = prefs.getBoolean("allowEqualsAssignment", false),
                requireSemicolons = prefs.getBoolean("requireSemicolons", false),
                strictTypes = prefs.getBoolean("strictTypes", false),
                allowImplicitVariables = prefs.getBoolean("allowImplicitVariables", true),
                editorFontSize = prefs.getInt("editorFontSize", 14),
                uninitializedVariables = prefs.getBoolean("uninitializedVariables", false),
                allowStringConcatenation = prefs.getBoolean("allowStringConcatenation", false),
                enableStringFunctions = prefs.getBoolean("enableStringFunctions", false),
                allowWordOperators = prefs.getBoolean("allowWordOperators", false),
                base0Arrays = prefs.getBoolean("base0Arrays", false),
                dynamicArrays = prefs.getBoolean("dynamicArrays", false),
                allowArrayResize = prefs.getBoolean("allowArrayResize", false),
                allowFunctions = prefs.getBoolean("allowFunctions", false),
                flexibleSyntax = prefs.getBoolean("flexibleSyntax", false),
                colloquialConditions = prefs.getBoolean("colloquialConditions", false),
                restrictSegunToNumeric = prefs.getBoolean("restrictSegunToNumeric", false),
                allowOmitStep1 = prefs.getBoolean("allowOmitStep1", false),
                useNassiShneiderman = prefs.getBoolean("useNassiShneiderman", false),
                alternativeIoShapes = prefs.getBoolean("alternativeIoShapes", false),
                allowAccentsInVariables = prefs.getBoolean("allowAccentsInVariables", false),
                preferAlgoritmo = prefs.getBoolean("preferAlgoritmo", false),
                preferFuncion = prefs.getBoolean("preferFuncion", false),
                allowRepetirMientrasQue = prefs.getBoolean("allowRepetirMientrasQue", false),
                enableParaCada = prefs.getBoolean("enableParaCada", false),
                preferRepetirMientrasQue = prefs.getBoolean("preferRepetirMientrasQue", false),
                protectParaCounter = prefs.getBoolean("protectParaCounter", false)
            )
        }

        fun exportProfileToString(profile: PSeIntProfile): String {
            return profile.toDesktopProfile()
        }

        fun importProfileFromString(data: String): PSeIntProfile {
            if (data.lineSequence().any { it.substringBefore('=').trim() in desktopProfileKeys || it.startsWith("version=") || it.startsWith("binprofile=") }) {
                val name = data.lineSequence().firstOrNull { it.startsWith("name=") }?.substringAfter('=') ?: "Personalizado Importado"
                return loadFromPrf(name, data)
            }
            val lines = data.lines()
            val map = lines.filter { it.contains("=") }.associate {
                val parts = it.split("=", limit = 2)
                parts[0].trim() to parts[1].trim()
            }
            require(map.keys.any { it in setOf("forceDefineVariables", "allowEqualsAssignment", "requireSemicolons", "allowImplicitVariables", "allowFunctions") }) {
                "El archivo no contiene un perfil de PSeInt reconocido."
            }
            return PSeIntProfile(
                name = map["name"] ?: "Personalizado Importado",
                description = "Perfil importado",
                forceDefineVariables = map["forceDefineVariables"].toBoolean(),
                allowEqualsAssignment = map["allowEqualsAssignment"].toBoolean(),
                requireSemicolons = map["requireSemicolons"].toBoolean(),
                strictTypes = map["strictTypes"].toBoolean(),
                allowImplicitVariables = map["allowImplicitVariables"].toBoolean(),
                uninitializedVariables = map["uninitializedVariables"].toBoolean(),
                allowStringConcatenation = map["allowStringConcatenation"].toBoolean(),
                enableStringFunctions = map["enableStringFunctions"].toBoolean(),
                allowWordOperators = map["allowWordOperators"].toBoolean(),
                base0Arrays = map["base0Arrays"].toBoolean(),
                dynamicArrays = map["dynamicArrays"].toBoolean(),
                allowArrayResize = map["allowArrayResize"].toBoolean(),
                allowFunctions = map["allowFunctions"].toBoolean(),
                flexibleSyntax = map["flexibleSyntax"].toBoolean(),
                colloquialConditions = map["colloquialConditions"].toBoolean(),
                restrictSegunToNumeric = map["restrictSegunToNumeric"].toBoolean(),
                allowOmitStep1 = map["allowOmitStep1"].toBoolean(),
                useNassiShneiderman = map["useNassiShneiderman"].toBoolean(),
                alternativeIoShapes = map["alternativeIoShapes"].toBoolean(),
                allowAccentsInVariables = map["allowAccentsInVariables"].toBoolean(),
                preferAlgoritmo = map["preferAlgoritmo"].toBoolean(),
                preferFuncion = map["preferFuncion"].toBoolean(),
                allowRepetirMientrasQue = map["allowRepetirMientrasQue"].toBoolean(),
                enableParaCada = map["enableParaCada"].toBoolean(),
                preferRepetirMientrasQue = map["preferRepetirMientrasQue"].toBoolean(),
                protectParaCounter = map["protectParaCounter"].toBoolean()
            )
        }

        fun loadFromPrf(profileName: String, data: String): PSeIntProfile {
            val lines = data.lines()
            val map = mutableMapOf<String, String>()
            val descLines = mutableListOf<String>()

            for (line in lines) {
                val t = line.trim()
                if (t.startsWith("#") || t.isEmpty()) continue
                if (t.startsWith("desc=")) {
                    descLines.add(t.substring(5).trim())
                } else if (t.contains("=")) {
                    val parts = t.split("=", limit = 2)
                    map[parts[0].trim()] = parts[1].trim()
                }
            }

            val version = map["version"]?.toIntOrNull() ?: 0
            // LangSettings::Reset(0), ProcessConfigLine and Fix, including old profiles.
            fun setting(key: String, default: Boolean = false): Boolean = map[key]?.firstOrNull()?.lowercaseChar()?.let { it in "1vst" } ?: default
            val lazy = setting("lazy_syntax", true)
            val colloquial = setting("coloquial_conditions", true)
            val repeat = if (version < 20210407) lazy else setting("allow_repeat_while", true)

            return PSeIntProfile(
                name = profileName,
                description = if (descLines.isNotEmpty()) descLines.joinToString("\n") else "Perfil original PSeInt",
                allowImplicitVariables = !setting("force_define_vars"),
                requireSemicolons = setting("force_semicolon"),
                allowEqualsAssignment = setting("overload_equal"),
                forceDefineVariables = setting("force_define_vars"),
                strictTypes = setting("force_define_vars"),
                editorFontSize = (map["editor_font_size"]?.toIntOrNull() ?: 14).coerceIn(10, 24),
                uninitializedVariables = setting("force_init_vars"),
                allowStringConcatenation = setting("allow_concatenation", true),
                enableStringFunctions = setting("enable_string_functions", true),
                allowWordOperators = colloquial || setting("word_operators", true),
                base0Arrays = setting("base_zero_arrays"),
                dynamicArrays = setting("allow_dinamyc_dimensions", true),
                allowArrayResize = setting("allow_resize_arrays", true),
                allowFunctions = setting("enable_user_functions", true),
                flexibleSyntax = lazy,
                colloquialConditions = colloquial,
                restrictSegunToNumeric = if (version < 20150304) !lazy else setting("integer_only_switch"),
                allowOmitStep1 = if (version < 20150304) lazy else setting("deduce_negative_for_step", true),
                useNassiShneiderman = setting("use_nassi_shneiderman", setting("use_nassi_schneiderman")),
                alternativeIoShapes = setting("use_alternative_io_shapes"),
                allowAccentsInVariables = if (version < 20160321) lazy else setting("allow_accents", true),
                preferAlgoritmo = version >= 20160321 && setting("prefer_algoritmo", true),
                preferFuncion = version >= 20160321 && setting("prefer_funcion", true),
                allowRepetirMientrasQue = repeat,
                enableParaCada = if (version < 20210609) lazy else setting("allow_for_each", true),
                preferRepetirMientrasQue = repeat && setting("prefer_repeat_while"),
                protectParaCounter = setting("protect_for_counter", true)
            ).let { profile -> map["binprofile"]?.takeIf { it.length == 24 }?.let { profile.withNativeFlags(it) } ?: profile }
        }
    }
}
