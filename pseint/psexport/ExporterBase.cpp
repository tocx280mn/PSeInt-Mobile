#include <cstdlib>
#include <memory>
#include "ExporterBase.hpp"
#include "exportexp.h"
#include "version.h"
#include "Evaluar.hpp"
#include "strFuncs.hpp"
using namespace std;

//map<string,Memoria*> ExporterBase::mapa_memorias;

ExporterBase *exporter=NULL;

RunTime ExporterBase::m_runtime;

string ExporterBase::make_dims(const int *tdims, string c1, string c2, string c3, bool numbers) {
	string dims=c1;
	for (int j=1;j<=tdims[0];j++) {
		if (j==tdims[0]) dims+=(numbers?IntToStr(tdims[j]):"")+c3;
		else dims+=(numbers?IntToStr(tdims[j]):"")+c2;
	}
	return dims;
}


void ExporterBase::bloque(t_output &prog, t_proceso_it r, t_proceso_it q, std::string tabs){
	if (r==q) return;
	while (r!=q) {
		switch(r->type) {
		case IT_COMMENT:
			comentar(prog,getImpl<IT_COMMENT>(*r).text,tabs);
			break;
		case IT_ESCRIBIR: {
			auto &impl = getImpl<IT_ESCRIBIR>(*r);
			escribir(prog,impl.expresiones,impl.saltar,tabs);
		} break;
		case IT_INVOCAR: {
			auto &impl = getImpl<IT_INVOCAR>(*r);
			invocar(prog,impl.nombre,impl.args,tabs);
		} break;
		case IT_BORRARPANTALLA:
			borrar_pantalla(prog,tabs);
			break;
		case IT_ESPERARTECLA:
			esperar_tecla(prog,tabs);
			break;
		case IT_ESPERAR: {
			auto &impl = getImpl<IT_ESPERAR>(*r);
			esperar_tiempo(prog,impl.tiempo,impl.factor==1,tabs);
		} break;
		case IT_DIMENSION: {
			auto &impl = getImpl<IT_DIMENSION>(*r);
			dimension(prog,impl.nombres,impl.tamanios,tabs);
		} break;
		case IT_DEFINIR: {
			auto &impl = getImpl<IT_DEFINIR>(*r);
			definir(prog,impl.variables,impl.tipo,tabs);
		} break;
		case IT_LEER: {
			auto &impl = getImpl<IT_LEER>(*r);
			// ACA HABIA ORIGINALMENTE UN DEFINIR TIPO POR CADA VAR PARA QUE LAS REGISTRE EN MEMORIA
			leer(prog,impl.variables,tabs);
		} break;
		case  IT_ASIGNAR: {
			auto &impl = getImpl<IT_ASIGNAR>(*r);
			asignacion(prog,impl.variable,impl.valor,tabs);
		} break;
		case IT_MIENTRAS: {
			auto &impl = getImpl<IT_MIENTRAS>(*r);
			auto it_fin = std::next(r,impl.fin);
			mientras(prog,r,it_fin,tabs);
			r = it_fin;
		} break;
		case IT_REPETIR: {
			auto &impl1 = getImpl<IT_REPETIR>(*r);
			auto it_fin = std::next(r,impl1.fin);
			repetir(prog,r,it_fin,tabs);
			r = it_fin;
		} break;
		case IT_SEGUN: {
			auto &impl = getImpl<IT_SEGUN>(*r);
			std::vector<t_proceso_it> its;
			its.push_back(r);
			for(int op : impl.opciones)
				its.push_back( std::next(r,op) );
			its.push_back(std::next(r,impl.fin));
			segun(prog,its,tabs);
			r = its.back();
		} break;
		case IT_PARA: {
			auto &impl = getImpl<IT_PARA>(*r);
			auto it_fin = std::next(r,impl.fin);
			para(prog,r,it_fin,tabs);
			r = it_fin;
		} break;
		case IT_PARACADA: {
			auto &impl = getImpl<IT_PARACADA>(*r);
			auto it_fin = std::next(r,impl.fin);
			paracada(prog,r,it_fin,tabs);
			r = it_fin;
		} break;
		case IT_SI: {
			auto &impl = getImpl<IT_SI>(*r);
			auto it_fin = std::next(r,impl.fin);
			si(prog,r,std::next(r,impl.sino==-1?impl.fin:impl.sino),it_fin,tabs);
			r = it_fin;
		} break;
		default:
			_impossible;
		}
		++r;
	}
}

void ExporterBase::definir(t_output &prog, t_arglist &variables, tipo_var tipo, std::string tabs) {
	for(auto &var:variables)
		memoria->DefinirTipo(ToUpper(var),tipo,tipo.rounded);
}

