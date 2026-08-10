#include <string>
#include <iostream>
#include "Ejecutar.hpp"
#include "RunTime.hpp"
#include "global.h"
#include "intercambio.h"
#include "utils.h"
#include "new_memoria.h"
#include "zcurlib.h"
#include "FuncsManager.hpp"
#include "Evaluar.hpp"
#include "strFuncs.hpp"
using namespace std;

// para la ejecucion explicada
#define _sub_msg(i,s) { Inter.SetLocation(rt.prog[i].loc); if (Inter.subtitles_on) { Inter.SendPositionToGUI(); Inter.SendSubtitle(s); } }
#define _sub_wait()   { if (Inter.subtitles_on) Inter.ChatWithGUI(); }
#define _sub(i,s)     { Inter.SetLocation(rt.prog[i].loc); if ( Inter.subtitles_on) { Inter.SendPositionToGUI(); Inter.SendSubtitle(s); Inter.ChatWithGUI(); } }
#define _pos(i)       { Inter.SetLocation(rt.prog[i].loc); if (!Inter.subtitles_on) { Inter.SendPositionToGUI();                        Inter.ChatWithGUI(); } }
#define _sub_raise()  { if (Inter.subtitles_on && for_pseint_terminal) { cout<<"\033[zr"; } }

// ********************* Ejecutar un Bloque de Instrucciones **************************
// Ejecuta desde linestart+1 hasta lineend inclusive, o hasta finproceso/finsubproceso si lineend es -1.
// Las variables aux?, tmp? y tipo quedaron del código viejo, se reutilizan para diferentes
// cosas, por lo que habría que analizarlas y cambiarlas por varias otras variables con scope y 
// nombres mas claros... por ahora cambie las obvias y reduje el scope de las que quedaron, pero falta...
void Ejecutar(RunTime &rt, int LineStart, int LineEnd) {
	
	Programa &programa = rt.prog;
	ErrorHandler &err_handler = rt.err;
	// variables auxiliares
	// Ejecutar el bloque
	int line=LineStart-1;
	while (true) {
		line++;
		if (LineEnd!=-1 && line>LineEnd) break; 
		
		/*const */auto &inst = programa[line];
		
		switch (inst.type) {
			case IT_FINPROCESO: {
				const auto &inst_impl = getImpl<IT_FINPROCESO>(inst);
				Inter.OnAboutToEndFunction();
				_pos(line);
				if (inst_impl.principal) {
					_sub(line,"Finaliza el algoritmo");
				} else {
					_sub(line,string("Se sale del subproceso ")+getImpl<IT_FINPROCESO>(inst).nombre);
					Inter.OnFunctionOut();
				}
			} return;
			case IT_PROCESO: {
				const auto &inst_impl = getImpl<IT_PROCESO>(inst);
				Inter.OnFunctionIn(inst_impl.nombre);
				_pos(line);
				_sub(line,string(inst_impl.principal?"El algoritmo comienza con el proceso ":"Se ingresa en el subproceso ")+inst_impl.nombre);
			} break;
			case IT_BORRARPANTALLA: {
				_pos(line);
				if (for_test) cout<<"***LimpiarPantalla***"<<endl; else { clrscr(); gotoXY(1,1); }
				_sub(line,"Se borra la pantalla");
			} break;
			case IT_ESPERARTECLA: {
				_pos(line);
				_sub_msg(line,"Se espera a que el usuario presione una tecla.");
				_sub_raise();
				if (for_test) cout<<"***EsperarTecla***"<<endl; else getKey();
				_sub_wait();
			} break;
			case IT_INVOCAR: {
				_pos(line);
				const auto &inst_impl = getImpl<IT_INVOCAR>(inst);
				tipo_var tipo=vt_desconocido;
				_sub(line,string("Se va a invocar al subproceso")+inst_impl.nombre);
				EvaluarFuncion(rt,inst_impl.nombre,inst_impl.args,tipo,false);
			} break;

			// ----------- ESCRIBIR ------------ //
			case IT_ESCRIBIR: {
				_pos(line);
				const auto &inst_impl = getImpl<IT_ESCRIBIR>(inst);
				// Separar parametros
				for(size_t i_expr=0;i_expr<inst_impl.expresiones.size();++i_expr) {
					const string &expression = inst_impl.expresiones[i_expr];
					if (colored_output) setForeColor(COLOR_OUTPUT);
					if (with_io_references) Inter.SendIOPositionToTerminal(i_expr+1);
					_sub(line,string("Se evalúa la expresion: ")+expression);
					DataValue res = Evaluar(rt,expression);
					if (res.IsOk()) {
						string ans = res.GetForUser(); fixwincharset(ans);
						cout<< ans <<flush; // Si es variable, muestra el contenido
						_sub(line,string("Se muestra en pantalla el resultado: ")+res.GetForUser());
					}
				}
				if (inst_impl.saltar) cout<<endl; else cout<<flush;
			} break;
			
			// ------------- LEER --------------- //
			case IT_LEER: {
				_pos(line);
				const auto &inst_impl = getImpl<IT_LEER>(inst);
				for(size_t i_var = 0; i_var<inst_impl.variables.size(); ++i_var) {
					string variable = inst_impl.variables[i_var];  // es copia por CheckDims
					
					if (lang[LS_FORCE_DEFINE_VARS] && !memoria->EstaDefinida(variable)) {
						err_handler.ExecutionError(208,"Variable no definida ("+variable+").");
					}
					tipo_var tipo=memoria->LeerTipo(variable);
					const int *dims=memoria->LeerDims(variable);
					size_t pp=variable.find("(");
					if (dims && pp==string::npos)
						err_handler.ExecutionError(200,"Faltan subindices para el arreglo ("+variable+").");
					else if (!dims && pp!=string::npos)
						err_handler.ExecutionError(201,"La variable ("+variable.substr(0,pp)+") no es un arreglo.");
					if (dims) {
						_sub(line,string("Se analizan las dimensiones de ")+variable);
						CheckDims(rt,variable);
						_sub(line,string("El resultado es ")+variable);
					}
					if (tipo.read_only)
						err_handler.ExecutionError(322,string("No se puede modificar la variable ")+variable);
					
					if (with_io_references) Inter.SendIOPositionToTerminal(i_var+1);
					if (colored_output) setForeColor(COLOR_INFO);
					cout<<"> "<<flush;
					if (colored_output) setForeColor(COLOR_INPUT);
					// Leer dato
					_sub_msg(line,"Se espera a que el usuario ingrese un valor y presiones enter."); // tipo?
					_sub_raise();
					
					string aux1;
					if (!predef_input.empty() || noinput) {
						if (predef_input.empty()) err_handler.ExecutionError(214,"Sin entradas disponibles.");
						aux1=predef_input.front(); predef_input.pop(); cout<<aux1<<endl;
						_sub_wait();
					} else {
						aux1=getLine();  
						if (for_eval) {
							if (aux1=="<{[END_OF_INPUT]}>") {
								cout << "<<No hay más datos de entrada>>" << endl; exit(0);
							}
							cout<<aux1<<endl; // la entrada en psEval es un stream separado de la salida, entonces la reproducimos alli para que la salida contenga todo el "dialogo"
						}
					}
					
					fixwincharset(aux1,true); // "descorrige" para que al corregir no traiga problemas
					
					string auxup=ToUpper(aux1);
					if (auxup=="VERDADERO" || auxup=="FALSO") aux1=auxup;
					
					if (tipo==vt_logica && aux1.size()==1 && (toupper(aux1[0])=='F'||aux1[0]=='0')) aux1=FALSO;
					if (tipo==vt_logica && aux1.size()==1 && (toupper(aux1[0])=='V'||aux1[0]=='1')) aux1=VERDADERO;
					tipo_var tipo2 = GuestTipo(aux1);
					if (!tipo.set(tipo2)) 
						err_handler.ExecutionError(120,string("No coinciden los tipos (")+variable+").");
					else if (tipo==vt_numerica_entera && tipo.rounded && aux1.find(".",0)!=string::npos)
						err_handler.ExecutionError(313,string("No coinciden los tipos (")+variable+"), el valor ingresado debe ser un entero.");
					if (Inter.subtitles_on) {
						string name = variable; 
						for (char &c:name) {
							if (c=='(') c='[';
							if (c==')') c=']';
						}
						_sub(line,string("El valor ingresado se almacena en ")+name);
					}
					memoria->DefinirTipo(variable,tipo);
					if (memoria->LeerTipo(variable)==vt_numerica) {
						if (TooManyDigits(aux1))
							err_handler.RunTimeWarning(329,"Posible pérdida de precisión (demasiados dígitos)");
					}
					DataValue dv(tipo,aux1);
					memoria->EscribirValor(variable,dv);
				}
			} break;
			
			// ------------- DIMENSION/REDIMENSION --------------- //
			case IT_DIMENSION: {
				_pos(line);
				const auto &inst_impl = getImpl<IT_DIMENSION>(inst);
				for(size_t i=0;i<inst_impl.nombres.size();++i) { 
					const string &nombre = inst_impl.nombres[i];
					const string tamanios = inst_impl.tamanios[i];
					
					// Separar indices
					auto dims_str = splitArgsList(tamanios); // expresiones de las dimensiones
					int *dims = new int[dims_str.size()+1]; dims[0] = dims_str.size(); // arreglo para las dimensiones
					if (lang[LS_ALLOW_DINAMYC_DIMENSIONS]) { _sub(line,string("Se evalúan las expresiones para cada dimensión del arreglo ")+nombre); }
					for(size_t i=0;i<dims_str.size();i++) {
						DataValue index = Evaluar(rt,dims_str[i]);
						dims[i+1] = index.GetAsInt();
						if (not index.CanBeReal()) {
							err_handler.ExecutionError(122,"No coinciden los tipos.");
						} else if (not IsInteger(index.GetAsReal())) {
							err_handler.ExecutionError(331,"Las dimensiones solo pueden ser números enteros.");
						} else if (dims[i+1]<0) {
							err_handler.ExecutionError(274,"Las dimensiones no pueden ser negativas.");
						} else if (dims[i+1]==0 and (not lang[LS_ALLOW_RESIZE_ARRAYS])) {
							err_handler.ExecutionError(274,"Las dimensiones no pueden ser 0.");
						}
					}
					if (inst_impl.redimension) {
						const int *old_dims = memoria->LeerDims(nombre);
						if (!old_dims) 
							err_handler.ExecutionError(327,nombre+" no estaba previamente dimensionado.");
						else if (dims[0]!=old_dims[0])
							err_handler.ExecutionError(328,"La nueva cantidad de dimensiones de "+nombre+"("+std::to_string(dims[0])+") no coincide con la previa("+std::to_string(old_dims[0])+").");
						else 
							memoria->RedimensionarArreglo(nombre, dims);
					} else { 
						if (memoria->HaSidoUsada(nombre)||memoria->LeerDims(nombre))
							err_handler.ExecutionError(123,"Identificador en uso.");
						else
							memoria->AgregarArreglo(nombre, dims);
					}
					if (Inter.subtitles_on) {
						string aux;
						for(int i=1;i<=dims[0];i++) aux+="x"+IntToStr(dims[i]);
						aux[0]=' ';
						if (inst_impl.redimension) {
							_sub(line,"El arreglo "+nombre+" cambia de tamaño, a"+aux+" elementos");
						} else {
							_sub(line,"Se crea el arreglo "+nombre+" de"+aux+" elementos");
						}
					}
				}
			} break;
			
			// ------------- DEFINICION --------------- //
			case IT_DEFINIR: {
				_pos(line);
				const auto &inst_impl = getImpl<IT_DEFINIR>(inst);
				for(const string &var : inst_impl.variables) {
					if (memoria->EstaDefinida(var) || memoria->EstaInicializada(var)) 
						err_handler.ExecutionError(124,string("La variable (")+var+") ya estaba definida.");
					memoria->DefinirTipo(var,inst_impl.tipo,inst_impl.tipo.rounded);
					if (inst_impl.tipo==vt_numerica) {
						if (inst_impl.tipo.rounded) {
							_sub(line,string("Se define el tipo de la variable \"")+var+"\" como Numérico(Entero).");
						} else {
							_sub(line,string("Se define el tipo de la variable \"")+var+"\" como Numérico(Real).");
						}
					} else if (inst_impl.tipo==vt_caracter) {
						_sub(line,string("Se define el tipo de la variable \"")+var+"\" como Caracter/Cadena de Caracteres.");
					} else if (inst_impl.tipo==vt_logica) {
						_sub(line,string("Se define el tipo de la variable \"")+var+"\" como Lógico.");
					} 
				}
			} break;
			
			// ------------- ESPERAR un tiempo --------------- //
			case IT_ESPERAR: {
				_pos(line);
				const auto &inst_impl = getImpl<IT_ESPERAR>(inst);
				string tiempo = inst_impl.tiempo; 
				int factor = inst_impl.factor;
				_sub(line,string("Se evalúa la cantidad de tiempo: ")+tiempo);
				DataValue time = Evaluar(rt,tiempo);
				if (!time.CanBeReal()) err_handler.ExecutionError(219,string("La longitud del intervalo debe ser numérica."));
				else {
					_sub(line,string("Se esperan ")+time.GetForUser()+(factor==1?" milisengudos":" segundos"));
					if (for_test) cout<<"***Esperar"<<time.GetAsInt()*factor<<"***"<<endl;
					else if (!Inter.subtitles_on) Sleep(time.GetAsInt()*factor);
				}
			} break;
			
			// ------------- ASIGNACION --------------- //
			case IT_ASIGNAR: {
				_pos(line);
				const auto &inst_impl = getImpl<IT_ASIGNAR>(inst);
				string var = inst_impl.variable; // es copia por CheckDims
				const string &valor = inst_impl.valor;
				if (lang[LS_FORCE_DEFINE_VARS] && !memoria->EstaDefinida(var)) {
					err_handler.ExecutionError(211,string("La variable (")+var+") no esta definida.");
				}
				// verificar indices si es arreglo
				if (memoria->LeerDims(var)) {
					if (var.find("(",0)==string::npos)
						err_handler.ExecutionError(200,"Faltan subindices para el arreglo ("+var+").");
					else
						CheckDims(rt,var);
				} else if (var.find("(",0)!=string::npos) {
					err_handler.ExecutionError(201,"La variable ("+var.substr(0,var.find("(",0))+") no es un arreglo.");
				}
				// evaluar expresion
				_sub(line,string("Se evalúa la expresion a asignar: ")+valor);
				DataValue result = Evaluar(rt,valor);
				// comprobar tipos
				tipo_var tipo_aux1 = memoria->LeerTipo(var);
				if (!tipo_aux1.can_be(result.type))
					err_handler.ExecutionError(125,"No coinciden los tipos.");
				if (tipo_aux1.read_only)
					err_handler.ExecutionError(322,string("No se puede modificar la variable ")+var);
				else if (tipo_aux1==vt_numerica_entera && tipo_aux1.rounded && result.GetAsInt()!=result.GetAsReal())
					err_handler.ExecutionError(314,"No coinciden los tipos, el valor a asignar debe ser un entero.");
				_sub(line,string("El resultado es: ")+result.GetForUser());
				// escribir en memoria
				if (Inter.subtitles_on and valor!=var) { 
					string name = var; 
					for (char &c:name) {
						if (c=='(') c='[';
						if (c==')') c=']';
					}
					_sub(line,string("El resultado se guarda en ")+name);
				}
				result.type.rounded=false; // no forzar a entera la variable en la que se asigna
				memoria->DefinirTipo(var,result.type);
				memoria->EscribirValor(var,result);
			} break;
			
			// ---------------- SI ------------------ //
			case IT_SI: {
				const auto &inst_impl = getImpl<IT_SI>(inst);
				_pos(line);
				_sub(line,string("Se evalúa la condición para Si-Entonces: ")+inst_impl.condicion);
				tipo_var tipo;
				bool condition_is_true = Evaluar(rt,inst_impl.condicion,vt_logica).GetAsBool();
				if (tipo!=vt_error) {
					// hasta donde llega el bucle
					int line_sino=inst_impl.sino, line_finsi=inst_impl.fin; 
					_expects(line_sino==-1 or programa[line_sino]==IT_SINO);
					_expects(line_finsi!=-1 and programa[line_finsi]==IT_FINSI);
					// ejecutar lo que corresponda
					if (condition_is_true) {
						_sub(line+1,"El resultado es Verdadero, se sigue por la rama del Entonces");
						if (line_sino==-1) line_sino=line_finsi;
						Ejecutar(rt,line+2,line_sino-1); // ejecutar salida por verdadero
					} else {
						if (line_sino!=-1) {
							line = line_sino;
							_pos(line);
							_sub(line,"El resultado es Falso, se sigue por la rama del SiNo");
							Ejecutar(rt,line+1,line_finsi-1); // ejecutar salida por falso
						} else {
							_sub(line,"El resultado es Falso, no se hace nada");
						}
					}
					// marcar la salida
					line=line_finsi;
					_pos(line);
					_sub(line,"Se sale de la estructura Si-Entonces");
				} else {
					err_handler.ExecutionError(275,"No coinciden los tipos.");
				}
			} break;
			
			// ---------------- MIENTRAS ------------------ //
			case IT_MIENTRAS: {
				_pos(line);
				const auto &inst_impl = getImpl<IT_MIENTRAS>(inst);
				const string &condicion = inst_impl.condicion;
				_sub(line,string("Se evalúa la condición para Mientras: ")+condicion);
				tipo_var tipo;
				bool condition_is_true = Evaluar(rt,condicion,vt_logica).GetAsBool();
				if (tipo!=vt_error) {
					int line_finmientras = inst_impl.fin;
					while (condition_is_true) {
						_sub(line,"La condición es Verdadera, se iniciará una iteración.");
						Ejecutar(rt,line+1,line_finmientras-1);
						_pos(line);
						_sub(line,string("Se evalúa nuevamente la condición: ")+condicion);
						condition_is_true = Evaluar(rt,condicion,vt_logica).GetAsBool();
					}
					line=line_finmientras;
					_pos(line);
					_sub(line,"La condición es Falsa, se sale de la estructura Mientras.");
				}
			} break;
			
			// ---------------- REPETIR HASTA QUE ------------------ //
			case IT_REPETIR: {
				_pos(line);
				const auto &inst_impl = getImpl<IT_REPETIR>(inst);
				int line_hastaque = inst_impl.fin;
				// cortar condicion de cierre
				const auto &hasta_impl = getImpl<IT_HASTAQUE>(programa[line_hastaque]);
				const string &condicion = hasta_impl.condicion;
				bool valor_verdad = hasta_impl.mientras_que;
				_sub(line,"Se ejecutarán las acciones contenidas en la estructura Repetir");
				tipo_var tipo;
				bool should_continue_iterating=true;
				while (should_continue_iterating) {
					Ejecutar(rt,line+1,line_hastaque-1);
					// evaluar condicion y seguir
					_pos(line_hastaque);
					_sub(line_hastaque,string("Se evalúa la condición: ")+condicion);
					should_continue_iterating = Evaluar(rt,condicion,vt_logica).GetAsBool()==valor_verdad;
					if (should_continue_iterating)
						_sub(line_hastaque,string("La condición es ")+(valor_verdad?VERDADERO:FALSO)+", se contiúa iterando.");
				} while (should_continue_iterating);
				line=line_hastaque;
				_sub(line_hastaque,string("La condición es ")+(valor_verdad?FALSO:VERDADERO)+", se sale de la estructura Repetir.");
			} break;
			
			// ------------------- PARA --------------------- //
			case IT_PARA: {
				const auto &inst_impl = getImpl<IT_PARA>(inst);
				_pos(line);
				const string &contador = inst_impl.contador;
				memoria->DefinirTipo(contador,vt_numerica);
				if (lang[LS_PROTECT_FOR_COUNTER]) memoria->SetearSoloLectura(contador,true);
				
				const string &expr_ini = inst_impl.val_ini;
				_sub(line,string("Se evalúa la expresion para el valor inicial: ")+expr_ini);
				DataValue res_ini = Evaluar(rt,expr_ini,vt_numerica);
				if (!res_ini.CanBeReal()) err_handler.ExecutionError(126,"No coinciden los tipos."); /// @todo: parece que esto no es posible, salta antes adentro del evaluar
				
				bool positivo; // para saber si es positivo o negativo
				DataValue res_paso(vt_numerica,"1"), res_fin;
				if (inst_impl.paso.empty()) { // si no hay paso adivinar
					res_fin = Evaluar(rt,inst_impl.val_fin,vt_numerica);
					if (lang[LS_DEDUCE_NEGATIVE_FOR_STEP] && res_ini.GetAsReal()>res_fin.GetAsReal()) {
						_sub(line,"Se determina que el paso será -1.");
						positivo=false; res_paso.SetFromInt(-1);
					} else {
						_sub(line,"Se determina que el paso será +1.");
						positivo=true; res_paso.SetFromInt(1);
					}
				} else { // si hay paso tomar ese
					const string &expr_fin = inst_impl.val_fin;
					res_fin = Evaluar(rt,expr_fin,vt_numerica);
					const string &expr_paso = inst_impl.paso;
					_sub(line,string("Se evalúa la expresion para el paso: ")+expr_paso);
					res_paso = Evaluar(rt,expr_paso,vt_numerica);
					positivo = res_paso.GetAsReal()>=0;
				}
				
				int line_finpara = inst_impl.fin;
				
				_sub(line,string("Se inicializar el contador ")+contador+" en "+res_ini.GetForUser());
				memoria->EscribirValor(contador,res_ini); // inicializa el contador
				string comp=positivo?"<=":">=";
				do {
					/// @todo: cuando memoria maneje DataValues usar el valor del contador directamente desde ahi en lugar de evaluar
					_sub(line,string("Se compara el contador con el valor final: ")+contador+"<="+res_fin.GetForUser());
					DataValue res_cont = Evaluar(rt,contador,vt_numerica);
					if ( positivo ? (res_cont.GetAsReal()>res_fin.GetAsReal()) : (res_cont.GetAsReal()<res_fin.GetAsReal()) ) break;
					_sub(line,"La expresión fue Verdadera, se iniciará una iteración.");
					Ejecutar(rt,line+1,line_finpara-1);
					_pos(line);
					res_cont = Evaluar(rt,contador,vt_numerica); // pueden haber cambiado a para el contador!!!
					DataValue new_val = DataValue::MakeReal(res_cont.GetAsReal()+res_paso.GetAsReal());
					memoria->EscribirValor(contador,new_val);
					_sub(line,string("Se actualiza el contador, ahora ")+contador+" vale "+new_val.GetAsString()+".");
				} while(true);
				if (lang[LS_PROTECT_FOR_COUNTER]) {
					memoria->SetearSoloLectura(contador,false);
					memoria->Desinicializar(contador);
				}
				line=line_finpara;
				_pos(line);
				_sub(line,"Se sale de la estructura repetitiva Para.");
			} break;
			
			// ------------------- PARA CADA --------------------- //
			case IT_PARACADA: {
				const auto &inst_impl = getImpl<IT_PARACADA>(inst);
				bool primer_iteracion=true; _pos(line);
				const string &identificador = inst_impl.identificador;
				const string &arreglo = inst_impl.arreglo;
				
				int line_finpara = inst_impl.fin;
				
				const int *dims=memoria->LeerDims(arreglo);
				if (!dims) err_handler.ExecutionError(276,"La variable ("+arreglo+") no es un arreglo.");
				int nelems=1; // cantidad total de iteraciones
				for (int i=1;i<=dims[0];i++) nelems*=dims[i];
				
				// bucle posta
				_sub(line,string("El arreglo \"")+arreglo+"\" contiene "+IntToStr(nelems)+" elementos. Se comienza a iterar por ellos.");
				for (int i=0;i<nelems;i++) {
					// armar expresion del elemento (ej: A[1])
					string elemento=")";
					int naux=1;
					for (int j=dims[0];j>0;j--) {
						elemento=string(",")+IntToStr((lang[LS_BASE_ZERO_ARRAYS]?0:1)+((i/naux)%dims[j]))+elemento;
						naux*=dims[j];
					}
					elemento=arreglo+"("+elemento.substr(1);
					// asignar el elemento en la variable del bucle
					if (primer_iteracion) primer_iteracion=false; else { _pos(line); }
					_sub(line,identificador+" será equivalente a "+elemento+" en esta iteración.");
					if (!memoria->DefinirTipo(identificador,memoria->LeerTipo(elemento)))
						err_handler.ExecutionError(277,"No coinciden los tipos.");
					memoria->EscribirValor(identificador,memoria->LeerValor(elemento));
					// ejecutar la iteracion
					Ejecutar(rt,line+1,line_finpara-1);
					// asignar la variable del bucle en el elemento
					memoria->DefinirTipo(elemento,memoria->LeerTipo(identificador));
					memoria->EscribirValor(elemento,memoria->LeerValor(identificador));
				}
				memoria->Desinicializar(identificador);
				line=line_finpara; // linea del finpara
				_pos(line);
				_sub(line,"Se sale de la estructura repetitiva Para Cada.");
				
			} break;
			
			// ------------------- SEGUN --------------------- //
			case IT_SEGUN: {
				const auto &inst_impl = getImpl<IT_SEGUN>(inst);
				const string &expr_control = inst_impl.expresion;
				tipo_var tipo_master=vt_caracter_o_numerica;
				_pos(line);
				_sub(line,string("Se evalúa la expresion: ")+expr_control);
				DataValue val_control = Evaluar(rt,expr_control,tipo_master); // evaluar para verificar el tipo
				if (!val_control.CanBeReal()&&(lang[LS_INTEGER_ONLY_SWITCH]||!val_control.CanBeString())) {
					if (!lang[LS_INTEGER_ONLY_SWITCH]) 
						err_handler.ExecutionError(205,"La expresión del SEGUN debe ser de tipo numerica o caracter.");
					else
						err_handler.ExecutionError(206,"La expresión del SEGUN debe ser numerica.");
				}
				_sub(line,string("El resultado es: ")+val_control.GetForUser());
				int line_finsegun=inst_impl.fin; 
				
				// analizar las opciones y comparar
				int i_opcion_correcta = -1, i_de_otro_modo = -1; // guardo el indice en opciones y no la linea, porque eso dice donde empieza, y el siguiente en opciones sería donde termina
				for(size_t i=0;i_opcion_correcta==-1 && i<inst_impl.opciones.size();++i) {
					int line_opcion = inst_impl.opciones[i];
					if (programa[line_opcion]==IT_OPCION) {
						const auto &posibles_valores = getImpl<IT_OPCION>(programa[line_opcion]).expresiones;
						for(const std::string &expr_opcion : posibles_valores) {
							_pos(line_opcion);
							_sub(line_opcion,string("Se evalúa la opcion: ")+expr_opcion);
							DataValue val_opcion = Evaluar(rt,expr_opcion,tipo_master);
							if (!val_opcion.CanBeReal()&&(lang[LS_INTEGER_ONLY_SWITCH]||!val_opcion.CanBeString())) err_handler.ExecutionError(127,"No coinciden los tipos.");
							// evaluar la condicion (se pone como estaban y no los resultados de la evaluaciones de antes porque sino las variables indefinida pueden no tomar el valor que corresponde
							if (Evaluar(rt,string("(")+expr_control+")=("+expr_opcion+")").GetAsBool()) {
								_sub(line_opcion,"El resultado coincide, se ingresará en esta opción.");
								i_opcion_correcta = i; break;
							} else {
								_sub(line_opcion,string("El resultado no coincide: ")+val_opcion.GetForUser());
							}
						}
					} else {
						_expects(programa[line_opcion]==IT_DEOTROMODO);
						i_de_otro_modo = i;
					}
				}
				
				if (i_opcion_correcta==-1) {
					if (i_de_otro_modo!=-1) {
						i_opcion_correcta = i_de_otro_modo;
						int line_dom = inst_impl.opciones[i_de_otro_modo];
						_pos(line_dom);
						_sub(line_dom,"Se ingresará en la opción De Otro Modo");
					}
				}
				if (i_opcion_correcta==-1) {
					_sub(line_finsegun,string("Ninguna de las opciones coincide. No se hace nada."));	
				} else {
					int line_from = inst_impl.opciones[i_opcion_correcta]+1;
					int line_to = i_opcion_correcta+1<inst_impl.opciones.size() 
						          ? (inst_impl.opciones[i_opcion_correcta+1]-1)
								  : (line_finsegun-1);
					Ejecutar(rt,line_from,line_to);
				}
				
				line=line_finsegun;
				_pos(line);
				_sub(line,"Se sale de la estructura Segun.	");
			} break;
			
			// ya deberíamos haber cubierto todas las opciones
			default:
				err_handler.ExecutionError(0,"Ha ocurrido un error interno en PSeInt.");
		} // switch 
	} // while
}
