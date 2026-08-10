#ifndef MXCONSOLE_H
#define MXCONSOLE_H
#include <wx/panel.h>
#include <wx/timer.h>
#include <wx/process.h>
#include <wx/scrolbar.h>
#include <vector>
using namespace std;

class mxFrame;

class mxConsole : public wxPanel {
	
public:
	mxFrame *parent; ///< para avisarle de algunos eventos
	wxScrollBar *scroll; ///< barra para moverse en el tiempo (es de la ventana padre, pero la ajusta la consola)

	int fixed_rand_seed; ///< seed argument to add to command in order to obtain same results in every run
	wxString command; ///< the last command runned in this console
	
	wxString m_font_name;
	int m_font_size;
	
	wxString history; ///< todo el texto procesado desde el inicio del proceso
	
	struct history_event {
		int last_clear; ///< valor de last_clear para el evento
		int input_count; ///< valor input_history.size()
		int pos; ///< posicion en history del evento
	};
	vector<history_event> events; ///< eventos a registrar en la scrollbar para viajes en el tiempo
	void MarkEvent();
	int cur_event; ///< ultimo evento que se esta mostrando actualmente, o -1 si va en tiempo real
	void SetTime(int t); ///< elige un evento para que la consola muestre solo hasta ese evento
	
	wxProcess *the_process = nullptr; ///< running process, or NULL if process have ended/not started
	int the_process_pid; ///< [last] running process id
	void KillProcess(); ///< kills current process if any
	void OnProcessTerminate( wxProcessEvent &event ); ///< si el proceso hijo termina, actualiza el estado de la clase
	
	wxTimer *timer_process; ///< timer to extract and process output from current process' output stream
	void OnTimerProcess( wxTimerEvent &event ); ///< llama a GetProcessOutput regularmente, para actualizar la salida
	void GetProcessOutput(bool refresh=true); ///< extract unprocessed text from output stream for current process
	
	int margin = 2; ///< margin in pixels for text inside the console
	float char_w; ///< width in pixels for a char with current fontsize
	int char_h; ///< height in pixels for a char with current fontsize
	int bg; ///< index for current console background color (the same for the whole console)
	struct code_location { ///< linea and instruction number in source pseudocode
		int line, inst/*, sub*/;
		bool is_error;
		code_location():line(-1),inst(-1),is_error(false){}
		bool IsValid() const { return line!=-1&&inst!=-1; }
		bool operator==(const code_location &o) const { return o.line==line&&o.inst==inst/*&&o.sub==sub*/; }
		bool operator!=(const code_location &o) const { return !((*this)==o); }
		void Clear() { line=inst=-1; is_error=false; }
	};
	struct console_char { ///< data for each visible char in the console
		wxChar the_char; ///< the char to show
		int fg; ///< index for forground color
		code_location loc; ///< instruccion en el pseudocódigo de la instruccion que genero esta entrada/salida
		console_char():the_char(' '),fg(0) {}
	};
	console_char *buffer = nullptr; ///< current content for the visible part of the console (size buffer_w*buffer_h), ordered by rows
	int buffer_w = 0; ///< number of columns for current console size
	int buffer_h = 0; ///< number of rows for current console size
	void OnPaint( wxPaintEvent &event ); ///< redraw the console based on buffers' content
	
	int cur_fg; ///< index for current text foreground color, will be used for printing int Print
	int cur_x; ///< current x caret position
	int cur_y; ///< current y caret position
	code_location cur_loc; ///< current instruccion in source pseudo-code (that is generating that input/output)
	
