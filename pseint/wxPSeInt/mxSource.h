#ifndef MXSOURCE_H
#define MXSOURCE_H
#include <wx/stc/stc.h>
#include <vector>
#include <wx/timer.h>
#include "RTSyntaxManager.h"
#include <set>
#include "../pseint/debug.h"
class mxInputDialog;
class mxProcess;
class wxTimer;
class wxSocketBase;
class er_source_register;

#define mxSOURCE_BOLD 1<<0
#define mxSOURCE_ITALIC 1<<1
#define mxSOURCE_UNDERL 1<<2
#define mxSOURCE_HIDDEN 1<<3
#define mxSOURCE_SMALLER 1<<4

enum {BT_NONE,BT_PARA,BT_SEGUN,BT_CASO,BT_REPETIR,BT_MIENTRAS,BT_SI,BT_SINO,BT_PROCESO,BT_SUBPROCESO,BT_FUNCION,BT_ALGORITMO,BT_SUBALGORITMO};

class mxSource : public wxStyledTextCtrl {
private:
	wxString m_main_process_title = "sin_titulo"; // por ahora, solo para sugerir el nombre al guardar por 1ra vez
	wxString temp_filename_prefix; // ruta y nombre de los temporales para este algoritmo (sin extension, ver GetTempFilename*)
	
	bool rt_running; // rt_syntax||highlight_blocks||show_vars
	bool mask_timers; // para evitar lanzar el timer en la modificacion que hace SaveTemp
	wxTimer *rt_timer; // se activa al cargar el pseudocodigo y al modificarlo, para llamar al rt_syntax
	wxTimer *reload_timer; // se activa al modificar el algoritmo si se estaba ejecutando en psterm, para mandar a reejecutar
	wxTimer *flow_timer; // se activa al recibir el foco con el editor de diagramas abierto para pedir el pseudocódigo actualizado
#ifdef _AUTOINDENT
	vector<bool> to_indent;
	int to_indent_first_line;
	wxTimer *indent_timer; // se activa al recibir el foco con el editor de diagramas abierto para pedir el pseudocódigo actualizado
#endif
	int comp_from, comp_to;
	int last_s1,last_s2;
	bool is_example;
	bool just_created;
	
	static int last_id;
	int id; // id unico e irrepetible para cada source, se usa para pasarle a los procesos externos y que digan al llamar al socket a que 

	wxSocketBase *flow_socket; // si esta siendo editado como diagrama de flujo, guarda el socket con el que se comunica con el editor, sino NULL
	wxSocketBase *run_socket; // si esta siendo ejecutado en un psterm, guarda el socket con el que se comunica con la terminal, sino NULL
	
	int debug_line, debug_line_handler_1, debug_line_handler_2;
	wxString page_text;
	int status; // estado actual para este fuente
	bool status_should_change; // para no cambiar ciertos estados hasta que no se modifique el pseudocódigo
	
public:
	mxInputDialog *input; // entrada predefinida para ese proceso, o NULL si no la hay
	bool sin_titulo;
	wxString filename;
	void SetFileName(wxString afilename);
	wxString GetFileName() const;
	void SetStyling(bool colour=true);
	void SetWords();
	static void SetAutocompletion();
	void SetStyle(int idx, const char *foreground, const char *background, int fontStyle);
	void SetIndics(int from, int len, int indic, bool on);
	void SetFieldIndicator(int p1, int p2, bool select=true); // para los campos a completar en las plantillas de instrucciones/estructuras
	void UnExample();
	void SetExample();
	void SetJustCreated();
	bool IsJustCreated() const { return just_created; }
	mxSource(wxWindow *parent, wxString ptext, wxString afilename=wxEmptyString);
	~mxSource();
	
	void OnZoomChange(wxStyledTextEvent &evt);
	void ZoomToSelection();
	
