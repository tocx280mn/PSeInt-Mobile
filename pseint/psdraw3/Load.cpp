#include <fstream>
#include <stack>
#include <iostream>
#include <sstream>
#include "Load.h"
#include "Global.h"
#include "Entity.h"
#include "Events.h"
#include "ProcessSelector.h"
#include "../wxPSeInt/CommonParsingFunctions.h"
#include "../pseint/Programa.hpp"
#include "../pseint/ProgramaDump.hpp"

#define _kw(id) g_lang.keywords[id].get(false)

// Agrega una nueva entidad donde corresponda. El "donde corresponda" sería
// o bien como hijo de la previa, o bien como siguiente de la previa.
// Para decidir cual corresponde está children_stack, que tiene en su top
// -1 si va como siguiente, o el indice de hijo si va como hija.
// El id que recibe es para indicar a dónde debería ir la próxima... es decir
// si recibo -1 (escribir, leer, dimension,... la próxima) irá como siguiente
// mientras que si recibo otra cosa la próxima irá como hija en ese lugar (0
// para Para, Repetir, Mientras; 1 para Si; 0 a X para Segun; etc).

static Entity *Add(std::stack<int> &children_stack, Entity *previa, Entity *nueva, int child_id_for_next=-1) {
	int where = children_stack.top(); 
	children_stack.pop(); 
	if (nueva->type==ET_OPCION) { assert(previa->type==ET_SEGUN); children_stack.push(where+(nueva->label==g_lang.keywords[KW_DEOTROMODO].get(false)?0:1)); }
	children_stack.push(-1);
	if (where==-1) {
		previa->LinkNext(nueva);
	} else { 
		if (nueva->type==ET_OPCION) {
			if (nueva->label==g_lang.keywords[KW_DEOTROMODO].get(false)) {
				delete nueva; nueva=previa->GetChild(previa->GetChildCount()-1); // porque el child para dom se crea ya en el ctor del segun
			} else {
				previa->InsertChild(where,nueva);
			}
		} else {
			previa->LinkChild(where,nueva);
		}
	}
	if (child_id_for_next!=-1) 
		children_stack.push(child_id_for_next);
	return nueva;
}


// Sube un nivel en el children_stack. Se usa por ej cuando se cierra 
// alguna estructura de control
static Entity *Up(std::stack<int> &children_stack, Entity *previa) {
	int where = children_stack.top();
	children_stack.pop(); 
	if (where==-1) {
		return previa->GetParent();
	} else { // si esperaba un hijo pero no vino nada (bucle vacio)
//		if (previa->type!=ET_OPCION && previa->type!=ET_SEGUN) children_stack.push(-1); 
		return previa;
	}
}

static void RemoveParentesis(std::string &ini) {
	if (ini.size()&&ini[0]=='(') { // suele venir envuelta en parentesis
		int par=1;
		for(unsigned int i=1;i<ini.size();i++) { 
			if (ini[i]=='(') par++;
			else if (ini[i]==')') {
				par--;
				if (par==0 && i==ini.size()-1)
					ini=ini.substr(1,ini.size()-2);
				if (par==0) break;
			}
		}
	}
}

static void ReemplazarOperadores(std::string &str) {
	bool comillas=false;
	for(unsigned int i=0;i<str.size();i++) { 
		if (str[i]=='\'' || str[i]=='\"') comillas=!comillas;
		else if (!comillas) {
			if (str[i]=='&') str.replace(i,str[i+1]=='&'?2:1," Y ");
			else if (str[i]=='|') str.replace(i,str[i+1]=='|'?2:1," O ");
			else if (str[i]=='~') str.replace(i,1," NO ");
			else if (str[i]=='%') str.replace(i,1," MOD ");
		}
	}
}

std::string join(const std::vector<std::string> &v) {
	std::string s;
	for (size_t i=0; i<v.size(); ++i) { 
		if (i) s += ", ";
		s += v[i];
	}
	return s;
}

#define cur_pos std::to_string(inst.loc.linea)+":"+std::to_string(inst.loc.instruccion)
#define _new_this(e) g_code.code2draw[cur_pos] = LineInfo{cur_proc,e}
#define _new_prev()  g_code.code2draw[cur_pos] = LineInfo{nullptr,cur_proc}
#define _new_none()  g_code.code2draw[cur_pos] = LineInfo{nullptr,nullptr}

