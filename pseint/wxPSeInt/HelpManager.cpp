#include "HelpManager.h"
#include <wx/textfile.h>
#include "ConfigManager.h"
#include "string_conversions.h"

HelpManager *help = NULL;

HelpManager::HelpManager() {
	LoadErrors();
	LoadCommands();
}

void HelpManager::LoadCommands() {
	wxTextFile fil(DIR_PLUS_FILE(config->help_dir,"comandos.hlp"));
	if (!fil.Exists()) return;
	fil.Open();
	wxString command, help_text;
	for ( wxString str = fil.GetFirstLine(); !fil.Eof(); str = fil.GetNextLine() ) {
		if (str[0]==' ') {
			help_text<<str;//<<"<br>";
		} else {
			commands[command] = help_text;
			help_text.Clear();
			command=str;
		}
	}
	commands[command] = help_text;
	fil.Close();
}

void HelpManager::LoadErrors() {
	for(int k=0;k<2;++k) {
		wxTextFile fil(DIR_PLUS_FILE(config->help_dir, k==0?"errores.hlp":"warnings.hlp"));
		if (not fil.Open()) continue;
		wxString command("0"), help_text;
		for ( wxString str = fil.GetFirstLine(); !fil.Eof(); str = fil.GetNextLine() ) {
			if (str[0]==' ') {
				help_text << str << "<br>";
			} else {
				long e = 0; command.ToLong(&e);
				errors[e] = help_text;
				help_text.Clear();
				command = str;
			}
		}
		long e=0; command.ToLong(&e);
		errors[e] = help_text;
		fil.Close();
	}
}

wxString HelpManager::GetErrorText(const wxString &text, int num) {
	wxString ret(_Z("<HTML><HEAD><TITLE>PSeInt QuickHelp</TITLE></HEAD><BODY><B>"));
	if (num>=MAX_ERRORS_TXT || errors[num].Len()==0) 
		ret<<utils->ToHtml(text)<<_Z("</B><BR><BR> No se encontro descripción para este error <BR><BR></BODY></HTML>");
	else
		ret<<utils->ToHtml(text)<<_Z("</B><BR><BR>")<<errors[num]<<_Z("<BR><BR></BODY></HTML>");
	return ret;
}

wxString HelpManager::GetCommandText(const wxString &com) {
	wxString ret(_Z("<HTML><HEAD><TITLE>PSeInt QuickHelp</TITLE></HEAD><BODY><B>"));
	ret<<com<<_Z("</B><BR><BR>")<<commands[com]<<_Z("<BR><BR></BODY></HTML>");
	return ret;
}