void ExporterBase::dimension(t_output &prog, t_arglist &nombres, t_arglist &tamanios, std::string tabs) {
	for(size_t iarg=0;iarg<nombres.size();++iarg) { 
		string arr = tamanios[iarg];
		arr += ",";
		unsigned int c = 0, pars = 0;
		for (int i=0;i<arr.size();i++)
			if (arr[i]=='(' && pars==0) { 
				pars++;
			} else if (arr[i]==')') {
				pars--;
			} else if (arr[i]==',' && pars==0) {
				c++;
			} else if (arr[i]>='A' && arr[i]<='Z') {
				arr[i]=tolower(arr[i]);
			}
		int *dims = new int[c+1];
		dims[0] = c;
		int f=0, p=0; c=1; pars=0;
		while (p<arr.size()) {
			if (arr[p]=='(')
				pars++;
			else if (arr[p]==')')
				pars++;
			else if (arr[p]==',' && pars==0) {
				tipo_var ch = vt_numerica_entera;
				dims[c++] = Evaluar(GetRT(),arr.substr(f,p-f),ch).GetAsInt();
				f=p+1;
			}
			p++;
		}
		memoria->AgregarArreglo(nombres[iarg],dims);
	}
}

// recibe los argumentos de una funcion (incluyendo los parentesis que los envuelven, y extra alguno (cual, base 1)
string ExporterBase::get_arg(string args, int cual) {
	int i=1,i0=1,parentesis=0,n=0; bool comillas=false;
	while (true) {
		if (args[i]=='\''||args[i]=='\"') comillas=!comillas;
		else if (!comillas) {
			if (parentesis==0 && (args[i]==','||args[i]==')')) {
				if (++n==cual) {
					if (args[i0]==' ') ++i0;
					return args.substr(i0,i-i0); 
				}
				i0=i+1;
			} 
			else if (args[i]=='('||args[i]=='[') parentesis++;
			else if (args[i]==')'||args[i]==']') parentesis--;
		}
		i++;
	}
	return "";
}

bool ExporterBase::es_cadena_constante(const string &s) {
	if (s[0]!='\'' && s[0]!='\"') return false;
	int l=s.size()-1;
	if (s[l]!='\'' && s[l]!='\"') return false;
	for(int i=1;i<l;i++) { 
		if (s[i]=='\'' || s[i]=='\"') return false;
	}
	return true;
}

bool ExporterBase::es_numerica_constante(const string &s) {
	int l=s.size();
	for(int i=0;i<l;i++) { 
		if ( (s[i]>='a' && s[i]<='z') || 
			 (s[i]>='A' && s[i]<='Z') || 
			 s[i]=='\'' || s[i]=='\"' || s[i]=='_'
		   ) return false;
	}
	return true;
}

bool ExporterBase::es_numerica_entera_constante(const string &s) {
	return es_numerica_constante(s) && s.find('.')==string::npos;
}

void ExporterBase::init_header(t_output &out, string comment_pre, string comment_post) {
	stringstream version; 
	version<<VERSION<<"-"<<ARCHITECTURE;
	if (!for_test) {
		out.push_back(comment_pre+"Este codigo ha sido generado por el modulo psexport "+version.str()+" de PSeInt.");
		if (comment_post.size()) comment_pre="";
		out.push_back(comment_pre+"Es posible que el codigo generado no sea completamente correcto. Si encuentra");
		out.push_back(comment_pre+"errores por favor reportelos en el foro (http://pseint.sourceforge.net)."+comment_post);
		if (!for_test) out.push_back("");
	}
}

void ExporterBase::replace_var(t_output &out, string src, string dst) {
	t_output_it it=out.begin();
	while (it!=out.end()) {
		string s=*it;
		bool comillas=false;
		for(unsigned int i=0, l=0;i<=s.size();i++) { 
			if (i<s.size() &&(s[i]=='\''||s[i]=='\"')) comillas=!comillas;
			if (!comillas) {
				if (i==s.size()||(s[i]!='_'&&s[i]!='.'&&(s[i]<'0'||s[i]>'9')&&(s[i]<'a'||s[i]>'z')&&(s[i]<'A'||s[i]>'Z'))) {
					if (i!=l && s.substr(l,i-l)==src) {
						s.replace(l,i-l,dst);
						i+=dst.size()-src.size();
					} 
					l=i+1;
				}
			}
		}
		*it=s;
		++it;
	}
}

string ExporterBase::make_varname(string varname) {
	return ToLower(varname);
}

