#ifndef MXMAINWINDOW_H
#define MXMAINWINDOW_H
#include <wx/frame.h>
#include <wx/aui/aui.h>
#include "mxSource.h"
#include <wx/treebase.h>
#include <wx/treectrl.h>
#include <wx/socket.h>
#include <wx/html/htmlwin.h>

class mxHtmlWindow;
class mxFindDialog;
class wxStaticText;
class wxScrollBar;
class mxDesktopTestPanel;
class mxTestPanel;

class mxMainWindow : public wxFrame {
	
public:
	
	class QuickHelpPanelPolicy {
		friend class mxMainWindow;
		enum QUICKHELP_MODE { 
			QHM_RT_RESULT, // iarg=as bool (1/0)
			QHM_RT_ERROR, // iargs=error num
			QHM_HELP,  // help _PAGE/_TEXT
			QHM_TEST,  // enuncidao de ejercicio autocontenido
			QHM_OUTPUT, // error list, or runtime error
			QHM_NULL 
		} m_mode;
		bool m_visible;
		mxHtmlWindow *m_ctrl;
		int m_last_code;
		wxAuiManager *m_aui;
	public:
		QuickHelpPanelPolicy() : m_mode(QHM_NULL), m_visible(false), m_ctrl(NULL), m_last_code(-1), m_aui(NULL) {}
		void ShowRTResult(bool errors, bool force=false);
		void ShowRTError(int code, wxString msg, bool force=false);
		void ShowHelpPage(wxString help_file);
		void ShowHelpText(wxString html_text);
		void ShowTestHelp(wxString html_text);
		void HideTestHelp();
		void ShowOutput(wxString html_text);
		void HideOutput();
		void HideErrors();
		void Hide();
		bool IsVisible() const { return m_visible; }
		void EnsureVisible();
	} m_quick_help;
	QuickHelpPanelPolicy &QuickHelp() { return m_quick_help; }
	
	void EnableDebugButton(bool enable);
	
private:
	friend class DebugManager;
	mxFindDialog *find_replace_dialog = nullptr;
	friend class mxSource; // para el page_text
	friend class mxProcess;
	friend class mxFindDialog;
	int last_proc;
	wxAuiManager aui_manager;
	wxToolBar *toolbar;
	wxPanel *commands;
	void CreateDesktopTestPanel();
	void CreateVarsPanel();
	void CreateOpersPanel();
	void CreateMenus();
	void CreateToolbars();
	void CreateNotebook();
	void CreateResultsTree();
	void CreateQuickHelp();
	void CreateButtonSubProceso(wxPanel *panel, wxSizer *sizer); ///< helper function for CreateCommandsPanel y ProfileChanged
	void CreateCommandsPanel();
	void CreateDebugControlsPanel();
	void CreateStatusBar();
	wxAuiNotebook *notebook = nullptr; 
	wxButton *button_subproc = nullptr;
	
	mxTestPanel *test_panel = nullptr;
	
	bool m_fullscreen = false;
	
public:
	
	mxSource *NewProgram(const wxString &title="<sin_titulo>");
	mxSource *OpenProgram(wxString path, bool is_example=false);
	mxSource *OpenTestPackage(const wxString &path);
	mxTestPanel *GetTestPanel() { return test_panel; }
	void CloseTestPackage();
	void OnSourceClose(mxSource *src);
	
	mxMainWindow(wxPoint pos, wxSize size);
	mxSource *last_source = nullptr;
	~mxMainWindow();
	
	void RegenFileMenu(wxString path);
	