wxString HelpManager::GetQuickHelp(wxString key, wxString def_val) {
	wxString &str=def_val;
	key.MakeLower();
	if (key==_Z("leer")) str="lectura.html";
	else if (key==_Z("lectura")) str="lectura.html";
	else if (key==_Z("mayusculas")) str="funciones.html";
	else if (key==_Z("mayúsculas")) str="funciones.html";
	else if (key==_Z("minusculas")) str="funciones.html";
	else if (key==_Z("minúsculas")) str="funciones.html";
	else if (key==_Z("longitud")) str="funciones.html";
	else if (key==_Z("subcadena")) str="funciones.html";
	else if (key==_Z("concatenar")) str="funciones.html";
	else if (key==_Z("convertiranumero")) str="funciones.html";
	else if (key==_Z("convertiranúmero")) str="funciones.html";
	else if (key==_Z("convertiratexto")) str="funciones.html";
	else if (key==_Z("subproceso")) str="subprocesos.html";
	else if (key==_Z("subprocesos")) str="subprocesos.html";
	else if (key==_Z("función")) str="subprocesos.html";
	else if (key==_Z("funcion")) str="subprocesos.html";
	else if (key==_Z("subalgoritmo")) str="subprocesos.html";
	else if (key==_Z("finsubalgoritmo")) str="subprocesos.html";
	else if (key==_Z("sin")) str="escritura.html";
	else if (key==_Z("saltar")) str="escritura.html";
	else if (key==_Z("bajar")) str="escritura.html";
	else if (key==_Z("sinbajar")) str="escritura.html";
	else if (key==_Z("sinsaltar")) str="escritura.html";
	else if (key==_Z("mostrar")) str="escritura.html";
	else if (key==_Z("imprimir")) str="escritura.html";
	else if (key==_Z("escribir")) str="escritura.html";
	else if (key==_Z("escritura")) str="escritura.html";
	else if (key==_Z("dimension")) str="arreglos.html";
	else if (key==_Z("dimensión")) str="arreglos.html";
	else if (key==_Z("dimensionar")) str="arreglos.html";
	else if (key==_Z("redimension")) str="arreglos.html";
	else if (key==_Z("redimensión")) str="arreglos.html";
	else if (key==_Z("redimensionar")) str="arreglos.html";
	else if (key==_Z("matriz")) str="arreglos.html";
	else if (key==_Z("matrices")) str="arreglos.html";
	else if (key==_Z("arreglos")) str="arreglos.html";
	else if (key==_Z("arreglo")) str="arreglos.html";
	else if (key==_Z("vector")) str="arreglos.html";
	else if (key==_Z("vectores")) str="arreglos.html";
	else if (key==_Z("son")) str="definir.html";
	else if (key==_Z("definir")) str="definir.html";
	else if (key==_Z("definición")) str="definir.html";
	else if (key==_Z("definicion")) str="definir.html";
	else if (key==_Z("como")) str="definir.html";
	else if (key==_Z("numérico")) str="tipos.html";
	else if (key==_Z("numérica")) str="tipos.html";
	else if (key==_Z("numero")) str="tipos.html";
	else if (key==_Z("número")) str="tipos.html";
	else if (key==_Z("logica")) str="tipos.html";
	else if (key==_Z("logico")) str="tipos.html";
	else if (key==_Z("cadena")) str="tipos.html";
	else if (key==_Z("texto")) str="tipos.html";
	else if (key==_Z("caracter")) str="tipos.html";
	else if (key==_Z("si")) str="si.html";
	else if (key==_Z("finsi")) str="si.html";
	else if (key==_Z("entonces")) str="si.html";
	else if (key==_Z("sino")) str="si.html";
	else if (key==_Z("asignar")) str="asignacion.html";
	else if (key==_Z("asignacion")) str="asignacion.html";
	else if (key==_Z("repetir")) str="repetir.html";
	else if (key==_Z("paso")) str="para.html";
	else if (key==_Z("finpara")) str="para.html";
	else if (key==_Z("para")) str="para.html";
	else if (key==_Z("cada")) str="para.html";
	else if (key==_Z("en")) str="para.html";
	else if (key==_Z("de")) str="para.html";
	else if (key==_Z("desde")) str="para.html";
	else if (key==_Z("paso")) str="para.html";
	else if (key==_Z("hasta")) str="para.html";
	else if (key==_Z("opcion")) str="segun.html";
	else if (key==_Z("caso")) str="segun.html";
	else if (key==_Z("segun")) str="segun.html";
	else if (key==_Z("finsegun")) str="segun.html";
	else if (key==_Z("según")) str="segun.html";
	else if (key==_Z("finsegún")) str="segun.html";
	else if (key==_Z("finmientras")) str="mientras.html";
	else if (key==_Z("mientras")) str="mientras.html";
	else if (key==_Z("proceso")) str="forma.html";
	else if (key==_Z("finproceso")) str="forma.html";
	else if (key==_Z("algoritmo")) str="forma.html";
	else if (key==_Z("finalgoritmo")) str="forma.html";
	else if (key==_Z("vectores")) str="arreglos.html";
	else if (key==_Z("operador")) str="operadores.html";
	else if (key==_Z("operadores")) str="operadores.html";
	else if (key==_Z("funciones")) str="funciones.html";
	else if (key==_Z("pi")) str="funciones.html";
	else if (key==_Z("euler")) str="funciones.html";
	else if (key==_Z("sen")) str="funciones.html";
	else if (key==_Z("asen")) str="funciones.html";
	else if (key==_Z("cos")) str="funciones.html";
	else if (key==_Z("acos")) str="funciones.html";
	else if (key==_Z("tan")) str="funciones.html";
	else if (key==_Z("atan")) str="funciones.html";
	else if (key==_Z("azar")) str="funciones.html";
	else if (key==_Z("aleatorio")) str="funciones.html";
	else if (key==_Z("trunc")) str="funciones.html";
	else if (key==_Z("rc")) str="funciones.html";
	else if (key==_Z("ln")) str="funciones.html";
	else if (key==_Z("redon")) str="funciones.html";
	else if (key==_Z("abs")) str="funciones.html";
	else if (key==_Z("fecha")) str="funciones.html";
	else if (key==_Z("hora")) str="funciones.html";
	else if (key==_Z("fechaactual")) str="funciones.html";
	else if (key==_Z("horaactual")) str="funciones.html";
	else if (key==_Z("hacer")) str="hacer.html";
	else if (key==_Z("tipos")) str="tipos.html";
	else if (key==_Z("tipo")) str="tipos.html";
	else if (key==_Z("otro")) str="segun.html";
	else if (key==_Z("modo")) str="segun.html";
	else if (key==_Z("sies")) str="segun.html";
	else if (key==_Z("case")) str="segun.html";
	else if (key==_Z("opcion")) str="segun.html";
	else if (key==_Z("mod")) str="operadores.html";
	else if (key==_Z("divisible")) str="colcond.html";
	else if (key==_Z("multiplo")) str="colcond.html";
	else if (key==_Z("positivo")) str="colcond.html";
	else if (key==_Z("negativo")) str="colcond.html";
	else if (key==_Z("impar")) str="colcond.html";
	else if (key==_Z("par")) str="colcond.html";
	else if (key==_Z("cero")) str="colcond.html";
	else if (key==_Z("distinto")) str="colcond.html";
	else if (key==_Z("igual")) str="colcond.html";
	else if (key==_Z("es")) str="colcond.html";
	else if (key==_Z("menor")) str="colcond.html";
	else if (key==_Z("mayor")) str="colcond.html";
	else if (key==_Z("esperartecla")) str="secotras.html";
	else if (key==_Z("tecla")) str="secotras.html";
	else if (key==_Z("esperar")) str="secotras.html";
	else if (key==_Z("limpiar")) str="secotras.html";
	else if (key==_Z("limpiarpantalla")) str="secotras.html";
	else if (key==_Z("borrar")) str="secotras.html";
	else if (key==_Z("borrarpantalla")) str="secotras.html";
	else if (key==_Z("pantalla")) str="secotras.html";
	else if (key==_Z("variable")) str="variables.html";
	else if (key==_Z("variables")) str="variables.html";
	else if (key==_Z("ejemplo")) str="ejemplos.html";
	else if (key==_Z("ejemplos")) str="ejemplos.html";
	if (def_val.Len()) str = DIR_PLUS_FILE(config->help_dir,str);
	return str;	
}
