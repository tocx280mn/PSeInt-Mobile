package com.example

data class PSeIntProfile(
    var name: String,
    var description: String,
    var allowImplicitVariables: Boolean = true,
    var requireSemicolons: Boolean = false,
    var allowEqualsAssignment: Boolean = true,
    var forceDefineVariables: Boolean = false,
    var strictTypes: Boolean = false,
    var editorFontSize: Int = 14
) {
    companion object {
        val Flexible = PSeIntProfile(
            name = "Flexible",
            description = "Perfil con sintaxis libre. No exige declarar variables ni usar punto y coma.",
            allowImplicitVariables = true,
            requireSemicolons = false,
            allowEqualsAssignment = true,
            forceDefineVariables = false,
            strictTypes = false
        )

        val Estricto = PSeIntProfile(
            name = "Estricto",
            description = "Perfil estricto. Exige definir variables obligatoriamente y usar asignación <-.",
            allowImplicitVariables = false,
            requireSemicolons = false,
            allowEqualsAssignment = false,
            forceDefineVariables = true,
            strictTypes = true
        )

        val PopularProfiles = listOf(
            Flexible,
            Estricto,
            PSeIntProfile("SENA (Colombia)", "Perfil oficial Servicio Nacional de Aprendizaje SENA.", forceDefineVariables = true),
            PSeIntProfile("UNAM (México)", "Perfil Universidad Nacional Autónoma de México.", forceDefineVariables = true, allowEqualsAssignment = false),
            PSeIntProfile("UTN (Argentina)", "Perfil Universidad Tecnológica Nacional.", forceDefineVariables = false),
            PSeIntProfile("Duoc UC (Chile)", "Perfil oficial Duoc UC Chile.", forceDefineVariables = true),
            PSeIntProfile("TECSUP (Perú)", "Perfil de la institución TECSUP Perú.", forceDefineVariables = true),
            PSeIntProfile("UNI (Perú)", "Perfil Universidad Nacional de Ingeniería.", forceDefineVariables = true, strictTypes = true),
            PSeIntProfile("UDES (Colombia)", "Perfil Universidad de Santander.", forceDefineVariables = true),
            PSeIntProfile("Personalizado", "Perfil 100% configurable con tus propias reglas de sintaxis.")
        )
    }
}