void LoadProc(Programa &prog, int &i_inst) {
	g_code.code2draw.clear();
	Entity *cur_proc; // para llenar code2draw
	bool start_done = false;
	cur_proc = g_code.start = new Entity(ET_PROCESO,"SinTitulo");
	Entity *aux = g_code.start, *aux_end = g_code.start;
	// child_id guarda en qué lugar de los hijos de la entidad anterior hay que
	// enlazar la próxima, o -1 si la próxima va como next y no como hijo
	std::stack<int> children_stack; children_stack.push(-1);
	for (; prog[i_inst].type!=IT_FINPROCESO; ++i_inst) {
		auto inst = prog[i_inst];
		switch(inst.type) {
		case IT_PROCESO: {
			auto &impl = getImpl<IT_PROCESO>(inst);
			if (impl.principal) { 
				g_code.start->lpre = _kw(KW_ALGORITMO) + " "; 
				g_code.start->SetLabel(impl.nombre); 
			} else { 
				g_code.start->lpre = _kw(KW_SUBALGORITMO) + " "; 
				std::string proto = (impl.ret_id.empty() ? "" : (impl.ret_id + " <- ")) + impl.nombre + impl.args;
				g_code.start->SetLabel(proto); 
			}
			_new_this(g_code.start); /*cur_proc=g_code.start;*/ start_done=true;
		} break;
		case IT_ENTONCES:
			aux_end = aux;
			if (inst.type==IT_FINPROCESO and getImpl<IT_FINPROCESO>(inst).principal) 
				_new_none();
			else
				_new_prev();
		break;
		case IT_INVOCAR: {
			auto &impl = getImpl<IT_INVOCAR>(inst);
			aux = Add(children_stack,aux,new Entity(ET_ASIGNAR,impl.nombre+impl.args));
			aux->variante = true;
			_new_this(aux);
		} break;
		case IT_COMMENT: {
			auto &impl = getImpl<IT_COMMENT>(inst);
			if (aux && aux->type==ET_SEGUN) {
				// no puedo poner comentarios a las opciones del segun... por ahora el 
				// parche es bajarlos hasta despues de la opcion, o sea ponerlos dentro
				// de su rama, para al menos no perderlos
				int j = i_inst+1;
				while (inst.loc.instruccion==1) ++j;
				auto opcion = prog[j];
				prog.Erase(j);
				prog.Insert(i_inst,opcion);
				--i_inst;
			} else {
				if (inst.loc.instruccion!=1) {
					if (aux && aux->type==ET_SEGUN) continue;
					aux=Add(children_stack,aux,new Entity(ET_COMENTARIO,impl.text));
					aux->variante=true;
				} else {
					aux=Add(children_stack,aux,new Entity(ET_COMENTARIO,impl.text));
				}
				if (not start_done) { g_code.start->UnLink(); aux->LinkNext(g_code.start); aux = g_code.start; }
			}
		} break;
		case IT_ESCRIBIR: {
			auto &impl = getImpl<IT_ESCRIBIR>(inst);
			aux = Add(children_stack,aux,new Entity(ET_ESCRIBIR,join(impl.expresiones)));
			aux->variante = not impl.saltar;
			_new_this(aux);
		} break;
		case IT_LEER: {
			auto &impl = getImpl<IT_LEER>(inst);
			aux=Add(children_stack,aux,new Entity(ET_LEER,join(impl.variables)));
			_new_this(aux);
		} break;
		case IT_HASTAQUE: {
			auto &impl = getImpl<IT_HASTAQUE>(inst);
			aux = Up(children_stack,aux); aux->SetLabel(impl.condicion,false);
			aux->variante = impl.mientras_que;
			_new_this(aux);
		} break;
		case IT_MIENTRAS: {
			auto &impl = getImpl<IT_MIENTRAS>(inst);
			aux = Add(children_stack,aux,new Entity(ET_MIENTRAS,impl.condicion),0);
			_new_this(aux);
		} break;
		case IT_REPETIR: {
			aux = Add(children_stack,aux,new Entity(ET_REPETIR,""),0);
			_new_prev();
		} break;
		case IT_PARA: {
			auto &impl = getImpl<IT_PARA>(inst);
			aux = Add(children_stack,aux,new Entity(ET_PARA,impl.contador),0);
			aux->GetChild(1)->SetLabel(impl.val_ini);
			aux->GetChild(2)->SetLabel(impl.paso);
			aux->GetChild(3)->SetLabel(impl.val_fin);
			aux->variante = false;
			_new_this(aux);
		} break;
		case IT_PARACADA: {
			auto &impl = getImpl<IT_PARACADA>(inst);
			aux=Add(children_stack,aux,new Entity(ET_PARA,impl.identificador),0);
			aux->GetChild(2)->SetLabel(impl.arreglo);
			aux->variante = true;
			_new_this(aux);
		} break;
		case IT_SI: {
			auto &impl = getImpl<IT_SI>(inst);
			aux = Add(children_stack,aux,new Entity(ET_SI,impl.condicion),1);
			_new_this(aux);
		} break;
		case IT_SINO: {
			aux = aux->GetParent(); children_stack.pop(); children_stack.push(0);
			_new_prev();
		} break;
		case IT_SEGUN: {
			auto &impl = getImpl<IT_SEGUN>(inst);
			aux=Add(children_stack,aux,new Entity(ET_SEGUN,impl.expresion),0);
			_new_this(aux);
		} break;
		case IT_OPCION: case IT_DEOTROMODO: {
			std::string str = inst.type==IT_DEOTROMODO ? _kw(KW_DEOTROMODO) : join(getImpl<IT_OPCION>(inst).expresiones);
			if (aux->type!=ET_SEGUN or children_stack.top()==-1) { // si no es el 1er hijo (no esta despues del segun)
				aux=Up(children_stack,aux); // sube a la opcion
				aux=Up(children_stack,aux); // sube al segun
			}
			aux = Add(children_stack,aux,new Entity(ET_OPCION,str),0);
			_new_this(aux);
		} break;
		case IT_FINPARA: case IT_FINSI: case IT_FINMIENTRAS: case IT_FINSEGUN: {
			// En el caso normal, del segun cuelgan como hijos las opciones, y las opciones
			// tienen como 1ros hijos a otras instrucciones... Entonces en ese caso, cuando
			// termina el segun hay que subir a la opcion, y luego al segun... eso hace el 
			// if que sigue.... Pero cuando el segun está vacío, llegamos acá con aux apuntando
			// al segun, no a una opción ni a una instrucción dentro de una opcion... Pero con
			// ver que aux sea un segun no alcanza, cuando hay dos segun anidados, y los dos
			// finsegun seguidos, luego del 1er fin segun aux queda en el 2do segun (el que
			// le corresponde a ese finsegun), y entonces cuando venga el 2do finsegun sí
			// hay que subir, para eso se mira children_stack
			if (inst.type==IT_FINSEGUN && (aux->type!=ET_SEGUN or children_stack.top()==-1)) { aux=Up(children_stack,aux); aux=Up(children_stack,aux); }
			aux = Up(children_stack,aux);
			_new_prev();
		} break;
		default: {
			std::string str;
			switch (inst.type) {
			case IT_BORRARPANTALLA: str = _kw(KW_LIMPIARPANTALLA); break;
			case IT_ESPERARTECLA: str = _kw(KW_ESPERARTECLA); break;
			case IT_ESPERAR: {
				auto &impl = getImpl<IT_ESPERAR>(inst);
				str = _kw(KW_ESPERARTIEMPO) + " " + impl.tiempo + " " + _kw(impl.factor==1?KW_MILISEGUNDOS:KW_SEGUNDOS); 
			} break;
			case IT_DEFINIR: {
				str = _kw(KW_DEFINIR) + " " + join(getImpl<IT_DEFINIR>(inst).variables) + " " + _kw(KW_COMO) + " ";
				auto &impl = getImpl<IT_DEFINIR>(inst);
				if      (impl.tipo==vt_caracter) str += _kw(KW_TIPO_CARACTER);
				else if (impl.tipo==vt_logica)   str += _kw(KW_TIPO_LOGICO);
				else { _expects(impl.tipo==vt_numerica);
					if (impl.tipo.rounded)       str += _kw(KW_TIPO_ENTERO);
					else                         str += _kw(KW_TIPO_REAL);
				}
			} break;
			case IT_DIMENSION: {
				auto &impl = getImpl<IT_DIMENSION>(inst);
				str = _kw(KW_DIMENSIONAR) + " ";
				for (size_t i=0; i<impl.nombres.size(); ++i) { 
					if (i) str += ", ";
					str += impl.nombres[i]+"("+impl.tamanios[i]+")";
				}
			} break;
			case IT_ASIGNAR: {
				auto &impl = getImpl<IT_ASIGNAR>(inst);
				str = impl.variable + "<-" + impl.valor;
			} break;
			}
			aux = Add(children_stack,aux,new Entity(ET_ASIGNAR,str));
			_new_this(aux);
		} break;
		} // main switch
	}
	Entity *efin = new Entity(ET_PROCESO,""); 
	efin->variante = true;
	efin->lpre = std::string("Fin")+g_code.start->lpre.substr(0,g_code.start->lpre.size()-1);
	efin->SetLabel("");
	aux->LinkNext(efin);
}

