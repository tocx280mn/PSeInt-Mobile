#include <fstream>
#include "../pseint/FuncsManager.hpp"
#include "../pseint/utils.h"
#include "defines.h"
#include "version.h"
#include "ExporterBase.hpp"
#include "exportexp.h"
#include "CppExporter.hpp"
#include "CExporter.hpp"
#include "JavaExporter.hpp"
#include "JSExporter.hpp"
#include "HTMLExporter.hpp"
#include "VBExporter.hpp"
#include "QBasicExporter.hpp"
#include "PythonExporter.hpp"
#include "PhpExporter.hpp"
#include "PascalExporter.hpp"
#include "MatLabExporter.hpp"
#include "CSharpExporter.hpp"
#include "ProgramaDump.hpp"

using namespace std;

LangSettings lang(LS_DO_NOT_INIT);

void MakeInstructionReferencesRelative(Programa &prog) {
	for(size_t i=0;i<prog.GetInstCount();++i) {
		Instruccion &inst = prog[i];
		switch(inst.type) {
		case IT_MIENTRAS:
			getImpl<IT_MIENTRAS>(inst).fin -= i;
			break;
		case IT_REPETIR:
			getImpl<IT_REPETIR>(inst).fin -= i;
			break;
		case IT_SI:
			if (getImpl<IT_SI>(inst).sino!=-1)
				getImpl<IT_SI>(inst).sino -= i;
			getImpl<IT_SI>(inst).fin -= i;
			break;
		case IT_SEGUN:
			getImpl<IT_SEGUN>(inst).fin -= i;
			for(int &op : getImpl<IT_SEGUN>(inst).opciones) 
				op -= i;
			break;
		case IT_PARA:
			getImpl<IT_PARA>(inst).fin -= i;
			break;
		case IT_PARACADA:
			getImpl<IT_PARACADA>(inst).fin -= i;
			break;
		case IT_PROCESO:
			getImpl<IT_PROCESO>(inst).fin -= i;
			break;
		case IT_OPCION:
			getImpl<IT_OPCION>(inst).siguiente -= i;
			break;
		}
	}
}

int main(int argc, char *argv[]){

	std::string fname_in, fname_out;

	_handle_version_query("psExport",0);
	
	lang.Reset();
	
	for (int i=1;i<argc;i++) {
		std::string s=argv[i];
		if (s=="--install-test") {
			std::cout << "OK";
			return 0;
		} else if (s=="--help") {
			std::cerr << "Use: " << argv[0]<<" [--base_zero_arrays=1] [--lang=<lenguaje>] <in_file.psd> <out_file.cpp>\n";
			return 1;
		} else if (s.substr(0,7)=="--lang=") {
			s.erase(0,7); 
			if (s=="c" || s=="c99") exporter = new CExporter();
			else if (s=="c++" || s=="cpp" || s=="c++98" || s=="cpp98" || s=="c++03" || s=="cpp03") exporter = new CppExporter();
			else if (s=="c#" || s=="cs" || s=="csharp") exporter = new CSharpExporter();
			else if (s=="htm" || s=="html") exporter=new HTMLExporter();
			else if (s=="java") exporter = new JavaExporter();
			else if (s=="js" || s=="javascript") exporter=new JSExporter();
			else if (s=="m" || s=="matlab") exporter = new MatLabExporter();
			else if (s=="pas" || s=="pascal") exporter=new PascalExporter();
			else if (s=="php") exporter=new PhpExporter();
			else if (s=="py" || s=="pyton" || s=="py3" || s=="python3") exporter=new PythonExporter(3);
			else if (s=="py2" || s=="python2") exporter=new PythonExporter(2);
			else if (s=="vb" || s=="visualbasic") exporter=new VbExporter();
			else if (s=="bas" || s=="qb" || s=="qbasic" || s=="quickbasic") exporter = new QBasicExporter();
			else {
				if (s=="ook") { 
					char s[]="Uû!op!fsft!vo!psbohvuâo-!qfsp!upnb!vob!cbobob;"; int i=0; while(s[i]!='\0') s[i++]--; cout<<s<<endl;
					cout<<"\t    _\n\t   | |\n\t  /  /\n\t /  /|\n\t | | |\n\t | | |\n\t | | |\n\t \\ | |\n\t  \\\\ |\n\t   \\__\\"<<endl;
					return 0;
				} else {
					cerr<<"El lenguaje no es válido. Los lenguajes disponibles son: bas, c, cpp, html, java, js, pas, php, py2, py3, vb"<<endl;
				}
			}
		} else if (s=="--for-testing") {
			for_test=true;
		} else if (s.substr(0,2)=="--" && lang.ProcessConfigLine(s.substr(2))) {
			; // procesado en lang.ProcessConfigLine
		} else if (fname_in.size() && fname_out.size()) {
			cerr<<"Argumentos incorrectos"<<endl;
			return 1;
		} else if (fname_in.size()) {
			fname_out=s;
		} else {
			fname_in=s;
		}
	}
	if (!fname_out.size()) {
		cerr<<"Argumentos incorrectos: falta archivo de salida."<<endl;
		return 1;
	}
	if (!exporter) {
		cerr<<"Argumentos incorrectos: no se especificó lenguaje."<<endl;
		return 1;
	}
	lang.Fix();
	input_base_zero_arrays=lang[LS_BASE_ZERO_ARRAYS];
	lang[LS_BASE_ZERO_ARRAYS]=output_base_zero_arrays; // lo va a consultar el evaluador de expresiones
	exporter->GetRT().funcs.LoadPredefs();
	
	// cargar programa
	Programa prog = LoadPrograma(fname_in);
	if (prog.GetInstCount()==0) {
		std::cerr << "No se pudo abrir el archivo " << fname_in << std::endl; 
		return 1;
	}
	// hacer las refs de una instruccion a otra relativas (porque las funcs de 
	// exporter reciben iterators, pero no el contenedor, entonces no pueden
	// usar posiciones absolutas del mismo)
	MakeInstructionReferencesRelative(prog);
	// convertir
	t_output out;
	exporter->translate(out,prog);
	
#ifdef _USE_COUT
#define fout cout
#else
	ofstream fout(fname_out.c_str());
#endif
	t_output::iterator it=out.begin();
	while (it!=out.end())
		fout<<*(it++)<<endl;
#ifndef _USE_COUT
	fout.close();
#endif
	
	return 0;
}
	