	wxString current_input; ///< the text entered for next/current input (since last time Enter key was presses, or since start), for storing in input_history on next Enter
	struct input_history_entry {
		wxString text; ///< el texto que ingreso el usuario para esta entrada en particular
		code_location loc; ///< posición en el código donde comienza esa entrada
		bool force_user_input; ///< flag que indica que no se debe utilizar el valor del historial, sino que se debe pedir de forma interactiva
		input_history_entry() {}
		input_history_entry(const wxString _text, code_location _loc) : text(_text), loc(_loc), force_user_input(false) {}
	};
	vector<input_history_entry> input_history; ///< array with all inputs since process started, for reapeting input when reloading process
	int input_history_position; ///< auxiliar para saber en un Reload cuales ya se usaron y cuales no de las entradas en input_history
	bool want_input; ///< indica que se espera una entrada (ver wait_input para saber si tecla o linea)
	bool wait_one_key; ///< indica que se espera una tecla cualquiera (que no se debe mostrar en pantalla)
	void OnChar( wxKeyEvent &event ); ///< process console input, and sends it to child process
	void RecordInput(wxString input); ///< guarda une lectura en el historial de entradas (input_history)
	int GetInputPositionFromBufferPosition(int pos);
	
	
	wxFont font; ///< current font
	void OnMouseWheel(wxMouseEvent &evt); ///< scroll y zoom
	
	bool selecting = false; ///< true if the user is selection (moving the mouse wiht left button down)
	int selection_start = -1; ///< one selection limit (only valid if selection_end!=-1)
	int selection_end = -1; ///< the other selection limit (can be less than selection_start), or -1 if there is no selection
	void OnMouseLeftDown(wxMouseEvent &evt); ///< para seleccionar y copiar
	void OnMouseLeftUp(wxMouseEvent &evt); ///< para seleccionar y copiar
	void OnMouseMotion(wxMouseEvent &evt); ///< para seleccionar y copiar
	
	void OnMouseDClick(wxMouseEvent &evt); ///< popup menu for copy/paste
	void OnMouseRightDown(wxMouseEvent &evt); ///< popup menu for copy/paste
	void OnPopupPaste(wxCommandEvent &evt);
	void OnPopupCopy(wxCommandEvent &evt);
	void OnPopupCloseAfterRun(wxCommandEvent &evt);
	void OnPopupStayOnTop(wxCommandEvent &evt);
	void OnPopupReset(wxCommandEvent &evt);
	void OnPopupChangeInput(wxCommandEvent &evt);
	void OnPopupFromHere(wxCommandEvent &evt);
	
	wxTimer *timer_size; ///< timer to reprocess last text after a resize event (regenerates buffer content)
	int last_clear; ///< position in history where last cls even occured (to rewrite the text when resizing the console)
	void CalcResize(); ///< reprocess since last cls event to redraw after a resize event
	void OnTimerSize(wxTimerEvent &event); ///< calls resize when in timer_tize event
	void OnSize( wxSizeEvent &event ); ///< starts timer_size for regenerating buffer after a resize event
	void RebuildBuffer(); ///< recalcula buffer, se usa luego de un resize o de un settime

	
	bool blinking_caret_aux; ///< aux flag for caret blinking animation
	wxTimer *timer_caret; ///< timer to blink caret if caret_visible
	void OnTimerCaret( wxTimerEvent &event ); ///< anima el parpadeo del cursor
	
	int dimmed = 0; /// puede valer 0 o 1, 0 es lo normal, 1 muestra el texto "apagado", se usa para indicar que la salida ya está desactualizada (es int y no bool para poder usarlo de indice para elegir el color)
	bool selection_is_input = false; /// si es true, la selección actual es una entrada, que eventualmente el usuario podría querer editar (se muestra un mensaje indicando esta situación)
	
	mxConsole(mxFrame *parent, wxScrollBar *scroll, bool dark_theme, const wxString &font_name, int font_size);
	
	void Run(wxString command);
	
	void Reload(); ///< recarga el proceso, reejecutandolo completo y volviendo a darle las mismas entradas
	void PlayFromCurrentEvent(); ///< recarga el proceso, reejecutandolo hasta el evento actual, y siguiendo desde ahi en vivo (si historial)
	
	void SetFont();
	void Process(wxString input, bool record=true/*, bool do_print=true*/);
	void Print(wxString text, bool record/*, bool do_print*/);
	void ClearBuffer();
	void GotoXY(int x, int y, bool record);
	void Reset(bool hard=false);
	void Dimm(); ///< pinta el texto "apagado" para indicar que esta salida está desactualizada respecto del algortimo

	void GetSourceLocationFromOutput(int pos); ///< envia una posicion al editor para que muestre que linea genero una entrada/salida

	void SetRandSeed();
	
	void Yield();
	
	DECLARE_EVENT_TABLE();
	
};

#endif