bool Load(std::string filename) {
	if (filename.empty()) filename = g_state.fname;
	else g_state.fname = filename;
	
	Programa prog = LoadPrograma(filename);
	if (prog.GetInstCount()==0) { New(); return false; }
	
	g_state.loading = true;
	// separar por procesos/subprocesos
	int imain=0;
	int i0 = 0; // posición en el vector donde empieza el proceso
	for (size_t i_inst=0; i_inst<prog.GetInstCount(); ++i_inst) {
		if (prog[i_inst].type==IT_PROCESO) {
			auto &impl = getImpl<IT_PROCESO>(prog[i_inst]);
			if (impl.principal) imain = g_code.procesos.size();
			
			Entity::AllSet(g_code.start=nullptr);
			LoadProc(prog,i0); i_inst = i0++;
			for(Entity &entity : AllEntities()) {
				ReemplazarOperadores(entity.label);
				entity.EditLabel(0);
			}
			g_code.procesos.push_back(g_code.start);
		}
		/// @todo: se pierden los comentarios que haya despues del ultimo proceso
	}
	SetProc(g_code.procesos[imain]);
	g_state.loading = g_state.modified = false;
#ifndef _FOR_EXPORT
	if (g_code.procesos.size()>1?2:0) g_process_selector->MakeCurrent();
#endif
	return true;
}

