#include "HTMLExporter.hpp"

HTMLExporter::HTMLExporter ( ) :JSExporter(true) {
	
}

void HTMLExporter::translate (t_output &out, Programa &prog) {
	insertar(out,"<!DOCTYPE html>");
	insertar(out,"<HTML>");
	insertar(out,"\t<HEAD>");
	insertar(out,std::string("\t\t<TITLE>")+GetRT().funcs.GetMainName()+"</TITLE>");
	insertar(out,"\t</HEAD>");
	insertar(out,std::string("\t<BODY onload=\"")+ToLower(GetRT().funcs.GetMainName())+"();\">");
	insertar(out,"\t\t<SCRIPT type=\"text/javascript\">");
	JSExporter::translate(out,prog);
	insertar(out,"\t\t</SCRIPT>");
	insertar(out,"\t</BODY>");
	insertar(out,"</HTML>");
}
