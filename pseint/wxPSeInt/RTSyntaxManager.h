#ifndef RTSYNTAXMANAGER_H
#define RTSYNTAXMANAGER_H
#include <wx/process.h>
#include <functional>

class mxSource;
class wxTimer;

/**
* Esta clase gestiona el subproceso del intérprete (pseint.bin) que se utiliza de 
* fondo para analizar la sintaxis (extrayendo tambien la lista de variables y los
* bloques para resaltar en amarillo). Hay una sola instancia, que es privada para
* que se maneja mediante los métodos estáticos, y estos métodos controlan que 
* sea una y siempre que se requiera exista (similar a un singleton). La comunicacion
* con el proceso hijo va por tuberias, y el parseo de la salida lo hace la 
* función ContinueProcessing. mxSource llama a Process(algo) cuando el código 
* cambia y pasa cierto tiempo (controlado por su timer). Process setea todo en 
* cero, envía el código por la tubería y llama por primera vez a ContinueProcessing
* para que analice la respuesta. Si no hay respuesta rapida (el pseudocódigo puede
* ser muy largo, la pc muy lenta, o lo que sea), usa un timer propio para relanzarse
* dentro de poco (el evento lo recibe la mainwindow y lo redirecciona mediante
* el método Process(NULL), el mismo que antes pero con NULL como argumento).
* Esto es así porque el wxYield (la facil sería un bucle con ContinueProcessing 
* y wxYield() adentro) puede traer problemas si justo se quiere ejecutar otra 
* cosa en medio (como el mismísimo algoritmo en la terminal).
**/

class RTSyntaxManager:public wxProcess {
	wxCSConv conv;
	wxTimer *timer;
	static RTSyntaxManager *the_one;
	int pid = -1;
	bool restart = false, running = false, processing = false;
	mxSource *src = nullptr;
	RTSyntaxManager();
	~RTSyntaxManager();
	void ContinueProcessing();
	void OnTerminate(int pid, int status);
	
	enum class Step { None, SendCode, ReadErrors, 
		              ReadVars, ReadBlocks, PostAction };
	Step m_current_step = Step::None;
public:
	std::function<void()> m_action_post;
public:
	
	static void Start();
	static void Restart();
	static void Stop();
	static bool IsLoaded();
	static bool Process(mxSource *src = nullptr, std::function<void()> &&action_post = {});
	static void OnSourceClose(mxSource *src);
};

#endif