bool Save(std::string filename) {
	if (filename.empty()) filename = g_state.fname;
	else g_state.fname = filename;
	
	std::ofstream fout(filename);
	if (!fout.is_open()) return false;
	g_code.code2draw.clear(); g_state.debug_current = nullptr;
	int line=1;
	for(unsigned int i=0;i<g_code.procesos.size();i++) {
		g_code.procesos[i]->GetTopEntity()->Print(fout,"",g_code.procesos[i],line);
		if (i+1!=g_code.procesos.size()) { fout<<std::endl; line++; }
	}
	fout.close();
	g_state.modified = false;
	return true;
}

// inicializa las estructuras de datos con un algoritmo en blanco
void CreateEmptyProc(std::string type) {
	Entity::AllSet(nullptr);
	g_code.start = new Entity(ET_PROCESO,"");
	Entity *aux = new Entity(ET_PROCESO,"");
	aux->variante=true;
	g_code.start->LinkNext(aux);
	g_code.start->lpre=type+" "; g_code.start->SetLabel("SinTitulo");
	aux->lpre = std::string("Fin")+type; aux->SetLabel("");
	Entity::CalculateAll();
	g_code.procesos.push_back(g_code.start);
}

void New() {
	g_state.fname = "temp.psd"; 
	CreateEmptyProc(_kw(KW_ALGORITMO));
	ProcessMenu(MO_ZOOM_EXTEND);
	g_state.modified = false;
}

void SetProc(Entity *proc) {
	Entity::AllSet(g_code.start=proc);
	Entity::CalculateAll(true);
	ProcessMenu(MO_ZOOM_EXTEND);
	Entity::AllIterator it = Entity::AllBegin();
	while (it!=Entity::AllEnd()) {
		it->d_h=it->d_w=it->d_bh=it->d_bwl=it->d_bwr=it->d_fx=it->d_fy=it->d_x=it->d_y=0;
		++it;
	}
}
