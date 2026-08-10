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
    var editorFontSize: Int = 14
) {
    companion object {
        val Flexible = PSeIntProfile(
            name = "Flexible",
            description = "Perfil con sintaxis relajada. No exige declarar variables ni usar punto y coma.",
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

        val PopularNames = listOf(
            "Flexible", "Estricto", "Personalizado",
            "111Mil", "AIEP", "Agustiniano", "Areandina", "Avansys", "Babar", "BeehiveSchool",
            "Bethlemitas", "BiffiLaSalle", "CATCE", "CBM", "CBTA09", "CBTis118", "CBTis155",
            "CBTis45", "CCS", "CECEP", "CECEYTE23-Tocumbo", "CECYTEO-Pl1", "CECyTEA", "CECyTEM",
            "CECyTEMichoacan", "CEDUC", "CEL", "CELPO", "CENSA-Avanzado", "CENSA-Basico",
            "CESCA-HNTG", "CESCA-JKDRC", "CETIS50", "CETYS", "CETis156", "CETis42", "CEVaP",
            "CFGS-DAW", "CFT-Valparaiso", "CIAF", "CIISA", "CLRM", "CNB-Saltillo", "CNTAutachi",
            "COAR-Puno", "COBAO", "COBAY", "COEES", "COLVIA", "CPRAfundacion", "CPereyra", "CSM",
            "CTPCIT", "CUC", "CUDI", "CUFM", "CUL", "CULTCA-VE", "CUNORI", "CUNSUROC", "CelestinoMarco",
            "CoDisOl", "ColNacJMF", "ColRosarioBogota", "ColegioAvances", "ColegioIntelecto",
            "ColegioLincoln", "ColegioNuevoGimnasio", "Conalep-Cuautla", "Conalep-Cuernavaca",
            "Conalep-NicolasRomero", "CorazonDeMaria", "DaVinci", "DonOrioneVictoria", "DuocUC",
            "EAFIT", "EEST1-Saladillo", "EET322", "EETP480", "EETP647", "EFPIA-UNDAC", "EIA",
            "ENAP", "EPET12", "EPN", "ESCRio3", "ESPE", "ESPOCH", "ESRN17", "EST50", "ET28", "ETI",
            "ETUSiemensUTN", "ETecnicaRRoca", "ElJazmin", "ElPilar", "ElValleColegio", "Euded-CPV",
            "Euded-MARM", "EudoroGranada", "FACENA-UNNE", "FACPYA-UANL", "FAH-USAC", "FCA-UNAM",
            "FIC-UAT", "FIME-UANL", "FINESI", "FINESI-UNAP-Estricto", "FINESI-UNAP-Flexible",
            "FIUBA-Schwarz-Sosa", "FJR-Tampico", "FP-UNE", "FPUNA", "IAMayllen", "ICAP", "ICEL",
            "ICESI", "IDEC", "IEBO26", "IEBrightonPamplona", "IEFelixHenao", "IEHectorAbadGomez",
            "IELuisLopezDeMesa", "IES-AntonioGaudi", "IES-ClaraDelRey", "IES-DuqueDeAlarcon",
            "IES-EnriqueTiernoGalvan", "IESNestorAlmendros", "IESTP-FVC", "IESTP-Vilcanota",
            "IEVillaDeLaCandelaria", "IFD-CoronelOviedo", "IJME", "IPJucutuma", "IPLeones",
            "IPLosLagos", "IPSS", "IParralenseAC", "ISTP", "ISTP-AbacoChiclayo", "ISTPanuco",
            "ITA", "ITC", "ITCC", "ITCG", "ITCelaya", "ITChilpancingo", "ITCuliacan", "ITDurango",
            "ITESM-PrepaTec", "ITESM-TC1001", "ITESO-AYP", "ITG", "ITIZ", "ITL", "ITMina", "ITNL",
            "ITP-Ecuador", "ITP-ISIC", "ITP-Putumayo", "ITS-Tequila", "ITSAV-AARS", "ITSAV-CLC",
            "ITSAcayucan", "ITSC", "ITSCC-JJSN", "ITSJuanDeVelasco", "ITSMisantla", "ITSOEH",
            "ITSR_advan", "ITSR_basic", "ITSSY-Oxkutzcab", "ITST", "ITSX", "ITSZO", "ITTux",
            "ITZ", "ITZacatecas", "IUGT", "IUP-Tabasco", "IUPSM-Guayana", "IUTAJS", "IUTIRLA",
            "IUTIRLA-Maturin", "IUTLL", "IUTM-Machiques", "IUTOMS-VE", "Inacap-Maipu", "Inacap-Osorno",
            "Inacap-Valparaiso", "InstMacedoMartinez", "InstitutoGottau", "JMC", "JeanPiaget",
            "Juan23-Souto", "LMAC", "LaMision", "LeccionesConTIC", "Leibnitz-PI", "LevVygotsky",
            "Luzac", "MB-UNC", "MadreVedrunaCastellon", "MalvarArganda", "MartimCerere", "Motolinia",
            "NesMeyTutoriales", "PCSantaAna", "PIO-IX", "PUCE", "PUCP", "PolitecnicoDeColombia",
            "PolitecnicoJIC", "Prepa-UAZ", "PrepaGandhi", "PrepaMexico", "Py-UTEC", "RamonCastilla",
            "SENA-CGMLTI", "SENA-SIGEC", "SENA-hm", "SENA-tadsi-Caqueta", "SENA-vhcg", "SENATI",
            "SISE", "SISE-Arequipa", "SanLuisRey", "StoTomas", "TECSUP", "TESI", "TESJI", "TESJo",
            "TLS", "TallerDeInformatica", "TecTijuana", "TecnologiaTecnica", "Torremar", "U-TAD",
            "UABC-II", "UABJO", "UACJ", "UACM-CL", "UACM-SLT", "UADY", "UAE", "UAEH-ESTi",
            "UAEM-FCQeI", "UAGRM-IntroInf", "UAGRM-Prog", "UAI-CL", "UAM", "UAMex-UAPT", "UAN",
            "UARM-TIC", "UASD", "UAT-FMeISCdeM", "UATF-II", "UATF-OBI", "UAnahuac", "UBioBio",
            "UCA-Nic", "UCAB", "UCAD", "UCC", "UCE", "UCLV-FIMI", "UCM", "UCN", "UCR-CI0202",
            "UCSC", "UCSH", "UCSP-APV", "UCSP-MPR", "UCTemuco", "UContinental", "UCuenca",
            "UDD-UCSC", "UDEA", "UDEC", "UDENAR", "UDI", "UDI-Colombia", "UDLA", "UDO-Anaco",
            "UDO-Anzoategui", "UDOYM", "UEFAL", "UEPillahuaso", "UES21", "UESucumbios", "UETS",
            "UFRO", "UG", "UGB", "UGFilo", "UIDE", "UIGV", "UIN", "UJAT-DAIA", "UJGH", "ULSA-Noreste",
            "ULSaOaxaca", "ULagos-Ancud", "ULatino", "ULibertadores", "UMAR", "UMBVirtual",
            "UManizales", "UMariana", "UMayor", "UNAB-CruzNaranjo", "UNAB-ElSalvador", "UNAB-Olivares",
            "UNACH", "UNACHI", "UNAD", "UNAJ-PUNO", "UNAM-LCPI", "UNAMBA", "UNAN-Leon", "UNAN-Managua",
            "UNAP", "UNAPEC", "UNAULA", "UNCP-FIE", "UNDAC-geo", "UNDAC-ingcivil", "UNDAC-sistcomp",
            "UNE", "UNE-LaCantuta", "UNFV", "UNHEVAL-FICA", "UNI", "UNI-FIC", "UNI-Nicaragua",
            "UNIAJC", "UNICAES", "UNICAES-Ingenieria", "UNICEN-FIO", "UNICEQ", "UNICOMPU", "UNID",
            "UNIDA", "UNIFIP", "UNINORTE", "UNISTMO-Ixtepec", "UNITEC-ni", "UNITEC-ve", "UNITEK-PUNO",
            "UNIVES", "UNJu", "UNL-FICH", "UNLPam", "UNLZ", "UNLa", "UNLa-IntroLS-EPyA", "UNMSM-FII",
            "UNN", "UNSAM", "UNSL-FCFMN", "UNSM-FISI", "UNSa-Oran", "UNT", "UNTRM", "UNaM-FIO",
            "UNorte", "UPBicentenario", "UPC", "UPC-Algoritmos", "UPCH", "UPDS", "UPEC", "UPEL-IPC",
            "UPES", "UPN", "UPONIC", "UPPE", "UPQ", "UPS", "UPSIN", "UPSO", "UPTECMS", "UPTNM",
            "UPTNMLS", "UPTelesup", "UPTex", "UPTulancingo", "UPVE", "UPanama", "UPlayaAncha",
            "URACCAN", "URosario", "USACH-IE-DMCC", "USBMed", "USCancun", "USFX-SIS100", "USM",
            "USPBarranca", "USS-IAP-Patgonia", "USS-ICI", "USTATUNJA", "USalesiana", "USergioArboleda",
            "UTA", "UTA-FISEI-Jara", "UTA-FISEI-Paredes", "UTA-Iquique", "UTCAM", "UTCD", "UTCH",
            "UTCorregidora", "UTDFT", "UTEC", "UTEtchojoa", "UTFV", "UTH", "UTH-Cofradia", "UTHH",
            "UTJ", "UTM", "UTMach", "UTMarT", "UTN-FRM", "UTN-FRSFco", "UTN-FRSN", "UTNayarit",
            "UTNeza", "UTP-Panama", "UTP-Peru", "UTPP", "UTS", "UTSOE", "UTSalamanca", "UTUsumacinta",
            "UTZAC", "UTalca", "UTalca-IIE", "UTalca-Videojuegos", "UVIM", "UVM-Hispano", "UValparaiso",
            "UdeCaldas", "UdeM-Managua", "UdeMM", "UnADM", "UniAmazonia", "UniAtlantico", "UniCauca",
            "UniGuajira", "UniIncca", "UniMagdalena", "UniMinuto", "UniMoron", "UniPiloto",
            "UniQuindio", "UniSon-LCC", "UniSur", "UniTru", "UniValle", "Unicafam-UA", "Unifranz",
            "VicenteFierro", "Yucatan", "cbtis53", "cobae-plantel11", "facet", "poligran"
        )

        fun saveCustomProfile(context: Context, profile: PSeIntProfile) {
            val prefs = context.getSharedPreferences("custom_profile_prefs", Context.MODE_PRIVATE)
            prefs.edit().apply {
                putBoolean("forceDefineVariables", profile.forceDefineVariables)
                putBoolean("allowEqualsAssignment", profile.allowEqualsAssignment)
                putBoolean("requireSemicolons", profile.requireSemicolons)
                putBoolean("strictTypes", profile.strictTypes)
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
                strictTypes = prefs.getBoolean("strictTypes", false)
            )
        }
    }
}
