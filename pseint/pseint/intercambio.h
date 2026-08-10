#ifndef INTERCAMBIO_H
#define INTERCAMBIO_H

#include <string>
#include <vector>
#include "Code.h"
#include "debug.h"
#include "RunTime.hpp"

#ifdef USE_ZOCKETS
#include "zockets.h"
#endif

#ifdef __WIN32__
	#include<windows.h>
#else
	#include<unistd.h>
	#define Sleep(x) usleep((x)*1000)
#endif

class Ejecutar;

// *********************** Intercambio ****************************

// estructura auxiliar para guardar datos del backtrace (llamadas a funciones)
struct FrameInfo {
	std::string func_name;
	CodeLocation loc;
	FrameInfo(std::string name, CodeLocation _loc):func_name(name),loc(_loc){}
};

class Intercambio {
	
	int backtraceLevel; // en que nivel del trazado inverso se encuentra el punto actual de ejecucion (1:proceso principal, >1:alguna funcion)
	int debugLevel; // solo interesa depurar si debugLevel<=backtraceLevel (si es 0 depura todo)
	std::vector<FrameInfo> backtrace;
	
	RunTime *m_runtime= nullptr;
	bool running = false;
	CodeLocation loc, prev_loc;        // Numero de linea e instruccion que se está ejecutando (base 1)
	std::vector <std::string> Archivo; // Archivo original
	std::vector <std::string> Errores; // Descripcion de los errores encontrados
	std::vector <int> Lineas;     // Numeros de lines correspondientes a los errores	
	

#ifdef USE_ZOCKETS
	
	// para pasar info cuando hay errores al evaluar, pero es una expresion para depuracion
	bool evaluating_for_debug, is_evaluation_error;
	std::string evaluation_error;
	
	std::vector <bool> autoevaluaciones_valid; // expresiones para el depurador que se muestran automaticamente en cada paso
	std::vector <std::string> autoevaluaciones; // expresiones para el depurador que se muestran automaticamente en cada paso
	ZOCKET zocket;
	
	int port; // nro de puerto para comunicarse con wxPSeInt
#endif
	
	std::string sbuffer;
	int delay; // indica el retardo entre instruccion e instruccion para el pasa a paso continuo
	bool do_continue; // indica si debe continuar la ejecucion  o pausarse
	bool do_one_step; // si do_continue, indica si continuar solo un paso (en cuyo caso do_continue volvera a ser falso)
public:
	bool subtitles_on; // si estamos en paso a paso explicado
	
public:
	
	// Linea que se esta ejecutando actualmente
	Intercambio();
	~Intercambio();
	void UnInit();
#ifdef USE_ZOCKETS
	void ProcData(std::string order);
	void ProcInput();
	void InitDebug(RunTime &runtime, int _delay); // si _delay!=0 inicializa la ejecución paso a paso enviando el hello y esperando la primer instruccion
	void SetPort(int p);
#endif
	void SetLocation(CodeLocation loc); // define cual es la instruccion que se va a ejecutar a continuacion
	void SendPositionToGUI(); // avisa a la gui en que instruccion va
	void SendIOPositionToTerminal(int argNumber); // avisa a la terminal en que instruccion va
//	void SendLoopPositionToTerminal(); // avisa a la terminal en que instruccion va
	void SendErrorPositionToTerminal(); // avisa a la terminal en que instruccion va
	void ChatWithGUI(); // espera respuesta de la gui para avanzar
	void SendSubtitle(std::string _str); // envia el texto para el subtitulo a la gui
	
	void SetStarted();
	void SetFinished(bool interrupted=false);
	CodeLocation GetLocation() const { return loc; }
	CodeLocation GetPreviousLocation() const { return prev_loc; }
//	int GetLineNumber() const { return lineNumber; }
//	int GetInstNumber() const { return instNumber; }
	bool IsRunning() const { return running; }
	
	// Manejo de errores
	int Errores_Size();
	void AddError(std::string s, int n);
	std::string GetErrorDesc(int x);
	int GetErrorLine(int x);
	
	bool EvaluatingForDebug();
	void SetError(std::string error);
	
	void OnFunctionIn(std::string nom);
	void OnFunctionOut();
	void OnAboutToEndFunction();
	int GetBacktraceLevel();
	FrameInfo GetFrame(int level);
	
};

extern Intercambio Inter;        // clase para enviar informacion de depuración al editor

// *****************************************************************

#endif