	void OnEditCut(wxCommandEvent &evt);
	void OnEditCopy(wxCommandEvent &evt);
	void OnEditPaste(wxCommandEvent &evt);
	void OnEditUndo(wxCommandEvent &evt);
	void OnEditRedo(wxCommandEvent &evt);
	void OnEditComment(wxCommandEvent &evt);
	void OnEditUnComment(wxCommandEvent &evt);
	void OnEditDuplicate(wxCommandEvent &evt);
	void OnEditDelete(wxCommandEvent &evt);
	void OnEditSelectAll(wxCommandEvent &evt);
	void OnEditToggleLinesUp (wxCommandEvent &event);
	void OnEditToggleLinesDown (wxCommandEvent &event);
//	void OnModify(wxStyledTextEvent &event);
	void OnCalltipClick(wxStyledTextEvent &event);
	void OnModifyOnRO(wxStyledTextEvent &event);
	void SetKeyWords(int num, const wxString &list);
	wxString m_selected_variable; int m_selected_variable_line_from=-1, m_selected_variable_line_to=-1;
	wxArrayString m_keywords, m_functions;
	const std::vector<int> &MapCharactersToPositions(int line, const wxString &text);
	void StyleLine(int line);
	void OnStyleNeeded(wxStyledTextEvent &event);
private:
	void MakeCompletionFromKeywords(wxArrayString &output, int start_pos, const wxString &typed);
	void MakeCompletionFromIdentifiers(wxArrayString &output, int start_pos, const wxString &typed);
public:
	void OnCharAdded(wxStyledTextEvent &event);
	void OnKeyDown(wxKeyEvent &evt);
	void OnUserListSelection (wxStyledTextEvent &event);
	void OnUpdateUI(wxStyledTextEvent &event);
private: 
	int brace_1, brace_2; //< auxiliares para OnPainted
	void MyBraceHighLight (int b1=wxSTC_INVALID_POSITION, int b2=wxSTC_INVALID_POSITION);  //< auxiliares para OnPainted
public:
	void OnPainted (wxStyledTextEvent & event);
	void SetModified(bool);
	void MessageReadOnly();
	
	int GetIndent(int line);
	int GetIndentLevel(int l, bool goup, int &btype, bool diff_proc_sub_func=false);
	void Indent(int l1, int l2);
	bool IndentLine(int l, bool goup=true);
	void OnEditIndentSelection(wxCommandEvent &evt);
//	void OnEditBeautifyCode(wxCommandEvent &evt);

	int GetId();
	
	void SetRunSocket(wxSocketBase *s);
	bool UpdateRunningTerminal(bool raise=false, bool ignore_rt=false);
	void KillRunningTerminal();
	void SetFlowSocket(wxSocketBase *s);
	wxSocketBase *GetFlowSocket();
	void ReloadFromTempPSD(bool check_syntax);
	bool HaveComments();
	
	void SetDebugLine(int l=-1, int i=-1); // para marcar donde va el paso a paso, -1 para desmarcar
	void SetDebugPause(); // cambia de marcador usando la ultima linea que recibio en SetDebugLine
	
	bool LineHasSomething(int l); // false si esta vacia o tiene solo comentarios
	
	wxString GetInstruction(int p); // devuelve la instrucción (primer palabra de la sentencia) a la que pertenece esa posición
	
	void SetPageText(wxString ptext);
	wxString GetPageText();
	void OnSavePointReached(wxStyledTextEvent &evt);
	void OnSavePointLeft(wxStyledTextEvent &evt);
	
	wxString SaveTemp(); // guarda el fuente actual en un archivo temporal (para pasarle al interprete)
	wxString GetTempFilenamePSC(); // nombre de archivo temporal para el pseudocodigo
	wxString GetTempFilenameOUT(); // nombre de archivo temporal para los resultados
	wxString GetTempFilenamePSD(); // nombre de archivo temporal para el diagrama de flujo
	
	// retorna las posiciones donde empieza y termina cada instruccion de una linea
	std::vector<int> &FillAuxInstr(int line);
	void SelectInstruccion(int line, int inst);
	void SelectLineAndCol(int line, int col0, int col1=1);
	
private:
	struct rt_result_t {
		struct rt_line_t { // hay uno de estos por linea de codigo
			wxString msg; // aqui se concatenan todos los mensajes de error de esa linea
			int code = -1; // numero de error de uno de los de la linea (o -1 si no hay)
			bool HasError() const { return code!=-1; }
			void Clear() { msg.clear(); code = -1; }
			void Add(int inst_num, int err_code, const wxString err_msg) {
				if (code!=-1) { msg<<"\n["<<inst_num+1<<"] "<<err_msg; }
				else          { msg<<  "["<<inst_num+1<<"] "<<err_msg; code=err_code; }
			}
		};
		std::vector<rt_line_t> lines;
		int errors_count = 0;
		void Clear() { for(rt_line_t &l : lines) l.Clear(); errors_count = 0; }
		void Add(int line_num, int inst_num, int err_code, const wxString err_msg, bool is_error=true) {
			if (line_num>=lines.size()) lines.resize(line_num+1);
			lines[line_num].Add(inst_num, err_code, err_msg);
			if (is_error) ++errors_count;
		}
		bool HasError(int line_num) const { return lines.size()>line_num and lines[line_num].HasError(); }
		const rt_line_t &GetError(int line_num) const { _expects(line_num<lines.size()); return lines[line_num]; }
		bool IsOk() const { return errors_count==0; }
		int GetLinesCount() const { return lines.size(); }
	} rt_results;
public:
	void DoRealTimeSyntax(std::function<void()> &&action_post={});
	void ClearErrorData();
	void ClearErrorMarks();
	void MarkError(wxString line);
	void MarkError(int l, int i, int n, wxString str, bool special=false);
	