	void OnFileNew(wxCommandEvent &evt);
	void OnFileOpen(wxCommandEvent &evt);
	void OnFileSave(wxCommandEvent &evt);
	void OnFileSaveAs(wxCommandEvent &evt);
	void OnFilePrint(wxCommandEvent &event);
	void OnFileEditFlow(wxCommandEvent &evt);
	void OnFileExportHtml(wxCommandEvent &evt);
	void OnFileExportLang(wxCommandEvent &evt);
	void OnFileExportPreview(wxCommandEvent &evt);
	void OnFileClose(wxCommandEvent &evt);
	void OnFileExit(wxCommandEvent &evt);
	void OnFileSourceHistory (wxCommandEvent &event);
	void OnEdit(wxCommandEvent &evt);
	void OnEditFind(wxCommandEvent &evt);
	void OnEditFindNext(wxCommandEvent &evt);
	void OnEditFindPrev(wxCommandEvent &evt);
	void OnEditReplace(wxCommandEvent &evt);
	void OnRunRun(wxCommandEvent &evt);
	void OnRunStepStep(wxCommandEvent &evt);
	void OnRunSubtitles(wxCommandEvent &evt);
	void OnRunCheck(wxCommandEvent &evt);
//	void OnRunDrawFlow(wxCommandEvent &evt);
	void OnRunSaveFlow(wxCommandEvent &evt);
	void OnRunSetInput(wxCommandEvent &evt);
	void OnHelpAbout(wxCommandEvent &evt);
	void OnHelpIndex(wxCommandEvent &evt);
	void OnHelpQuickHelp(wxCommandEvent &evt);
	void OnHelpExamples(wxCommandEvent &evt);
//	void OnHelpLogger (wxCommandEvent &event);
	void OnHelpUpdates (wxCommandEvent &event);

	void OnViewNotebookNext (wxCommandEvent &event);
	void OnViewNotebookPrev (wxCommandEvent &event);
	void OnViewZoomToSelection(wxCommandEvent &event);
	
	void OnConfigCalltipHelps(wxCommandEvent &evt);
	void OnConfigAutoComp(wxCommandEvent &evt);
	void OnConfigHighlightBlocks(wxCommandEvent &evt);
	void OnConfigAutoClose(wxCommandEvent &evt);
	void OnConfigShowQuickHelp(wxCommandEvent &evt);
	void OnConfigAnimateGui(wxCommandEvent &evt);
	void OnConfigReorganizeForDebug(wxCommandEvent &evt);
	void OnConfigUseColors(wxCommandEvent &evt);
	void OnConfigUnicodeOpers(wxCommandEvent &evt);
	void OnConfigSelectFonts(wxCommandEvent &evt);
	void OnConfigBigIcons(wxCommandEvent &evt);
	void OnConfigUseDarkTheme(wxCommandEvent &evt);
	void OnConfigFullScreen(wxCommandEvent &evt);
	void OnConfigUseDarkPSTerm(wxCommandEvent &evt);
	void OnConfigUseDarkPSDraw(wxCommandEvent &evt);
	void OnConfigPSDrawNoCrop(wxCommandEvent &evt);
	void OnConfigShowShapeColors(wxCommandEvent &evt);
	void OnConfigNassiScheiderman(wxCommandEvent &evt);
	void OnConfigLanguage(wxCommandEvent &evt);
	void OnConfigRealTimeSyntax(wxCommandEvent &evt);
	void OnConfigRealTimeAnnotate(wxCommandEvent &evt);
	void OnConfigSmartIndent(wxCommandEvent &evt);
	void OnInconInstaller(wxCommandEvent &evt);
	
	void OnDoThat(wxCommandEvent &evt);
	void OnDebugShortcut(wxCommandEvent &evt);
	
	void OnSocketEvent(wxSocketEvent &event);
	void OnScrollDegugSpeed(wxScrollEvent &evt);
	
	wxMenuItem *mi_autocomp, *mi_autoclose, *mi_highlight_blocks, *mi_quickhelp, *mi_smart_indent,
		//*mi_debug_panel, *mi_toolbar, *mi_commands, *mi_vars_panel
		*mi_shape_colors, *mi_psdraw_nocrop, *mi_calltip_helps, *mi_rt_syntax, *mi_rt_annotate, 
		*mi_nassi_shne, *mi_use_colors, *mi_unicode_opers, *mi_reorganize_for_debug, *mi_animate_gui, 
		*mi_use_dark_psterm, *mi_use_dark_psdraw, *mi_big_icons, *mi_use_dark_theme, *mi_fullscreen;
	
	wxMenu *file_menu;
	wxMenuItem *file_history[5];
	
	void OnClose(wxCloseEvent &evt);
	void OnSelectError(wxTreeEvent &evt);
	void SelectError(wxString text);
//	void HideQuickHelp();

	void OnCmdEscribir(wxCommandEvent &evt);
	void OnCmdLeer(wxCommandEvent &evt);
	void OnCmdAsignar(wxCommandEvent &evt);
	void OnCmdPara(wxCommandEvent &evt);
	void OnCmdMientras(wxCommandEvent &evt);
	void OnCmdRepetir(wxCommandEvent &evt);
	void OnCmdSi(wxCommandEvent &evt);
	void OnCmdSegun(wxCommandEvent &evt);
	void OnCmdSubProceso(wxCommandEvent &evt);
	void InsertCode(wxString toins);
	void InsertCode(wxArrayString &toins);