/**
* Esta funcion está para que varias instancias derivadas de ExporterBase puedan
* utilizar la el mismo mapa de memorias (que contiene una instancia de Memoria
* por proceso/subproceso). Esto es por ejemplo, para que el que exporta a c++
* primero pueda hacer una pasada del algoritmo com TiposExport para que las 
* variables ya tengan sus tipos definidos en cada ambito. SiNo, por ejemplo
* podría declararse una variable como string o sin_tipo en un paso por no 
* conocer su tipo y luego darse cuenta que un paso siguiente se utiliza como
* indice o dimensión de un arreglo y entonces debió ser int. Lo mismo se aplica
* a las lecturas en lenguajes donde no son iguales para todos los tipos (como
* c o vb).
**/
void ExporterBase::set_memoria(string key) {
	// static para que se comparta por ej entre el de tipos y el de otro lenguaje cuando se encadenan
	static map<string,std::unique_ptr<Memoria>> mapa_memorias;
	auto it = mapa_memorias.find(key);
	if (it==mapa_memorias.end()) {
		memoria = (mapa_memorias[key] = std::make_unique<Memoria>(nullptr)).get();
	} else {
		memoria = it->second.get();
	}
}

string ExporterBase::get_aux_varname(string pref) {
	stringstream ss; ss<<pref<<(aux_varnames.size()+(output_base_zero_arrays?0:1));
	aux_varnames.push_back(ss.str());
	return ss.str();
}

void ExporterBase::release_aux_varname(string vname) {
	if (vname!=aux_varnames.back()) cerr<<"ERROR RELEASING AUX VARNAME\n";
	aux_varnames.pop_back();
}

void ExporterBase::crop_name_and_dims(string decl, string &name, string &dims, string par_open, string comma, string par_close) {
	name=decl; dims=decl;
	name.erase(name.find("("));
	dims.erase(0,dims.find("(")+1);
	dims.erase(dims.size()-1,1);
	fix_dims(dims,par_open,comma,par_close);
}

void ExporterBase::fix_dims(string &dims, string par_open, string comma, string par_close) {
	t_arglist dimlist = splitArgsList(dims);
	dims=par_open; int n=0;
	t_arglist_it it2=dimlist.begin();
	while (it2!=dimlist.end()) {
		if ((n++)) dims+=comma;
		dims+=expresion(*it2);
		++it2;
	}
	dims+=par_close;
}

bool ExporterBase::replace_all(string &str, string from, string to) {
	bool retval=false;
	size_t pos = str.find(from,0);
	while (pos!=string::npos) {
		retval=true;
		str.replace(pos,from.size(),to);
		pos=str.find(from,pos+to.size());
	}
	return retval;
}

void ExporterBase::comentar (t_output & prog, string text, std::string tabs) {
	
}

void ExporterBase::translate_all_procs (t_output &out, Programa &prog, std::string tabs) {
	translate_all_procs(out,out,prog,tabs);
}

void ExporterBase::translate_all_procs (t_output &out_main, t_output &out_procs, Programa &prog, std::string tabs) {
	for (auto it = prog.begin(); it!=prog.end(); ++it) {
		t_output out_com; 
		while (it->type==IT_COMMENT) {
			comentar(out_com,getImpl<IT_COMMENT>(*it).text,tabs);
			if (++it==prog.end()) {
				insertar_out(out_main,out_com);
				return;
			}
		}
		std::unique_ptr<Funcion> f;
		auto &impl = getImpl<IT_PROCESO>(*it);
		if (impl.principal) { set_memoria(impl.nombre); }
		else { f = MakeFuncionForSubproceso(GetRT(),{impl.ret_id,impl.nombre,impl.args},false); set_memoria(f->id); }
		auto it_fin = std::next(it,getImpl<IT_PROCESO>(*it).fin);
		insertar_out(f?out_procs:out_main,out_com);
		t_proceso proc(it,it_fin);
		translate_single_proc(f?out_procs:out_main,f.get(),proc);
		it = it_fin;
	}
}

void ExporterBase::translate_single_proc (t_output & out, Funcion *f, t_proceso & proc) {
	
}

void ExporterBase::load_subs_in_funcs_manager (Programa & prog) {
	for(auto &inst : prog) {
		if (inst.type!=IT_PROCESO) continue;
		auto &impl = getImpl<IT_PROCESO>(inst);
		GetRT().funcs.AddSub( MakeFuncionForSubproceso(GetRT(),{impl.ret_id,impl.nombre,impl.args},impl.principal) );
		if (impl.principal) GetRT().funcs.SetMain(impl.nombre);
	}
}