	void StartRTSyntaxChecking(); // habilita la verificacion de sintaxis en tiempo real para este fuente (coloca rt_running en true y lanza la primer verificacion)
	void DoRTSyntaxChecking(); // marca que se debe rehacer la verificacion (inicia el timer, pero solo si esta habilitada, segun rt_running)
	void StopRTSyntaxChecking(); // deshabilita la verificacion de sintaxis en tiempo real para este fuente (coloca rt_running en false, limpia las marcas y detiene el timer)
	void SetMainProcessTitleFromRTSM(wxString title) { m_main_process_title = title; }
	
	void OnTimer(wxTimerEvent &te); // event dispatcher para los timers, mira que timer es e invoca al metodo que le corresponda
	void OnChange(wxStyledTextEvent &event);
#ifdef _AUTOINDENT
	void OnModified(wxStyledTextEvent &event);
#endif
	
	wxString GetCurrentKeyword (int pos=-1); // auxiliar para OnPopupMenu y otros
	void OnPopupMenu(wxMouseEvent &evt);
	void PopupMenu(wxMouseEvent &evt);
	
	void AddOneToDesktopTest(wxCommandEvent &evt);
	void OnRenameVar(wxCommandEvent &evt);
	void OnDefineVar(wxCommandEvent &evt);
	void RenameVar(int where, wxString var_name, int line_from=-1, int line_to=-1);
	void DefineVar(int where, wxString var_name, int line_from=-1, int type=-1);
	
	bool IsEmptyLine(int line); // auxiliar para DefineVar
	bool IsDimOrDef(int line); // auxiliar para DefineVar
	bool IsProcOrSub(int line); // auxiliar para OnPopupMenu
		
	
	void OnMarginClick(wxStyledTextEvent &event);
	
	void RTOuputStarts();
	void RTOuputEnds();

	static void SetCalltips();
	
	struct current_calltip_info {
		int pos;
		bool is_error;
		current_calltip_info() : pos(-1), is_error(false) { }
	};
	current_calltip_info current_calltip;
	void ShowCalltip(int pos, const wxString &l, bool is_error=false);
 	void HideCalltip(bool if_is_error=true,bool if_is_not_error=true);
	void ShowRealTimeError(int pos, const wxString &l);
	
	void OnToolTipTime (wxStyledTextEvent &event);
	void OnToolTipTimeOut (wxStyledTextEvent &event);
	
	void TryToAutoCloseSomething(int l);
	
	void HighLight(wxString words, int from=-1, int to=-1);
	
	wxArrayInt blocks; // blocks[l1]=l2 guarda un bloque que va de l1 a l2
	wxArrayInt blocks_reverse; // blocks_reverse[l1]=l2 guarda un bloque que va de l2 a l1
	wxArrayInt blocks_markers; // arreglo de handlers de los markers insertados para resaltar un bloque
	void ClearBlocks(); // borra las listas de bloques (blocks y blocks_reverse)
	void AddBlock(int l1, int l2); // registra un bloque que va desde l1 a l2 en blocks y blocks_reverse, lo llama el rt_syntax)
	void UnHighLightBlock(); // borra el resaltado
	void HighLightBlock(); // resalta el bloque de la linea actual si es que hay y el rt_syntax no tiene trabajo pendiente
	
	int GetStatus();
	void SetStatus(int cual=-1);
	
	void StopReloadTimer();
	
	wxString GetPathForExport();
	wxString GetNameForExport();
	
	void ProfileChanged();
	
	void DebugMode(bool on);
	
	void OnSetFocus(wxFocusEvent &evt);
	void UpdateFromFlow();
	
	void OnMouseWheel(wxMouseEvent &event);
	void OnClick(wxMouseEvent &evt);
	
	bool IsExample() { return is_example; }
	
	bool LoadFile(const wxString &fname);
	bool SaveFile(const wxString &fname);
	
	void ToUnicodeOpers(int line);
	void ToRegularOpers(wxString &s);
	void FixExtraUnicode(wxString &s);
	void Analyze();
	void Analyze(int line);
	void Analyze(int line_from, int line_to);
	
	void ShowUserList(wxArrayString &arr, int p1, int p2);
	
	void FocusKilled();
	
	er_source_register *er_register;
	
	DECLARE_EVENT_TABLE();
};

#endif