	void OnPaneClose(wxAuiManagerEvent& event);
	void OnNotebookPageChange(wxAuiNotebookEvent& event);
	void OnNotebookPageClose(wxAuiNotebookEvent& event);
	
//	const wxArrayString &GetDesktopVars();
//	void SetDesktopVars(bool do_dt, const wxArrayString &vars);

	void SelectLine(mxSource *src, int line);
	
	bool SelectFirstError();
	
	void SetAccelerators();
	
	void OnLink (wxHtmlLinkEvent &event);
		
	mxSource *FindSourceById(int id);
	mxSource *FindFlowSocket(wxObject *s);
	void SelectSource(mxSource *s);
	
	void ProfileChanged(); // lo llama mxConfig para que actualice el menu configurar y reinicie el rtsyntax
	void UpdateRealTimeSyntax(); // lo llama rtsyntax despues de reiniciarse
	void OnRTSyntaxAuxTimer(wxTimerEvent &event);
	
private:
	void (*m_lambda_func)();
	wxTimer *m_lambda_timer;
public:
	void SetLambda(void (*lamda_func)());
	void OnLambdaTimer(wxTimerEvent &event);
	
	mxSource *GetCurrentSource();
	
	void OnHelperOpers(wxCommandEvent &evt);
	void OnHelperVars(wxCommandEvent &evt);
	void OnHelperDebug(wxCommandEvent &evt);
	void OnHelperCommands(wxCommandEvent &evt);
	
	void CheckIfNeedsRTS();
	
	void ShowOpersPanel(bool show, bool anim=false);
	void ShowVarsPanel(bool show, bool anim=false);
	void ShowDebugPanel(bool show, bool anim=false);
	void ShowCommandsPanel(bool show, bool anim=false);
	void ShowResults(bool show, bool no_error);
	void ShowQuickHelp(bool show);
	void ShowSubtitles(bool show, bool anim=false);
	void ShowBacktrace(bool show);
	void ShowDesktopTestPanel(bool show, bool anim=false);
	
	void ParseResults(mxSource *source); // analiza el archivo de salida que genera una ejecucion, que contiene los errores de la misma
	
	void RunCurrent(bool raise, bool from_psdraw=false); // ejecuta o re-ejecuta el source actual (si ya estaba corriendo en un psterm, raise indica si debe pasar al frente esa ventana)
	
	void ReorganizeForDebugging();
	
	// para los paneles laterales
	void ShowPanel(wxString helper, wxWindow *panel, bool anim=true);
	void HidePanel(wxString helper, wxWindow *panel, bool anim=true);
	// para los paneles inferiores
	void ShowPanel(wxWindow *panel, bool anim=true);
	void HidePanel(wxWindow *panel, bool anim=true);
	
private:
	bool result_tree_done;
	wxTreeItemId results_root, results_last;
	wxTreeCtrl *results_tree_ctrl; // si no hay rt_syntax mejor no usar el result_tree y poner todo en el...
	wxString results_tree_text; // ...si no se usa el resulttree, aca va acumulando el equivalente para mostrarlo en el quick_html
	wxArrayString results_tree_errors; // lista de errores para obtener el texto completo al hacer click en los mismos como links en el quick_html
	int result_tree_text_level; // al ir armando por partes con el RTreeAdd el texto del html, se dejan listas e items incompletos (<OL>,<LI>), esto indica qué
	wxString RTreeAdd_auxHtml(wxString str); // pasa a html un mesaje de error, reemplazando caracteres especiales y convirtiendolo en hyperlink si corresponde
public:
	void RTreeReset();
	enum class RTAddType { TreeRoot, Level1Node, Level2Node, OutOfTree };
	void RTreeAdd(wxString text, RTAddType type, mxSource *source=NULL);
	void RTreeDone(bool show, bool error);
	
	void OnKillFocus(wxFocusEvent &event);
	void OnActivate(wxActivateEvent &event);
	
	void ResetInLogMode();
	
	void UpdatePSDrawSettings(); // cuando psdraw cambia una conf, invoca a esta func para que se refleje en los menues
	
	void SetFullScreen(bool on);
	
	DECLARE_EVENT_TABLE();
	
};

extern mxMainWindow *main_window;

#endif

