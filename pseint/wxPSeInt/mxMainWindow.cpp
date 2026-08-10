#include <wx/menu.h>
#include <wx/aui/auibook.h>
#include <wx/toolbar.h>
#include <wx/filename.h>
#include <wx/html/htmlwin.h>
#include <wx/imaglist.h>
#include <wx/artprov.h>
#include <wx/textfile.h>
#include <wx/cmndata.h>
#include "mxUtils.h"
#include "mxMainWindow.h"
#include "ids.h"
#include "ConfigManager.h"
#include "mxProcess.h"
#include "version.h"
#include "HelpManager.h"
#include "mxFindDialog.h"
#include "mxAboutWindow.h"
#include "mxHelpWindow.h"
#include "mxDropTarget.h"
#include "DebugManager.h"
#include "mxDesktopTestPanel.h"
#include "mxEvaluateDialog.h"
#include "mxUpdatesChecker.h"
#include "mxConfig.h"
#include "mxProfile.h"
#include "mxPrintOut.h"

#define IF_THERE_IS_SOURCE if (notebook->GetPageCount()>0)
#define CURRENT_SOURCE ((mxSource*)notebook->GetPage(notebook->GetSelection()))
#include "mxInputDialog.h"
#include "RTSyntaxManager.h"
#include "mxVarWindow.h"
#include "mxDebugWindow.h"
#include "mxSubtitles.h"
#include "mxStatusBar.h"
#include "CommunicationsManager.h"
#include "HtmlExporter.h"
#include "mxOpersWindow.h"
#include "mxPanelHelper.h"
#include "Logger.h"
#include <wx/filedlg.h>
#include <wx/msgdlg.h>
#include <wx/app.h>
#include <wx/textdlg.h>
#include <wx/settings.h>
#include "string_conversions.h"
#include "mxExportPreview.h"
#include "mxIconInstaller.h"
#include "mxTestPanel.h"
#include "mxHtmlWindow.h"
#include "error_recovery.h"
#include "mxFontsConfig.h"
#include <wx/button.h>
#include "mxBacktrace.h"

mxMainWindow *main_window = NULL;

//#define _debug_speed_h 80
//#define _debug_speed_m 55
//#define _debug_speed_l 20

// organizacion de los elementos de la ventana (layer,row,position para el wxAuiPaneInfo, p es el panel, h es el helper(boton para hacerlo visible))
static const int hvar[]={3,0,0};
static const int hopr[]={3,0,1};
static const int hcmd[]={3,0,0};
static const int hdbg[]={3,0,1};
static const int pvar[]={1,1,0}; // LEFT	vars_window
static const int popr[]={1,2,0}; // LEFT	opers_window
static const int pdbg[]={1,1,0}; // RIGHT	debug_panel
static const int pcmd[]={3,1,0}; // RIGHT	commands
static const int prtr[]={2,1,0}; // BOTTOM	results_tree_ctrl
static const int phlp[]={2,1,1}; // BOTTOM	help
static const int pdkt[]={2,1,2}; // BOTTOM	desktop_test_panel
static const int psub[]={2,2,0}; // BOTTOM	subtitles
static const int pbak[]={0,0,3}; // BOTTOM	backtrace
static const int ptlb[]={4,0,0}; // TOP		toolbars
static const int pevl[]={2,0,0}; // BOTTOM	test_panel

BEGIN_EVENT_TABLE(mxMainWindow, wxFrame)
	EVT_CLOSE(mxMainWindow::OnClose)
	EVT_MENU(mxID_FILE_NEW, mxMainWindow::OnFileNew)
	EVT_MENU(mxID_FILE_OPEN, mxMainWindow::OnFileOpen)
	EVT_MENU(mxID_FILE_SAVE, mxMainWindow::OnFileSave)
	EVT_MENU(mxID_FILE_EDIT_FLOW, mxMainWindow::OnFileEditFlow)
	EVT_MENU(mxID_FILE_EXPORT_HTML, mxMainWindow::OnFileExportHtml)
	EVT_MENU_RANGE(mxID_FILE_EXPORT_LANG_FIRST, mxID_FILE_EXPORT_LANG_LAST,mxMainWindow::OnFileExportLang)
	EVT_MENU(mxID_FILE_EXPORT_PNG, mxMainWindow::OnRunSaveFlow)
	EVT_MENU(mxID_FILE_EXPORT_PREVIEW, mxMainWindow::OnFileExportPreview)
	EVT_MENU(mxID_FILE_CLOSE, mxMainWindow::OnFileClose)
	EVT_MENU(mxID_FILE_SAVE_AS, mxMainWindow::OnFileSaveAs)
	EVT_MENU(mxID_FILE_PRINT, mxMainWindow::OnFilePrint)
	EVT_MENU(mxID_FILE_EXIT, mxMainWindow::OnFileExit)
	EVT_MENU(mxID_EDIT_UNDO, mxMainWindow::OnEdit)
	EVT_MENU(mxID_EDIT_REDO, mxMainWindow::OnEdit)
	EVT_MENU(mxID_EDIT_CUT, mxMainWindow::OnEdit)
	EVT_MENU(mxID_EDIT_COPY, mxMainWindow::OnEdit)
	EVT_MENU(mxID_EDIT_PASTE, mxMainWindow::OnEdit)
	EVT_MENU(mxID_EDIT_COMMENT, mxMainWindow::OnEdit)
	EVT_MENU(mxID_EDIT_UNCOMMENT, mxMainWindow::OnEdit)
	EVT_MENU(mxID_EDIT_DUPLICATE, mxMainWindow::OnEdit)
	EVT_MENU(mxID_EDIT_DELETE, mxMainWindow::OnEdit)
	EVT_MENU(mxID_VARS_DEFINE, mxMainWindow::OnEdit)
	EVT_MENU(mxID_VARS_RENAME, mxMainWindow::OnEdit)
	EVT_MENU(mxID_VARS_ADD_ONE_TO_DESKTOP_TEST, mxMainWindow::OnEdit)
	EVT_MENU(mxID_VARS_ADD_ALL_TO_DESKTOP_TEST, mxMainWindow::OnEdit)
	EVT_MENU(mxID_EDIT_FIND, mxMainWindow::OnEditFind)
	EVT_MENU(mxID_EDIT_REPLACE, mxMainWindow::OnEditReplace)
	EVT_MENU(mxID_EDIT_FIND_NEXT, mxMainWindow::OnEditFindNext)
	EVT_MENU(mxID_EDIT_FIND_PREV, mxMainWindow::OnEditFindPrev)
	EVT_MENU(mxID_EDIT_SELECT_ALL, mxMainWindow::OnEdit)
	EVT_MENU(mxID_RUN_RUN, mxMainWindow::OnRunRun)
	EVT_MENU(mxID_RUN_STEP_STEP, mxMainWindow::OnRunStepStep)
	EVT_MENU(mxID_RUN_SUBTITLES, mxMainWindow::OnRunSubtitles)
	EVT_MENU(mxID_RUN_CHECK, mxMainWindow::OnRunCheck)
//	EVT_MENU(mxID_RUN_DRAW_FLOW, mxMainWindow::OnRunDrawFlow)
	EVT_MENU(mxID_RUN_SET_INPUT, mxMainWindow::OnRunSetInput)
	EVT_MENU(mxID_EDIT_TOGGLE_LINES_UP, mxMainWindow::OnEdit)
	EVT_MENU(mxID_EDIT_TOGGLE_LINES_DOWN, mxMainWindow::OnEdit)
	EVT_MENU(mxID_EDIT_INDENT_SELECTION, mxMainWindow::OnEdit)
//	EVT_MENU(mxID_EDIT_BEAUTIFY_CODE, mxMainWindow::OnEdit)
	
	EVT_MENU(mxID_DEBUG_STEP, mxMainWindow::OnDebugShortcut)
	EVT_MENU(mxID_DO_THAT, mxMainWindow::OnDoThat)
	EVT_MENU(mxID_CONFIG_LANGUAGE, mxMainWindow::OnConfigLanguage)
	EVT_MENU(mxID_CONFIG_ANIMATE_GUI, mxMainWindow::OnConfigAnimateGui)
	EVT_MENU(mxID_CONFIG_REORGANIZE_FOR_DEBUG, mxMainWindow::OnConfigReorganizeForDebug)
	EVT_MENU(mxID_CONFIG_PSDRAW_NO_CROP, mxMainWindow::OnConfigPSDrawNoCrop)
	EVT_MENU(mxID_CONFIG_USE_COLORS, mxMainWindow::OnConfigUseColors)
	EVT_MENU(mxID_CONFIG_UNICODE_OPERS, mxMainWindow::OnConfigUnicodeOpers)
	EVT_MENU(mxID_CONFIG_USE_DARK_THEME, mxMainWindow::OnConfigUseDarkTheme)
	EVT_MENU(mxID_CONFIG_FULLSCREEN, mxMainWindow::OnConfigFullScreen)
	EVT_MENU(mxID_CONFIG_BIG_ICONS, mxMainWindow::OnConfigBigIcons)
	EVT_MENU(mxID_CONFIG_USE_DARK_PSTERM, mxMainWindow::OnConfigUseDarkPSTerm)
	EVT_MENU(mxID_CONFIG_USE_DARK_PSDRAW, mxMainWindow::OnConfigUseDarkPSDraw)
	EVT_MENU(mxID_CONFIG_SELECT_FONTS, mxMainWindow::OnConfigSelectFonts)
	EVT_MENU(mxID_CONFIG_SHAPE_COLORS, mxMainWindow::OnConfigShowShapeColors)
	EVT_MENU(mxID_CONFIG_HIGHLIGHT_BLOCKS, mxMainWindow::OnConfigHighlightBlocks)
	EVT_MENU(mxID_CONFIG_AUTOCLOSE, mxMainWindow::OnConfigAutoClose)
	EVT_MENU(mxID_CONFIG_AUTOCOMP, mxMainWindow::OnConfigAutoComp)
	EVT_MENU(mxID_CONFIG_CALLTIP_HELPS, mxMainWindow::OnConfigCalltipHelps)
	EVT_MENU(mxID_CONFIG_SHOW_QUICKHELP, mxMainWindow::OnConfigShowQuickHelp)
	EVT_MENU(mxID_CONFIG_RT_SYNTAX, mxMainWindow::OnConfigRealTimeSyntax)
	EVT_MENU(mxID_CONFIG_RT_ANNOTATE, mxMainWindow::OnConfigRealTimeAnnotate)
	EVT_MENU(mxID_CONFIG_NASSI_SHNEIDERMAN, mxMainWindow::OnConfigNassiScheiderman)
	EVT_MENU(mxID_CONFIG_SMART_INDENT, mxMainWindow::OnConfigSmartIndent)
	EVT_MENU(mxID_CONFIG_ICON_INSTALLER, mxMainWindow::OnInconInstaller)

	EVT_BUTTON(mxID_CMD_SUBPROCESO, mxMainWindow::OnCmdSubProceso)
	EVT_BUTTON(mxID_CMD_ASIGNAR, mxMainWindow::OnCmdAsignar)
	EVT_BUTTON(mxID_CMD_LEER, mxMainWindow::OnCmdLeer)
	EVT_BUTTON(mxID_CMD_ESCRIBIR, mxMainWindow::OnCmdEscribir)
	EVT_BUTTON(mxID_CMD_MIENTRAS, mxMainWindow::OnCmdMientras)
	EVT_BUTTON(mxID_CMD_REPETIR, mxMainWindow::OnCmdRepetir)
	EVT_BUTTON(mxID_CMD_PARA, mxMainWindow::OnCmdPara)
	EVT_BUTTON(mxID_CMD_SI, mxMainWindow::OnCmdSi)
	EVT_BUTTON(mxID_CMD_SEGUN, mxMainWindow::OnCmdSegun)
	EVT_MENU_RANGE(mxID_FILE_SOURCE_HISTORY_0, mxID_FILE_SOURCE_HISTORY_0+5,mxMainWindow::OnFileSourceHistory)
	EVT_MENU(mxID_HELP_INDEX, mxMainWindow::OnHelpIndex)
	EVT_MENU(mxID_HELP_QUICKHELP, mxMainWindow::OnHelpQuickHelp)
	EVT_MENU(mxID_HELP_ABOUT, mxMainWindow::OnHelpAbout)
	EVT_MENU(mxID_HELP_EXAMPLES, mxMainWindow::OnHelpExamples)
	EVT_MENU(mxID_HELP_UPDATES, mxMainWindow::OnHelpUpdates)
//	EVT_MENU(mxID_HELP_LOGGER, mxMainWindow::OnHelpLogger)
	
	EVT_MENU(mxID_VIEW_NOTEBOOK_PREV, mxMainWindow::OnViewNotebookPrev)
	EVT_MENU(mxID_VIEW_NOTEBOOK_NEXT, mxMainWindow::OnViewNotebookNext)
	EVT_MENU(mxID_ZOOM_TO_SELECTION, mxMainWindow::OnViewZoomToSelection)

	EVT_TREE_ITEM_ACTIVATED(wxID_ANY, mxMainWindow::OnSelectError)
	EVT_TREE_SEL_CHANGED(wxID_ANY, mxMainWindow::OnSelectError)
	EVT_AUI_PANE_CLOSE(mxMainWindow::OnPaneClose)
	EVT_AUINOTEBOOK_PAGE_CLOSE(wxID_ANY, mxMainWindow::OnNotebookPageClose)
	EVT_AUINOTEBOOK_PAGE_CHANGED(wxID_ANY, mxMainWindow::OnNotebookPageChange)
	EVT_SOCKET(wxID_ANY,mxMainWindow::OnSocketEvent)
	EVT_COMMAND_SCROLL(wxID_ANY, mxMainWindow::OnScrollDegugSpeed)
	EVT_HTML_LINK_CLICKED(wxID_ANY, mxMainWindow::OnLink)
	
	EVT_BUTTON(mxID_HELPER_VARS,mxMainWindow::OnHelperVars)
	EVT_BUTTON(mxID_HELPER_OPERS,mxMainWindow::OnHelperOpers)
	EVT_BUTTON(mxID_HELPER_DEBUG,mxMainWindow::OnHelperDebug)
	EVT_BUTTON(mxID_HELPER_COMMANDS,mxMainWindow::OnHelperCommands)
	
	EVT_TIMER(mxID_RT_TIMER,mxMainWindow::OnRTSyntaxAuxTimer)
	EVT_TIMER(mxID_LAMBDA_TIMER,mxMainWindow::OnLambdaTimer)
	
	EVT_KILL_FOCUS(mxMainWindow::OnKillFocus)
	EVT_ACTIVATE(mxMainWindow::OnActivate)
END_EVENT_TABLE()

mxMainWindow::mxMainWindow(wxPoint pos, wxSize size)
	: wxFrame(NULL, wxID_ANY, "PSeInt", pos, size, wxDEFAULT_FRAME_STYLE)
{
	m_lambda_func = NULL; m_lambda_timer = new wxTimer(GetEventHandler(),mxID_LAMBDA_TIMER);
	aui_manager.GetArtProvider()->SetColour(wxAUI_DOCKART_BACKGROUND_COLOUR,wxSystemSettings::GetColour(wxSYS_COLOUR_FRAMEBK));
	
	wxTheColourDatabase->AddColour("Z LIGHT GRAY",wxColour(240,240,240));
	wxTheColourDatabase->AddColour("Z LIGHT BLUE",wxColour(240,240,255));
	wxTheColourDatabase->AddColour("Z LIGHT RED",wxColour(255,220,220));
	wxTheColourDatabase->AddColour("DARK BLUE",wxColour(0,0,128));
	wxTheColourDatabase->AddColour("DARK RED",wxColour(128,0,0));
	wxTheColourDatabase->AddColour("DARK GRAY",wxColour(128,128,128));
	wxTheColourDatabase->AddColour("Z DARK GRAY",wxColour(150,150,150));
	
	aui_manager.SetManagedWindow(this);
	
	for (int i=0;i<5;i++)
		file_history[i]=NULL;
	
	wxIconBundle bundle;
	wxIcon icon16; icon16.CopyFromBitmap(wxBitmap("imgs/icon16.png",wxBITMAP_TYPE_PNG)); bundle.AddIcon(icon16);
	wxIcon icon32; icon32.CopyFromBitmap(wxBitmap("imgs/icon32.png",wxBITMAP_TYPE_PNG)); bundle.AddIcon(icon32);
	wxIcon icon48; icon48.CopyFromBitmap(wxBitmap("imgs/icon48.png",wxBITMAP_TYPE_PNG)); bundle.AddIcon(icon48);
	wxIcon icon64; icon64.CopyFromBitmap(wxBitmap("imgs/icon64.png",wxBITMAP_TYPE_PNG)); bundle.AddIcon(icon64);
	wxIcon icon128; icon128.CopyFromBitmap(wxBitmap("imgs/icon128.png",wxBITMAP_TYPE_PNG)); bundle.AddIcon(icon128);
	SetIcons(bundle);
	
	find_replace_dialog = new mxFindDialog(this);
	
	CreateMenus();
	CreateToolbars();
	CreateVarsPanel();
	CreateOpersPanel();
	CreateDebugControlsPanel();
	CreateCommandsPanel();
	CreateDesktopTestPanel();
	CreateNotebook();
	CreateResultsTree();
	CreateQuickHelp();
	CreateStatusBar();
	
	aui_manager.SetFlags(aui_manager.GetFlags() | wxAUI_MGR_TRANSPARENT_DRAG|wxAUI_MGR_LIVE_RESIZE);
	aui_manager.GetArtProvider()->SetMetric(wxAUI_DOCKART_GRADIENT_TYPE,wxAUI_GRADIENT_NONE);
	aui_manager.Update();

	SetDropTarget(new mxDropTarget(NULL));
	SetAccelerators();
	
	debug = new DebugManager();
	
}

void mxMainWindow::OnFileSourceHistory (wxCommandEvent &event) {
	OpenProgram(config->last_files[event.GetId()-mxID_FILE_SOURCE_HISTORY_0]);
}

mxMainWindow::~mxMainWindow() {
	RTSyntaxManager::Stop();
	aui_manager.UnInit();
	if (logger) delete logger;
}

void mxMainWindow::CreateMenus() {
	wxMenuBar *menu = new wxMenuBar;
	
	wxMenu *file = new wxMenu;
	utils->AddItemToMenu(file,mxID_FILE_NEW, _Z("&Nuevo\tCtrl+N"),"","nuevo.png");
	utils->AddItemToMenu(file,mxID_FILE_OPEN, _Z("&Abrir...\tCtrl+O"),"","abrir.png");
	utils->AddItemToMenu(file,mxID_FILE_SAVE, _Z("&Guardar\tCtrl+S"),"","guardar.png");
	utils->AddItemToMenu(file,mxID_FILE_SAVE_AS, _Z("Guardar &Como...\tCtrl+Shift+S"),"","guardar_como.png");
	utils->AddItemToMenu(file,mxID_FILE_EDIT_FLOW, _Z("Editar Diagrama de Flujo...\tF7"),"","flujo.png");
	utils->AddItemToMenu(file,mxID_FILE_PRINT, _Z("Imprimir...\tCtrl+P"),"","imprimir.png");
	
	wxMenu *export_menu=new wxMenu;
	
	utils->AddItemToMenu(export_menu,mxID_FILE_EXPORT_PREVIEW, _Z("Vista previa..."),"","export_preview.png");
	export_menu->AppendSeparator();
	for (int id = mxID_FILE_EXPORT_LANG_FIRST+1; id < int(mxID_FILE_EXPORT_LANG_LAST); id++)
		utils->AddItemToMenu( export_menu,id, _ZZ("Convertir a ")+utils->GetExportLangName(id)+_T("..."),"",wxString("exp_")+utils->GetExportLangCode(id)+".png");
	
	export_menu->AppendSeparator();
	utils->AddItemToMenu(export_menu,mxID_FILE_EXPORT_HTML, _Z("Pseudocódigo coloreado (html)..."),"","html.png");
	utils->AddItemToMenu(export_menu,mxID_FILE_EXPORT_PNG, _Z("Diagrama de flujo (png, bmp o jpg)..."),"","flujo.png");
	file->AppendSubMenu(export_menu,_Z("Exportar"),"");
	
	utils->AddItemToMenu(file,mxID_FILE_CLOSE, _Z("&Cerrar...\tCtrl+W"),"","cerrar.png");
	file->AppendSeparator();
	utils->AddItemToMenu(file,mxID_FILE_EXIT, _Z("&Salir\tAlt+F4"),"","salir.png");
	menu->Append(file, _Z("&Archivo"));
	file_menu=file;
	RegenFileMenu("");
	
	wxMenu *edit = new wxMenu;
	utils->AddItemToMenu(edit,mxID_EDIT_UNDO, _Z("&Deshacer\tCtrl+Z"),"","deshacer.png");
	utils->AddItemToMenu(edit,mxID_EDIT_REDO, _Z("&Rehacer\tCtrl+Shift+Z"),"","rehacer.png");
	edit->AppendSeparator();
	utils->AddItemToMenu(edit,mxID_EDIT_CUT, _Z("C&ortar\tCtrl+X"),"","cortar.png");
	utils->AddItemToMenu(edit,mxID_EDIT_COPY, _Z("&Copiar\tCtrl+C"),"","copiar.png");
	utils->AddItemToMenu(edit,mxID_EDIT_PASTE, _Z("&Pegar\tCtrl+V"),"","pegar.png");
	utils->AddItemToMenu(edit,mxID_EDIT_TOGGLE_LINES_UP, _Z("Mover Hacia Arriba\tCtrl+T"),_Z("Mueve la o las lineas seleccionadas hacia arriba"),"arriba.png");
	utils->AddItemToMenu(edit,mxID_EDIT_TOGGLE_LINES_DOWN, _Z("Mover Hacia Abajo\tCtrl+Shift+T"),_Z("Mueve la o las lineas seleccionadas hacia abajo"),"abajo.png");
	edit->AppendSeparator();
	utils->AddItemToMenu(edit,mxID_EDIT_FIND, _Z("Buscar...\tCtrl+F"),"","buscar.png");
	utils->AddItemToMenu(edit,mxID_EDIT_FIND_PREV, _Z("Buscar Anterior\tShift+F3"),"","anterior.png");
	utils->AddItemToMenu(edit,mxID_EDIT_FIND_NEXT, _Z("Buscar Siguiente\tF3"),"","siguiente.png");
	utils->AddItemToMenu(edit,mxID_EDIT_REPLACE, _Z("&Reemplazar...\tCtrl+R"),"","reemplazar.png");
	edit->AppendSeparator();
	utils->AddItemToMenu(edit,mxID_EDIT_SELECT_ALL, _Z("Seleccionar Todo\tCtrl+A"),"","seleccionar.png");
	utils->AddItemToMenu(edit,mxID_EDIT_DUPLICATE, _Z("Duplicar Lineas\tCtrl+L"),"","duplicar.png");
	utils->AddItemToMenu(edit,mxID_EDIT_DELETE, _Z("Eliminar Lineas\tCtrl+Shift+L"),"","eliminar.png");
	utils->AddItemToMenu(edit,mxID_EDIT_COMMENT, _Z("Comentar Lineas\tCtrl+D"),"","comentar.png");
	utils->AddItemToMenu(edit,mxID_EDIT_UNCOMMENT, _Z("Descomentar Lineas\tCtrl+Shift+D"),"","descomentar.png");
	edit->AppendSeparator();
	utils->AddItemToMenu(edit,mxID_EDIT_INDENT_SELECTION,_Z("Corregir Indentado\tCtrl+I"),"","indentar.png");
//	utils->AddItemToMenu(edit,mxID_EDIT_BEAUTIFY_CODE,_Z("Emprolijar Algoritmo\tCtrl+Shift+I"),"","acomodar.png");
	menu->Append(edit, _Z("&Editar"));
	
	wxMenu *cfg = new wxMenu;
	
	wxMenu *cfg_help = new wxMenu;
	mi_autocomp = utils->AddCheckToMenu(cfg_help,mxID_CONFIG_AUTOCOMP, _Z("Utilizar Autocompletado"),"",config->autocomp);
	mi_autoclose = utils->AddCheckToMenu(cfg_help,mxID_CONFIG_AUTOCLOSE, _Z("Cerrar Repetitivas/Condicionales"),"",config->autoclose);
	mi_highlight_blocks = utils->AddCheckToMenu(cfg_help,mxID_CONFIG_HIGHLIGHT_BLOCKS, _Z("Resaltar bloques lógicos"),"",config->highlight_blocks);
	mi_calltip_helps = utils->AddCheckToMenu(cfg_help,mxID_CONFIG_CALLTIP_HELPS, _Z("Utilizar Ayudas Emergentes"),"",config->calltip_helps);
	mi_smart_indent = utils->AddCheckToMenu(cfg_help,mxID_CONFIG_SMART_INDENT, _Z("Utilizar Indentado Inteligente"),"",config->smart_indent);
	mi_rt_syntax = utils->AddCheckToMenu(cfg_help,mxID_CONFIG_RT_SYNTAX, _Z("Comprobar Sintaxis Mientras Escribe\tF12"),"",config->rt_syntax);
	mi_quickhelp = utils->AddCheckToMenu(cfg_help,mxID_CONFIG_SHOW_QUICKHELP, _Z("Mostrar Ayuda Rapida"),"",config->auto_quickhelp);
	cfg->AppendSubMenu(cfg_help,_Z("Asistencias"));
	
	wxMenu *cfg_pres = new wxMenu;
	mi_fullscreen = utils->AddCheckToMenu(cfg_pres,mxID_CONFIG_FULLSCREEN, _Z("Pantalla Completa\tF11"),"",m_fullscreen);
	mi_animate_gui = utils->AddCheckToMenu(cfg_pres,mxID_CONFIG_ANIMATE_GUI, _Z("Animar paneles"),"",config->animate_gui);
	mi_reorganize_for_debug = utils->AddCheckToMenu(cfg_pres,mxID_CONFIG_REORGANIZE_FOR_DEBUG, _Z("Organizar Ventanas al Iniciar Paso a Paso"),"",config->reorganize_for_debug);
	mi_rt_annotate = utils->AddCheckToMenu(cfg_pres,mxID_CONFIG_RT_ANNOTATE, _Z("Intercalar los mensajes de error en el pseudocódigo"),"",config->rt_annotate);
	mi_unicode_opers = utils->AddCheckToMenu(cfg_pres,mxID_CONFIG_UNICODE_OPERS, _Z("Utilizar símbolos unicode para representar operadores"),"",config->unicode_opers);
	mi_use_colors = utils->AddCheckToMenu(cfg_pres,mxID_CONFIG_USE_COLORS, _Z("Utilizar colores al ejecutar en la terminal"),"",config->use_colors);
	mi_shape_colors = utils->AddCheckToMenu(cfg_pres,mxID_CONFIG_SHAPE_COLORS, _Z("Colorear bloques según tipo en el diagrama de flujo"),_Z(""),config->shape_colors);	
	mi_psdraw_nocrop = utils->AddCheckToMenu(cfg_pres,mxID_CONFIG_PSDRAW_NO_CROP, _Z("Mostrar textos completos en el diagrama de flujo"),_Z(""),config->psdraw_nocrop);	
	mi_use_dark_theme = utils->AddCheckToMenu(cfg_pres,mxID_CONFIG_USE_DARK_THEME, _Z("Utilizar fondo negro en este editor"),"",config->use_dark_theme);
	mi_use_dark_psdraw = utils->AddCheckToMenu(cfg_pres,mxID_CONFIG_USE_DARK_PSDRAW, _Z("Utilizar fondo oscuro en el editor de diagramas"),"",config->use_dark_psdraw);
	mi_use_dark_psterm = utils->AddCheckToMenu(cfg_pres,mxID_CONFIG_USE_DARK_PSTERM, _Z("Utilizar fondo negro en la terminal"),"",config->use_dark_psterm);
	mi_big_icons = utils->AddCheckToMenu(cfg_pres,mxID_CONFIG_BIG_ICONS, _Z("Íconos más grandes (p/pantallas HiDPI)"),"",config->big_icons);
	utils->AddItemToMenu(cfg_pres,mxID_CONFIG_SELECT_FONTS, _Z("Seleccionar fuentes..."),"","fuentes.png");
	cfg->AppendSubMenu(cfg_pres,_Z("Presentación"));
	
	utils->AddItemToMenu(cfg,mxID_CONFIG_LANGUAGE, _Z("Opciones del Lenguaje (perfiles)..."),"","lenguaje.png");
	mi_nassi_shne = utils->AddCheckToMenu(cfg,mxID_CONFIG_NASSI_SHNEIDERMAN, _Z("Utilizar diagramas Nassi-Shneiderman"),"",cfg_lang[LS_USE_NASSI_SHNEIDERMAN]);
#if !defined(__WIN32__) && !defined(__APPLE__)
	cfg->AppendSeparator();
	utils->AddItemToMenu(cfg,mxID_CONFIG_ICON_INSTALLER, _Z("Actualizar accesos directos..."),"","");
#endif	
	menu->Append(cfg, _Z("&Configurar"));
	
	wxMenu *run = new wxMenu;
	utils->AddItemToMenu(run,mxID_RUN_RUN, _Z("Ejecutar\tF9"),"","ejecutar.png");
	utils->AddItemToMenu(run,mxID_RUN_STEP_STEP, _Z("Ejecutar Paso a Paso\tF5"),"","pasos.png");
	utils->AddItemToMenu(run,mxID_RUN_SUBTITLES, _Z("Ejecución Explicada"),"","subtitles.png");
	utils->AddItemToMenu(run,mxID_RUN_CHECK, _Z("Verificar Sintaxis\tShift+F9"),"","verificar.png");
//	utils->AddItemToMenu(run,mxID_RUN_DRAW_FLOW, _Z("Dibujar Diagrama de Flujo"),"","flujo.png");
	utils->AddItemToMenu(run,mxID_RUN_SET_INPUT, _Z("Predefinir Entrada...\tCtrl+F9"),"","input.png");
	menu->Append(run, _Z("E&jecutar"));
	
	wxMenu *help = new wxMenu;
	utils->AddItemToMenu(help,mxID_HELP_INDEX, _Z("Indice...\tF1"),"","ayuda.png");
//	utils->AddItemToMenu(help,mxID_HELP_REFERENCE, _Z("Referencia...","","referencia.png");
	utils->AddItemToMenu(help,mxID_HELP_QUICKHELP, _Z("Ayuda Rapida\tShift+F1"),"","referencia.png");
	utils->AddItemToMenu(help,mxID_HELP_EXAMPLES, _Z("&Ejemplos..."),"","abrir.png");
//	help->AppendSeparator();
//	utils->AddItemToMenu(help,mxID_HELP_LOGGER, _Z("Reiniciar en modo \"Logging\"...\t"),_Z("Reinicia PSeInt de un modo especial que recaba información para depuración en un archivo de texto"),"");
	help->AppendSeparator();
	utils->AddItemToMenu(help,mxID_HELP_UPDATES, _Z("&Buscar actualizaciones...\t"),_Z("Comprueba a traves de Internet si hay versiones mas recientes de PSeInt disponibles..."),"updates.png");
	utils->AddItemToMenu(help,mxID_HELP_ABOUT, _Z("Acerca de..."),"","acerca_de.png");
	menu->Append(help, _Z("A&yuda"));
	
	SetMenuBar(menu);
	
}

void mxMainWindow::CreateToolbars() {
	
	toolbar = CreateToolBar(wxTB_FLAT | wxTB_NODIVIDER, wxID_ANY);
	int isize = config->big_icons ? 32 : 24;
	toolbar->SetToolBitmapSize(wxSize(isize,isize));
	utils->AddTool(toolbar,mxID_FILE_NEW,_Z("Nuevo"),"nuevo.png","");
	utils->AddTool(toolbar,mxID_FILE_OPEN,_Z("Abrir..."),"abrir.png","");
	utils->AddTool(toolbar,mxID_FILE_SAVE,_Z("Guardar"),"guardar.png","");
	utils->AddTool(toolbar,mxID_FILE_SAVE_AS,_Z("Guardar Como..."),"guardar_como.png","");
	toolbar->AddSeparator();
	utils->AddTool(toolbar,mxID_EDIT_UNDO,_Z("Deshacer"),"deshacer.png","");
	utils->AddTool(toolbar,mxID_EDIT_REDO,_Z("Rehacer"),"rehacer.png","");
	utils->AddTool(toolbar,mxID_EDIT_CUT,_Z("Cortar"),"cortar.png","");
	utils->AddTool(toolbar,mxID_EDIT_COPY,_Z("Copiar"),"copiar.png","");
	utils->AddTool(toolbar,mxID_EDIT_PASTE,_Z("Pegar"),"pegar.png","");
	utils->AddTool(toolbar,mxID_EDIT_INDENT_SELECTION,_Z("Corregir Indentado"),"indentar.png","");
	utils->AddCheckTool(toolbar,mxID_CONFIG_RT_ANNOTATE,_Z("Mostrar mensajes de error"),"annotate.png","",config->rt_annotate);
	toolbar->EnableTool(mxID_CONFIG_RT_ANNOTATE,config->rt_syntax);
	toolbar->AddSeparator();
	utils->AddTool(toolbar,mxID_EDIT_FIND,_Z("Buscar"),"buscar.png","");
	utils->AddTool(toolbar,mxID_EDIT_FIND_PREV,_Z("Buscar Anterior"),"anterior.png","");
	utils->AddTool(toolbar,mxID_EDIT_FIND_NEXT,_Z("Buscar Siguiente"),"siguiente.png","");
	utils->AddTool(toolbar,mxID_EDIT_REPLACE,_Z("Reemplazar"),"reemplazar.png","");
	toolbar->AddSeparator();
	utils->AddTool(toolbar,mxID_RUN_RUN,_Z("Ejecutar..."),"ejecutar.png","");
	utils->AddTool(toolbar,mxID_RUN_STEP_STEP,_Z("Ejecutar paso a paso..."),"paso.png","");
	utils->AddTool(toolbar,mxID_FILE_EDIT_FLOW,_Z("Dibujar Diagrama de Flujo..."),"flujo.png","");
	toolbar->AddSeparator();
	utils->AddTool(toolbar,mxID_HELP_INDEX,_Z("Ayuda..."),"ayuda.png","");
//	utils->AddTool(toolbar,mxID_FILE_EXIT,"Salir","salir.png","");
	toolbar->Realize();
	wxAuiPaneInfo info; info.Name("toolbar").Caption("Toolbar").ToolbarPane().Top().Layer(ptlb[0]).Row(ptlb[1]).Position(ptlb[2]).LeftDockable(false).RightDockable(false).Layer(ptlb[0]).Row(ptlb[1]).Position(ptlb[2]);
	/*if (config->show_toolbar)*/ info.Show(); /*else info.Hide();*/
//	aui_manager.AddPane(toolbar, info);
	
}

static void AddCommandButton(wxBoxSizer *sizer, wxPanel *panel, wxWindowID id, wxString image, wxString text) {
	static wxString tooltip = utils->FixTooltip(
		_Z("Con estos botones puede insertar instrucciones o estructuras de control "
		   "en el pseudocódigo. Al seleccionar uno se introduce en el algoritmo dicha "
		   "instrucción o estructura, se marcan con recuadros los argumentos que debe "
		   "completar, y se presenta en la parte inferior de la ventana una ayuda "
		   "rápida acerca de la misma. Puede mantener presionada la tecla shift para "
		   "insertar versiones alternativas o variantes de estos comandos y estructuras."));
	wxButton *but = utils->AddImgButton(sizer,panel,id,DIR_PLUS_FILE_2("inst",config->big_icons?"52":"35",image),text);
	but->SetToolTip(tooltip); but->SetMinSize(wxSize(but->GetMinSize().GetWidth(),10));
}

void mxMainWindow::CreateCommandsPanel() {
	wxPanel *panel = commands = new wxPanel(this);
	wxBoxSizer *sizer = new wxBoxSizer(wxVERTICAL);
	AddCommandButton(sizer,panel,mxID_CMD_ESCRIBIR,"escribir.png",_Z("Escribir"));
	AddCommandButton(sizer,panel,mxID_CMD_LEER,    "leer.png",    _Z("Leer"));
	AddCommandButton(sizer,panel,mxID_CMD_ASIGNAR, "asignar.png", _Z("Asignar"));
	AddCommandButton(sizer,panel,mxID_CMD_SI,      "si.png",      _Z("Si-Entonces"));
	AddCommandButton(sizer,panel,mxID_CMD_SEGUN,   "segun.png",   _Z("Según"));
	AddCommandButton(sizer,panel,mxID_CMD_MIENTRAS,"mientras.png",_Z("Mientras"));
	AddCommandButton(sizer,panel,mxID_CMD_REPETIR, "repetir.png", _Z("Repetir"));
	AddCommandButton(sizer,panel,mxID_CMD_PARA,    "para.png",    _Z("Para"));
	button_subproc=NULL; CreateButtonSubProceso(panel,sizer);
	panel->SetSizerAndFit(sizer);
	
	wxAuiPaneInfo info_helper,info_win;
	info_win.Name("commands").Caption(_Z("Comandos")).Right().Layer(pcmd[0]).Row(pcmd[1]).Position(pcmd[2]);
	info_helper.Name("helper_commands").CaptionVisible(false).PaneBorder(false).Resizable(false).Right().Layer(hcmd[0]).Row(hcmd[1]).Position(hcmd[2]);
	if (config->show_commands) {
		info_win.Show(); info_helper.Hide();
	} else {
		info_win.Hide(); info_helper.Show();
	}
	aui_manager.AddPane(commands, info_win);
	aui_manager.AddPane(new mxPanelHelper(this,mxID_HELPER_COMMANDS,DIR_PLUS_FILE_3(config->images_path,"panels",config->big_icons?"24":"16","commands.png"),"Comandos y Estructuras"), info_helper);
}

void mxMainWindow::CreateVarsPanel() {
	vars_window=new mxVarWindow(this);
	wxAuiPaneInfo info_helper,info_win;
	info_win.Name("vars_panel").Caption(_Z("Variables")).Left().Layer(pvar[0]).Row(pvar[1]).Position(pvar[2]);
	info_helper.Name("helper_vars").CaptionVisible(false).PaneBorder(false).Resizable(false).Left().Layer(hvar[0]).Row(hvar[1]).Position(hvar[2]);
	if (config->show_vars) {
		info_win.Show(); info_helper.Hide();
	} else {
		info_win.Hide(); info_helper.Show();
	}
	aui_manager.AddPane(new mxPanelHelper(this,mxID_HELPER_VARS,DIR_PLUS_FILE_3(config->images_path,"panels",config->big_icons?"24":"16","vars.png"),"Lista de Variables"), info_helper);
	aui_manager.AddPane(vars_window, info_win);
}

void mxMainWindow::CreateOpersPanel() {
	static bool built = false;
	if (built) { 
		wxAuiPaneInfo &pi = aui_manager.GetPane(opers_window);
		config->show_opers = pi.IsShown();
		aui_manager.DetachPane(opers_window); opers_window->Destroy(); 
	}
	opers_window = new mxOpersWindow(this);
	wxAuiPaneInfo info_win;
	info_win.Name("opers_panel").Caption(_Z("Operadores y Funciones")).Left().Layer(popr[0]).Row(popr[1]).Position(popr[2]);
	if (!built) {
		wxAuiPaneInfo info_helper;
		info_helper.Name("helper_opers").CaptionVisible(false).PaneBorder(false).Resizable(false).Left().Layer(hopr[0]).Row(hopr[1]).Position(hopr[2]);
		if (config->show_opers) info_helper.Hide(); else info_helper.Show();
		aui_manager.AddPane(new mxPanelHelper(this,mxID_HELPER_OPERS,DIR_PLUS_FILE_3(config->images_path,"panels",config->big_icons?"24":"16","opers.png"),"Operadores y Funciones"), info_helper);
	}
	if (config->show_opers) info_win.Show(); else info_win.Hide();
	aui_manager.AddPane(opers_window, info_win);
	built = true;
}

void mxMainWindow::CreateDebugControlsPanel() {
	backtrace=new mxBacktrace(this); // hay que crearlo antes que el debug
	aui_manager.AddPane(backtrace, wxAuiPaneInfo().Name(_Z("¿Dónde estoy?")).Bottom().CaptionVisible(false).Hide().Layer(pbak[0]).Row(pbak[1]).Position(pbak[2]));	
	subtitles=new mxSubtitles(this); // hay que crearlo antes que el debug
	aui_manager.AddPane(subtitles, wxAuiPaneInfo().Name("subtitles").Bottom().CaptionVisible(false).Hide().Layer(psub[0]).Row(psub[1]).Position(psub[2]));	
	debug_panel = new mxDebugWindow(this);
	wxAuiPaneInfo info_helper,info_win;
	info_win.Name("debug_panel").Caption(_Z("Paso a paso")).Right().Layer(pdbg[0]).Row(pdbg[1]).Position(pdbg[2]);
	info_helper.Name("helper_debug").CaptionVisible(false).PaneBorder(false).Resizable(false).Right().Layer(hdbg[0]).Row(hdbg[1]).Position(hdbg[2]);
	
	if (config->show_debug_panel) {
		info_win.Show(); info_helper.Hide();
	} else {
		info_win.Hide(); info_helper.Show();
	}
	aui_manager.AddPane(debug_panel, info_win);
	aui_manager.AddPane(new mxPanelHelper(this,mxID_HELPER_DEBUG,DIR_PLUS_FILE_3(config->images_path,"panels",config->big_icons?"24":"16","debug.png"),_Z("Ejecución Paso a Paso")), info_helper);
}

void mxMainWindow::CreateNotebook() {
//	wxSize client_size = GetClientSize();
	notebook = new wxAuiNotebook(this, wxID_ANY, wxDefaultPosition,wxDefaultSize, wxAUI_NB_DEFAULT_STYLE | wxAUI_NB_TAB_EXTERNAL_MOVE | wxNO_BORDER | wxAUI_NB_WINDOWLIST_BUTTON);
//	wxBitmap page_bmp = wxArtProvider::GetBitmap(wxART_NORMAL_FILE, wxART_OTHER, wxSize(16,16));
	aui_manager.AddPane(notebook, wxAuiPaneInfo().Name("notebook_sources").CenterPane().PaneBorder(false));
}

void mxMainWindow::CreateStatusBar() {
	status_bar=new mxStatusBar(this);
	aui_manager.AddPane(status_bar, wxAuiPaneInfo().Name("status_bar").Resizable(false).Bottom().Layer(5).CaptionVisible(false).MinSize(10,status_bar->GetHeight()).PaneBorder(false)	);
}

mxSource *mxMainWindow::NewProgram(const wxString &title) {
	mxSource *source = new mxSource(notebook,title);
	notebook->AddPage(source,title,true);
	if (cfg_lang[LS_PREFER_ALGORITMO]) {
		source->SetText("Algoritmo sin_titulo\n\t\nFinAlgoritmo\n");
		source->SetFieldIndicator(10,20);
		source->SetSelection(22,22);
	} else {
		source->SetText("Proceso sin_titulo\n\t\nFinProceso\n");
		source->SetFieldIndicator(8,18);
		source->SetSelection(20,20);
	}
	source->Analyze();
	source->SetJustCreated();
	status_bar->SetStatus(STATUS_NEW_SOURCE);
	return source;
}

mxSource *mxMainWindow::OpenTestPackage(const wxString &path) {
	if (test_panel) CloseTestPackage();
	test_panel = new mxTestPanel(this);
	aui_manager.AddPane(test_panel, wxAuiPaneInfo().Name("ejercicio").Caption(_Z("Ejercicio")).Bottom().CaptionVisible(false).Hide().Layer(pevl[0]).Row(pevl[1]).Position(pevl[2]));	
	wxString key = "";
	if (path.Lower().EndsWith(".psx")) {
		aui_manager.Update(); wxYield();
		key = wxGetTextFromUser(_Z("Ingrese la clave:"),_Z("PSeInt"),"",this);
	}
	if (!test_panel->Load(path,key)) {
		CloseTestPackage();
		aui_manager.Update();
		return NULL;
	} else {
		aui_manager.GetPane(test_panel).Show();
		if (!test_panel->GetHelp().IsEmpty()) {
			QuickHelp().ShowTestHelp(test_panel->GetHelp());
		}
		aui_manager.Update();
		return test_panel->GetSrc();
	}
}

void mxMainWindow::CloseTestPackage() {
	aui_manager.GetPane(test_panel).Hide();
	aui_manager.DetachPane(test_panel);
	test_panel->Destroy();
	test_panel = NULL;
}

void mxMainWindow::OnFileNew(wxCommandEvent &evt) {
	NewProgram();
}

void mxMainWindow::OnFileExportLang(wxCommandEvent &evt) {
	IF_THERE_IS_SOURCE {
		mxSource *source = CURRENT_SOURCE;
		wxString fname=source->SaveTemp();
		if (debug->debugging)
			debug->Stop();
		else {
			(new mxProcess(source))->ExportLang(fname,utils->GetExportLangCode(evt.GetId()),true);
		}
	}	
}

void mxMainWindow::OnFileExportHtml(wxCommandEvent &evt) {
	IF_THERE_IS_SOURCE {
		mxSource *source = CURRENT_SOURCE;
		wxFileDialog dlg (this, _Z("Exportar"),source->GetPathForExport(),source->GetNameForExport()+_Z(".html"), _Z("Documentos HTML|*.html;*.htm;*.HTML;*.HTM"), wxFD_SAVE | wxFD_OVERWRITE_PROMPT);
		if (dlg.ShowModal() == wxID_OK) {
			config->last_dir=config->last_dir=wxFileName(dlg.GetPath()).GetPath();
			HtmlExporter ce;
			wxString title = source->GetPageText();
			if (title.Last()=='*') title.RemoveLast();
			if (ce.Export(source,title,dlg.GetPath()))
				wxLaunchDefaultBrowser(dlg.GetPath());
			else
				wxMessageBox(_Z("No se pudo guardar el archivo"),_Z("Error"),wxID_OK|wxICON_ERROR,this);
		}
	}
}

void mxMainWindow::OnFileClose(wxCommandEvent &evt) {
	IF_THERE_IS_SOURCE {
		mxSource *source=CURRENT_SOURCE;
		if (source->GetModify()) {
			int res=wxMessageBox(_Z("Hay cambios sin guardar. ¿Desea guardarlos antes de cerrar?"), source->filename, wxYES_NO|wxCANCEL|wxICON_QUESTION,this);
			if (res==wxCANCEL)
				return;
			else if (res==wxYES) {
				if (source->sin_titulo) {
					OnFileSaveAs(evt);
					if (source->GetModify())
						return;
				} else
					source->SaveFile(source->filename);
			}
		}
		notebook->DeletePage(notebook->GetSelection());
	}
}


void mxMainWindow::OnFileOpen(wxCommandEvent &evt) {
	wxFileDialog dlg (this, _Z("Abrir Archivo"), config->last_dir, _Z(" "), "Any file (*)|*", wxFD_OPEN | wxFD_FILE_MUST_EXIST | wxFD_MULTIPLE);
	dlg.SetWildcard(_Z("Todos los archivos|*|Algoritmos en pseudocódigo|*.psc;*.PSC|Ejercicios empaquetados|*.psz;*.PSZ;*.psx;*.PSX|Archivos de texto|*.txt;*.TXT"));
	if (dlg.ShowModal() == wxID_OK) {
		wxArrayString paths;
		dlg.GetPaths(paths);
		for (unsigned int i=0;i<paths.GetCount();i++) {
			OpenProgram(paths[i]);
		}
		if (paths.GetCount()) config->last_dir=wxFileName(paths[0]).GetPath();
	}
}

void mxMainWindow::OnHelpExamples(wxCommandEvent &evt) {
	if (!helpw) helpw = new mxHelpWindow();
	helpw->ShowHelp("ejemplos.html");
}

void mxMainWindow::RegenFileMenu(wxString path) {
	int i=0, c = config->last_files.GetCount();
	if (path.Len()) {
		if (c) {
			for (i=0;i<c;i++)
				if (config->last_files[i]==path)
					break;
			if (i==c) {
				if (c==5) {
					for (int i=4;i>0;i--)
						config->last_files[i]=config->last_files[i-1];
				} else {
					config->last_files.Add("");
					for (int i=c;i>0;i--)
						config->last_files[i]=config->last_files[i-1];
					c++;
				}
			} else if (i) {
				if (i!=4) for (int j=i;j>0;j--)
					config->last_files[j]=config->last_files[j-1];
			}
			config->last_files[0]=path;
		} else {
			config->last_files.Add(path);
			c++;
		}
	}
		
	if (!file_history[0] && c)
		file_menu->AppendSeparator();
	
	for (i=0;i<c;i++) {
		if (file_history[i])
			file_menu->Remove(file_history[i]);
		if (config->last_files[i][0])
			file_history[i] = utils->AddItemToMenu(file_menu, mxID_FILE_SOURCE_HISTORY_0+i,config->last_files[i],config->last_files[i],wxString("recent")<<i+1<<".png");
	}
}

mxSource *mxMainWindow::OpenProgram(wxString path, bool is_example) {
	
	bool file_exists = wxFileName(path).FileExists();
#ifndef __WIN32__
	if (!file_exists) { // problems due to ansi-wx on utf8-linux
		wxFileName new_filename(path.ToUTF8().data());
		if (new_filename.FileExists()) { path = new_filename.GetFullPath(); file_exists=true; }
	}
#endif
	
	if (!file_exists) {
		wxMessageBox(wxString(_Z("No se pudo abrir el archivo "))<<path,_Z("Error"),wxOK|wxICON_ERROR);
		return NULL;
	}
		
	if (!is_example) RegenFileMenu(path); // por esta linea no recibo path como const wxString &, porque si es del historial me lo modifica
	if (path.Lower().EndsWith(".psz")||path.Lower().EndsWith(".psx")) return OpenTestPackage(path);
	
	mxSource *source = new mxSource(notebook,wxFileName(path).GetFullName(),path);
	notebook->AddPage(source,wxFileName(path).GetFullName(),true);
	source->LoadFile(path);
	source->Analyze();
	if (is_example) source->SetExample();
	if (config->rt_syntax) source->DoRealTimeSyntax();
	source->SetStatus(STATUS_NEW_SOURCE);
	return source;
}

void mxMainWindow::OnFileSave(wxCommandEvent &evt) {
	IF_THERE_IS_SOURCE {
		mxSource *source = CURRENT_SOURCE;
		if (source->sin_titulo)
			OnFileSaveAs(evt);
		else
			source->SaveFile(source->filename);
	}
}

void mxMainWindow::OnFileSaveAs(wxCommandEvent &evt) {
	
	IF_THERE_IS_SOURCE {
		mxSource *source=CURRENT_SOURCE;
		wxFileDialog dlg (this, _Z("Guardar"),
						  source->sin_titulo?config->last_dir:wxFileName(source->filename).GetPath(),
						  source->GetFileName(), "Any file (*)|*", wxFD_SAVE | wxFD_OVERWRITE_PROMPT);
		dlg.SetWildcard(_Z("Todos los archivos|*|Algoritmos en pseudocódigo|*.psc;*.PSC|Archivos de texto|*.txt;*.TXT"));
		if (dlg.ShowModal() == wxID_OK) {
			wxFileName file = dlg.GetPath();
#ifndef __WIN32__
			if (file.GetFullPath().IsEmpty()) { // problems due to ansi-wx on utf8-linux
				wxMessageBox("El nombre del archivo o de alguna carpeta en su ruta\n"
							 "contiene acentos u otro caracteres especiales. En este\n"
							 "sistema ZinjaI no puede guardar correctamente los\n"
							 "cambios del archivo a menos que modifique su nombre o ruta.",
							 "Error",wxOK|wxICON_ERROR,this);
				OnFileSaveAs(evt);
				return;
			}
#endif
			if (file.GetExt().Len()==0) file.SetExt("psc");
			source->UnExample();
			config->last_dir=file.GetPath();
			source->SaveFile(file.GetFullPath());
			source->SetFileName(file.GetFullPath());
			source->SetPageText(file.GetFullName());
			RegenFileMenu(file.GetFullPath());
		}
	}	
}

void mxMainWindow::OnFileExit(wxCommandEvent &evt) {
	Close(true);
}

void mxMainWindow::OnEdit(wxCommandEvent &evt) {
	IF_THERE_IS_SOURCE 
		CURRENT_SOURCE->GetEventHandler()->ProcessEvent(evt);
}

void mxMainWindow::OnRunRun(wxCommandEvent &evt) {
	RunCurrent(true);
}

void mxMainWindow::RunCurrent(bool raise, bool from_psdraw) {
	IF_THERE_IS_SOURCE {
		mxSource *source=CURRENT_SOURCE;
		if (!from_psdraw && source->GetFlowSocket()) {
			source->GetFlowSocket()->Write("send run\n",9);
			return;
		}
		if (!source->UpdateRunningTerminal(raise,true)) {
			wxString fname=source->SaveTemp();
			if (debug->debugging)
				debug->Stop();
			else
				(new mxProcess(source))->Run(fname,true);
		} else {
			source->DoRealTimeSyntax();
		}
	}
}

void mxMainWindow::OnRunStepStep(wxCommandEvent &evt) {
	debug_panel->DebugStartFromGui();
}

void mxMainWindow::OnRunSubtitles(wxCommandEvent &evt) {
	debug_panel->SetSubtitles(true);
	if (!debug->debugging) debug_panel->DebugStartFromGui();
}

void mxMainWindow::OnRunCheck(wxCommandEvent &evt) {
	IF_THERE_IS_SOURCE {
		mxSource *source = CURRENT_SOURCE;
		wxString fname=source->SaveTemp();
		if (debug->debugging)
			debug->Stop();
		else
			(new mxProcess(source))->CheckSyntax(fname);
	}
}

//void mxMainWindow::OnRunDrawFlow(wxCommandEvent &evt) {
//	IF_THERE_IS_SOURCE {
//		mxSource *source = CURRENT_SOURCE;
//		wxString fname=source->SaveTemp();
//		if (debug->debugging)
//			debug->Stop();
//		else
//			(new mxProcess(source))->Draw(fname,true);
//	}
//}

void mxMainWindow::OnRunSaveFlow(wxCommandEvent &evt) {
	IF_THERE_IS_SOURCE {
		mxSource *source = CURRENT_SOURCE;
		wxString fname=source->SaveTemp();
		if (debug->debugging)
			debug->Stop();
		else
			(new mxProcess(source))->SaveDraw(fname,true);
	}		
}

void mxMainWindow::OnClose(wxCloseEvent &evt) {
	
	if (helpw && helpw->IsShown()) helpw->Close();
	
	IF_THERE_IS_SOURCE {
		for (int i=notebook->GetPageCount()-1;i>=0;i--) {
			mxSource *source = (mxSource*)(notebook->GetPage(i));
			if (source->GetModify()) {
				notebook->SetSelection(i);
				int res=wxMessageBox(_Z("Hay cambios sin guardar. ¿Desea guardarlos antes de salir?"), source->filename, wxYES_NO|wxCANCEL|wxICON_QUESTION,this);
				if (res&wxYES) {
					if (!source->sin_titulo)
						source->SaveFile(source->filename);
					else {
						wxCommandEvent evt;
						OnFileSaveAs(evt);
						if (source->GetModify())
							return;
					}
				}
				if (res&wxCANCEL) {
					return;
				}
			}
			notebook->DeletePage(i);
		}
	}
	
	wxYield(); // ~mxClose mata su proceso de paso a paso, el yield es para procesar su OnTerminate antes del while que sigue y no matarlo dos veces
	
	if (proc_for_killing) delete proc_for_killing;
	proc_for_killing=NULL;
	mxProcess *proc=proc_list;
	while (proc) {
		mxProcess *nproc=proc->next;
		if (proc_list->pid) 
			proc->Kill(proc_list->pid,wxSIGKILL);
		proc=nproc;
	}
	if (proc_for_killing) delete proc_for_killing;
	
	config->show_debug_panel = aui_manager.GetPane(debug_panel).IsShown();
	config->show_vars = aui_manager.GetPane(vars_window).IsShown();
	config->show_opers = aui_manager.GetPane(opers_window).IsShown();
	config->show_commands = aui_manager.GetPane(commands).IsShown();
//	config->show_toolbar = aui_manager.GetPane(toolbar).IsShown();
	if (IsMaximized()) {
		config->maximized=true;
	} else if (!IsIconized()) { 
		config->pos_x=GetPosition().x;
		config->pos_y=GetPosition().y;
		config->size_x=GetSize().GetWidth();
		config->size_y=GetSize().GetHeight();
		config->maximized=false;
	}	
	config->Save();
	er_uninit();
	delete find_replace_dialog; find_replace_dialog=NULL;
	delete help; help=NULL;
	delete debug; debug=NULL;
	delete config; config=NULL;
	wxExit();
}

void mxMainWindow::CreateResultsTree() {
	results_tree_ctrl = new wxTreeCtrl(this, wxID_ANY, wxPoint(0,0), wxSize(160,250), wxTR_DEFAULT_STYLE | wxNO_BORDER );
	int isize = config->big_icons?24:16;
	wxImageList* imglist = new wxImageList(isize,isize, true, 2);
	wxString ipath = DIR_PLUS_FILE_2(config->images_path,"res",config->big_icons?"24":"16");
	imglist->Add(wxBitmap(DIR_PLUS_FILE(ipath,"info.png"), wxBITMAP_TYPE_PNG));
	imglist->Add(wxBitmap(DIR_PLUS_FILE(ipath,"error.png"),wxBITMAP_TYPE_PNG));
	results_tree_ctrl->AssignImageList(imglist);
	results_root = results_tree_ctrl->AddRoot(wxString(_Z("PSeInt "))<<VERSION, 0);
	aui_manager.AddPane(results_tree_ctrl, wxAuiPaneInfo().Name("results_tree").Caption(_Z("Resultados")).Bottom().CloseButton(true).MaximizeButton(true).Hide().Layer(prtr[0]).Row(prtr[1]).Position(prtr[2]));	
	
}

void mxMainWindow::OnHelpQuickHelp(wxCommandEvent &evt) {
	
	bool ask = true; // preguntar si no hay fuente abierto o palabra seleccionada
	wxString key;
	IF_THERE_IS_SOURCE { // si hay fuente abierto
		mxSource *source=CURRENT_SOURCE; 
		int pos=source->GetCurrentPos(); // buscar la palabra sobre el cursor
		int s=source->WordStartPosition(pos,true);
		int e=source->WordEndPosition(pos,true);
		key = source->GetTextRange(s,e);
		if (key.Len()!=0) { // puede ser una directiva de preprocesador
			ask=false;
		}
	}
	// si no hay clave, preguntar
	if (ask) key = wxGetTextFromUser(_Z("Palabra a buscar:"), _Z("Ayuda Rápida"), _T(""), this);
	if (key=="K-BOOM!!!") { // para probar el error-recovery
		int *p = NULL;
		std::cout << *p;
	}
	// mostrar panel y cargar ayuda
	if (key.Len()) QuickHelp().ShowHelpPage(help->GetQuickHelp(key));
}


//void mxMainWindow::HideQuickHelp() {
//	if (aui_manager.GetPane(m_quick_help.m_ctrl).IsShown()) {
//		aui_manager.GetPane(quick_html).Hide();
//		aui_manager.Update();
//	} 
//}

void mxMainWindow::CreateQuickHelp() {
	m_quick_help.m_ctrl = new mxHtmlWindow(this, wxID_ANY, wxDefaultPosition, wxSize(400,300));
	m_quick_help.m_ctrl->SetPage(wxString(_Z("PSeInt "))<<VERSION);
	m_quick_help.m_aui = &aui_manager;
	aui_manager.AddPane(m_quick_help.m_ctrl, wxAuiPaneInfo().Name("quick_html").Caption(_Z("Ayuda Rápida")).Bottom().CloseButton(true).MaximizeButton(true).Hide().Layer(phlp[0]).Row(phlp[1]).Position(phlp[2]));	
}

void mxMainWindow::SelectError(wxString text) {
	// elegir el fuente que generó al error
	int index = notebook->GetPageIndex(last_source);
	if (index==wxNOT_FOUND) return;
	if (index!=notebook->GetSelection()) notebook->SetSelection(index);
	// seleccionar esa esa linea e instruccion
	long l,i=-1;
	wxString where=text.AfterFirst(' ').BeforeFirst(':');
	if (where.Contains("inst ")) {
		where.BeforeFirst(' ').ToLong(&l); l--;
		where.AfterLast(' ').BeforeFirst(')').ToLong(&i); i--;
		last_source->SelectInstruccion(l,i);
	} else {
		where.ToLong(&l); l--;
		last_source->SetSelection(last_source->GetLineIndentPosition(l),last_source->GetLineEndPosition(l));
	}
	if (main_window->IsActive()) last_source->SetFocus();
	text = text.AfterFirst(':');
	if (text.StartsWith(" ERROR ")) {
		long e=0;
		text.Mid(7).ToLong(&e);
		if (config->auto_quickhelp) 
			QuickHelp().ShowOutput(help->GetErrorText(text,e));
	}
}

void mxMainWindow::OnSelectError(wxTreeEvent &evt) {
	if (!result_tree_done) return;
	wxString text = results_tree_ctrl->GetItemText(evt.GetItem());
	if (text.StartsWith("Lin ")) SelectError(text);
}

void mxMainWindow::OnCmdAsignar(wxCommandEvent &evt) {
	if (config->auto_quickhelp) 
		QuickHelp().ShowHelpText(help->GetCommandText("ASIGNAR"));
	wxArrayString toins;
	if (cfg_lang[LS_FORCE_SEMICOLON])
		toins.Add("{variable}<-{expresion};");
	else
		toins.Add("{variable}<-{expresion}");
	InsertCode(toins);
}

void mxMainWindow::OnCmdLeer(wxCommandEvent &evt) {
	if (config->auto_quickhelp) 
		QuickHelp().ShowHelpText(help->GetCommandText("LEER"));
	wxArrayString toins;
	if (cfg_lang[LS_FORCE_SEMICOLON])
		toins.Add("Leer {lista_de_variables};");
	else
		toins.Add("Leer {lista_de_variables}");
	InsertCode(toins);
}

void mxMainWindow::OnCmdEscribir(wxCommandEvent &evt) {
	bool alternative = wxGetKeyState(WXK_SHIFT);
	if (config->auto_quickhelp) 
		QuickHelp().ShowHelpText(help->GetCommandText("ESCRIBIR"));
	wxArrayString toins;
	wxString line = "Escribir {lista_de_expresiones}";
	if (alternative) line<<" sin saltar";
	if (cfg_lang[LS_FORCE_SEMICOLON]) line<<";";
	toins.Add(line);
	InsertCode(toins);
}

void mxMainWindow::OnCmdMientras(wxCommandEvent &evt) {
	if (config->auto_quickhelp) 
		QuickHelp().ShowHelpText(help->GetCommandText("MIENTRAS"));
	wxArrayString toins;
	toins.Add("Mientras {expresion_logica} Hacer");
	toins.Add("\t{secuencia_de_acciones}");
	toins.Add("FinMientras");
	InsertCode(toins);
}

void mxMainWindow::OnCmdRepetir(wxCommandEvent &evt) {
	bool alternative = cfg_lang[LS_PREFER_REPEAT_WHILE]!=wxGetKeyState(WXK_SHIFT);
	if (config->auto_quickhelp) 
		QuickHelp().ShowHelpText(help->GetCommandText(alternative?"REPETIR - MIENTRAS QUE":"REPETIR - HASTA QUE"));
	wxArrayString toins;
	toins.Add("Repetir");
	toins.Add("\t{secuencia_de_acciones}");
	if (alternative)
		toins.Add("Mientras Que {expresion_logica}");
	else
		toins.Add("Hasta Que {expresion_logica}");
	InsertCode(toins);
}

void mxMainWindow::OnCmdPara(wxCommandEvent &evt) {
	bool alternative = wxGetKeyState(WXK_SHIFT);
	if (config->auto_quickhelp) 
		QuickHelp().ShowHelpText(help->GetCommandText(alternative?"PARA CADA":"PARA"));
	wxArrayString toins;
	if (alternative) {
		toins.Add("Para Cada {id_elemento} de {id_arreglo} Hacer");
	} else {
		toins.Add("Para {variable_numerica}<-{valor_inicial} Hasta {valor_final} Con Paso {paso} Hacer");
	}
	toins.Add("\t{secuencia_de_acciones}");
	toins.Add("FinPara");
	InsertCode(toins);
}

void mxMainWindow::OnCmdSubProceso(wxCommandEvent &evt) {
	bool alternative = wxGetKeyState(WXK_SHIFT);
	wxString funcion = cfg_lang[LS_PREFER_FUNCION]?"Funcion":(cfg_lang[LS_PREFER_ALGORITMO]?"SubAlgoritmo":"SubProceso");
	if (config->auto_quickhelp) 
		QuickHelp().ShowHelpText(help->GetCommandText(funcion.Upper()));
	wxArrayString toins;
	toins.Add(funcion+(alternative?"":" {variable_de_retorno} <-")+" {Nombre} ( {Argumentos} )");
	toins.Add("\t");
	toins.Add(wxString("Fin")+funcion);
	toins.Add("");
	IF_THERE_IS_SOURCE {
		mxSource *source = CURRENT_SOURCE;
		source->SetSelection(0,0);
		source->EnsureVisibleEnforcePolicy(0);
	}
	InsertCode(toins);
}

void mxMainWindow::OnCmdSi(wxCommandEvent &evt) {
	bool alternative = wxGetKeyState(WXK_SHIFT);
	if (config->auto_quickhelp) 
		QuickHelp().ShowHelpText(help->GetCommandText(alternative?"SI - ENTONCES":"SI - ENTONCES - SINO"));
	wxArrayString toins;
	toins.Add("Si {expresion_logica} Entonces");
	toins.Add("\t{acciones_por_verdadero}");
	if (!alternative) {
		toins.Add("SiNo");
		toins.Add("\t{acciones_por_falso}");
	}
	toins.Add("FinSi");
	InsertCode(toins);
}

void mxMainWindow::OnCmdSegun(wxCommandEvent &evt) {
	if (config->auto_quickhelp) 
		QuickHelp().ShowHelpText(help->GetCommandText("SEGUN"));
	wxArrayString toins;
	toins.Add("Segun {variable_numerica} Hacer");
	toins.Add("\t{opcion_1}:");
	toins.Add("\t\t{secuencia_de_acciones_1}");
	toins.Add("\t{opcion_2}:");
	toins.Add("\t\t{secuencia_de_acciones_2}");
	toins.Add("\t{opcion_3}:");
	toins.Add("\t\t{secuencia_de_acciones_3}");
	toins.Add("\tDe Otro Modo:");
	toins.Add("\t\t{secuencia_de_acciones_dom}");
	toins.Add("FinSegun");
	InsertCode(toins);
}	

void mxMainWindow::InsertCode(wxString toins) {
	IF_THERE_IS_SOURCE {
		mxSource *source = CURRENT_SOURCE;
		if (source->GetReadOnly()) return source->MessageReadOnly();
		// obtener la seleccion y eliminarla
		int ss = source->GetSelectionStart(), se = source->GetSelectionEnd();
		if (ss!=se) {
			source->SetTargetStart(ss);
			source->SetTargetEnd(se);
			source->ReplaceTarget("");
		}
		int pos = ss;
		wxString toindic = toins;
		// quitar las llaves del texto e insertarlo
		for (int j=toindic.size()-1;j>=0;j--) {
			if (toindic[j]=='{' || toindic[j]=='}')
				toins = toins.SubString(0,j-1)+toins.Mid(j+1);
		}
		source->InsertText(pos,toins);
		// aplicarle al texto los indicadores en los campos a completar
		int p1=-1,p2=-2, des=0;
		for (unsigned int j=0;j<toindic.size();j++) {
			if (toindic[j]=='{' && p1==-1) {
				p1 = j+pos-des;
				des++;
			} else if (toindic[j]=='}' && p1!=-1) {
				p2 = j+pos-des;
				des++;
				source->SetFieldIndicator(p1,p2);
				p1 = -1;
			}
		}
		source->SetSelectionStart(ss);
		source->SetSelectionEnd(ss+toins.Len());
		source->SetFocus();
	}
}

void mxMainWindow::InsertCode(wxArrayString &toins) {
	IF_THERE_IS_SOURCE {
		mxSource *source = CURRENT_SOURCE;
		if (source->GetReadOnly()) return source->MessageReadOnly();
		
		// obtener la seleccion y eliminarla
		int ss = source->GetSelectionStart(), se = source->GetSelectionEnd();
		if (ss!=se) {
			source->SetTargetStart(ss);
			source->SetTargetEnd(se);
			source->ReplaceTarget("");
		}
		int line = source->LineFromPosition(ss);
		// ver si había algo en la misma linea despues de la seleccion para saber si va un enter más o no
		int oline_end = source->GetLineEndPosition(line);
		se=ss; while (se<oline_end && (source->GetCharAt(se)==' '||source->GetCharAt(se)=='\t')) se++;
		if (se<oline_end) { source->InsertText(ss,"\n"); source->Indent(line+1,line+1); }
		// ver si había algo en la misma linea antes de la seleccion para saber si va un enter más o no
		int oline_beg = source->PositionFromLine(line);
		se=ss-1; while (se>=oline_beg && (source->GetCharAt(se)==' '||source->GetCharAt(se)=='\t')) se--;
		if (se>=oline_beg) { source->InsertText(ss,"\n"); line++; }
			
		// insertar el código con su correspondiente formato (en la linea dada por line, que está en blanco)
		int oline=line;
		for (unsigned int i=0;i<toins.GetCount();i++) {
			if (cfg_lang[LS_LAZY_SYNTAX] && toins[i].StartsWith("Fin"))
				toins[i].Replace("Fin","Fin ",false);
			if (i) source->InsertText(source->PositionFromLine(line),"\n");
			int pos = source->GetLineIndentPosition(line);
			wxString toindic = toins[i];
			// quitar las llaves del texto e insertarlo
			for (int j=toindic.size()-1;j>=0;j--) {
				if (toindic[j]=='{' || toindic[j]=='}')
					toins[i] = toins[i].SubString(0,j-1)+toins[i].Mid(j+1);
			}
			source->InsertText(pos,toins[i]);
			// aplicarle al texto los indicadores en los campos a completar
			int p1=-1,p2=-2, des=0;
			for (unsigned int j=0;j<toindic.size();j++) {
				if (toindic[j]=='{' && p1==-1) {
					p1 = j+pos-des;
					des++;
				} else if (toindic[j]=='}' && p1!=-1) {
					p2 = j+pos-des;
					des++;
					source->SetFieldIndicator(p1,p2);
					p1 = -1;
				}
			}
			line++;
		}
		// corregir el indentado y seleccionar el código nuevo 
		source->Indent(oline,line-1);
		source->SetSelectionStart(source->GetLineIndentPosition(oline));
		source->SetSelectionEnd(source->GetLineEndPosition(line-1));
		source->SetFocus();
		source->SetStatus(STATUS_COMMAND);
	}
}
void mxMainWindow::OnEditFind (wxCommandEvent &event) {
	IF_THERE_IS_SOURCE find_replace_dialog->ShowFind(CURRENT_SOURCE);
}

void mxMainWindow::OnEditFindNext (wxCommandEvent &event) {
	if (find_replace_dialog->last_search.Len()) {
		if (!find_replace_dialog->FindNext())
			wxMessageBox(_ZZ("La cadena \"")<<find_replace_dialog->last_search<<_Z("\" no se encontró."), _Z("Buscar"),wxOK|wxICON_HAND);
	} else {
		OnEditFind(event);
	}
}

void mxMainWindow::OnEditFindPrev (wxCommandEvent &event) {
	if (find_replace_dialog->last_search.Len()) {
		if (!find_replace_dialog->FindPrev())
			wxMessageBox(_ZZ("La cadena \"")<<find_replace_dialog->last_search<<_Z("\" no se encontró."), _Z("Buscar"),wxOK|wxICON_HAND);
	} else {
		OnEditFind(event);
	}
}

void mxMainWindow::OnEditReplace (wxCommandEvent &event) {
	IF_THERE_IS_SOURCE find_replace_dialog->ShowReplace(CURRENT_SOURCE);
	return;
}

void mxMainWindow::OnHelpAbout(wxCommandEvent &evt) {
	mxAboutWindow::Run(this);
}

void mxMainWindow::OnHelpIndex(wxCommandEvent &evt) {
	if (helpw) {
		helpw->ShowIndex();
		if (helpw->IsIconized())
			helpw->Maximize(false);
		else
			helpw->Raise();
	} else
		helpw = new mxHelpWindow();
}

void mxMainWindow::OnConfigShowQuickHelp(wxCommandEvent &evt) {
	if (!mi_quickhelp->IsChecked()) {
		mi_quickhelp->Check(false);
		config->auto_quickhelp = false;
	} else {
		mi_quickhelp->Check(true);
		config->auto_quickhelp = true;
	}
}
	
void mxMainWindow::OnConfigSmartIndent(wxCommandEvent &evt) {
	if (!mi_smart_indent->IsChecked()) {
		mi_smart_indent->Check(false);
		config->smart_indent=false;
	} else {
		mi_smart_indent->Check(true);
		config->smart_indent=true;
	}
}

void mxMainWindow::OnConfigRealTimeSyntax(wxCommandEvent &evt) {
	if (!mi_rt_syntax->IsChecked()) {
		mi_rt_syntax->Check(false);
		config->rt_syntax=false;
		toolbar->EnableTool(mxID_CONFIG_RT_ANNOTATE,false);
		for(unsigned int i=0;i<notebook->GetPageCount();i++) {
			((mxSource*)notebook->GetPage(i))->ClearErrorData();
			((mxSource*)notebook->GetPage(i))->ClearErrorMarks();
		}
	} else {
		mi_rt_syntax->Check(true);
		toolbar->EnableTool(mxID_CONFIG_RT_ANNOTATE,true);
		config->rt_syntax=true;
		ShowResults(false,true);
	}
	CheckIfNeedsRTS();
}

void mxMainWindow::OnConfigRealTimeAnnotate(wxCommandEvent &evt) {
	IF_THERE_IS_SOURCE CURRENT_SOURCE->HideCalltip(true,true);
	if (config->rt_annotate) {
		mi_rt_annotate->Check(false);
		toolbar->ToggleTool(mxID_CONFIG_RT_ANNOTATE,false);
		config->rt_annotate=false;
	} else {
		mi_rt_annotate->Check(true);
		toolbar->ToggleTool(mxID_CONFIG_RT_ANNOTATE,true);
		config->rt_annotate=true;
	}
//	for(unsigned int i=0;i<notebook->GetPageCount();i++) {
//		((mxSource*)notebook->GetPage(i))->ClearErrorData();
//		((mxSource*)notebook->GetPage(i))->ClearErrorMarks();
//	}
	CheckIfNeedsRTS();
}

void mxMainWindow::OnConfigCalltipHelps(wxCommandEvent &evt) {
	if (!mi_calltip_helps->IsChecked()) {
		mi_calltip_helps->Check(false);
		config->calltip_helps=false;
	} else {
		mi_calltip_helps->Check(true);
		config->calltip_helps=true;
	}
}

void mxMainWindow::OnConfigAutoComp(wxCommandEvent &evt) {
	if (!mi_autocomp->IsChecked()) {
		mi_autocomp->Check(false);
		config->autocomp=false;
	} else {
		mi_autocomp->Check(true);
		config->autocomp=true;
	}
}

void mxMainWindow::OnConfigAutoClose(wxCommandEvent &evt) {
	if (!mi_autoclose->IsChecked()) {
		mi_autoclose->Check(false);
		config->autoclose=false;
	} else {
		mi_autoclose->Check(true);
		config->autoclose=true;
	}
}

void mxMainWindow::OnConfigHighlightBlocks(wxCommandEvent &evt) {
	if (!mi_highlight_blocks->IsChecked()) {
		mi_highlight_blocks->Check(false);
		config->highlight_blocks=false;
		IF_THERE_IS_SOURCE CURRENT_SOURCE->UnHighLightBlock();
	} else {
		mi_highlight_blocks->Check(true);
		config->highlight_blocks=true;
	}
	CheckIfNeedsRTS();
}

void mxMainWindow::OnConfigUseColors(wxCommandEvent &evt) {
	if (!mi_use_colors->IsChecked()) {
		mi_use_colors->Check(false);
		config->use_colors=false;
	} else {
		mi_use_colors->Check(true);
		config->use_colors=true;
	}
}

void mxMainWindow::OnConfigUnicodeOpers(wxCommandEvent &evt) {
	if (!mi_unicode_opers->IsChecked()) {
		mi_unicode_opers->Check(false);
		for(unsigned int i=0;i<notebook->GetPageCount();i++) {
			mxSource *src = ((mxSource*)notebook->GetPage(i));
			wxString text = src->GetText(); src->ToRegularOpers(text); src->SetText(text);
		}
		config->unicode_opers=false;
	} else {
		mi_unicode_opers->Check(true);
		config->unicode_opers=true;
	}
	for(unsigned int i=0;i<notebook->GetPageCount();i++)
		((mxSource*)notebook->GetPage(i))->Analyze();
}

void mxMainWindow::OnConfigPSDrawNoCrop(wxCommandEvent &evt) {
	if (!mi_psdraw_nocrop->IsChecked()) {
		mi_psdraw_nocrop->Check(false);
		config->psdraw_nocrop=false;
	} else {
		mi_psdraw_nocrop->Check(true);
		config->psdraw_nocrop=true;
	}
}

void mxMainWindow::OnConfigReorganizeForDebug(wxCommandEvent &evt) {
	if (!mi_reorganize_for_debug->IsChecked()) {
		mi_reorganize_for_debug->Check(false);
		config->reorganize_for_debug=false;
	} else {
		mi_reorganize_for_debug->Check(true);
		config->reorganize_for_debug=true;
	}
}

void mxMainWindow::OnConfigAnimateGui(wxCommandEvent &evt) {
	if (!mi_animate_gui->IsChecked()) {
		mi_animate_gui->Check(false);
		config->animate_gui=false;
	} else {
		mi_animate_gui->Check(true);
		config->animate_gui=true;
	}
}

void mxMainWindow::OnConfigSelectFonts(wxCommandEvent &evt) {
	if (mxFontsConfig().ShowModal()) {
		for(unsigned int i=0;i<notebook->GetPageCount();i++) { 
			((mxSource*)notebook->GetPage(i))->SetStyling(true);
		}
	}
}

void mxMainWindow::OnConfigUseDarkPSTerm(wxCommandEvent &evt) {
	if (!mi_use_dark_psterm->IsChecked()) {
		mi_use_dark_psterm->Check(false);
		config->use_dark_psterm=false;
	} else {
		mi_use_dark_psterm->Check(true);
		config->use_dark_psterm=true;
	}
}

void mxMainWindow::OnConfigUseDarkPSDraw(wxCommandEvent &evt) {
	if (!mi_use_dark_psdraw->IsChecked()) {
		mi_use_dark_psdraw->Check(false);
		config->use_dark_psdraw=false;
	} else {
		mi_use_dark_psdraw->Check(true);
		config->use_dark_psdraw=true;
	}
}

void mxMainWindow::OnConfigBigIcons(wxCommandEvent &evt) {
	if (!mi_big_icons->IsChecked()) {
		mi_big_icons->Check(false);
		config->big_icons=false;
	} else {
		mi_big_icons->Check(true);
		config->big_icons=true;
	}
	wxMessageBox(_Z("Deberá reiniciar PSeInt para que se aplique este cambio"),_Z("Íconos más grandes"),wxOK|wxICON_INFORMATION);
}

void mxMainWindow::OnConfigUseDarkTheme(wxCommandEvent &evt) {
	if (!mi_use_dark_theme->IsChecked()) {
		mi_use_dark_theme->Check(false);
		config->use_dark_theme=false;
	} else {
		mi_use_dark_theme->Check(true);
		config->use_dark_theme=true;
	}
	for(unsigned int i=0;i<notebook->GetPageCount();i++) { 
		((mxSource*)notebook->GetPage(i))->SetStyling(true);
	}
}

void mxMainWindow::OnConfigFullScreen(wxCommandEvent &evt) {
	if (!mi_fullscreen->IsChecked()) {
		mi_fullscreen->Check(false);
		SetFullScreen(false);
	} else {
		mi_fullscreen->Check(true);
		SetFullScreen(true);
	}
}

void mxMainWindow::OnConfigShowShapeColors (wxCommandEvent & evt) {
	if (!mi_shape_colors->IsChecked()) {
		mi_shape_colors->Check(false);
		config->shape_colors=false;
	} else {
		mi_shape_colors->Check(true);
		config->shape_colors=true;
	}
}


void mxMainWindow::OnPaneClose(wxAuiManagerEvent& event) {
	if (event.pane->name == "commands")
		ShowCommandsPanel(false,true);
	else if (event.pane->name == "debug_panel")
		ShowDebugPanel(false,true);
	else if (event.pane->name == "vars_panel")
		ShowVarsPanel(false,true);
	else if (event.pane->name == "opers_panel")
		ShowOpersPanel(false,true);
	else if (event.pane->name == "quick_html")
		ShowQuickHelp(false);
	else if (event.pane->name == "results_tree")
		ShowResults(false,false);
	else if (event.pane->name == "desktop_test_panel") {
		ShowDesktopTestPanel(false,true);
		debug_panel->OnDesktopTestPanelHide();
//	else if (event.pane->name == "ejercicio") {
//		CloseTestPackage();
	}
}

void mxMainWindow::OnNotebookPageClose(wxAuiNotebookEvent& event)  {
	mxSource *source = (mxSource*)notebook->GetPage(event.GetSelection());
	if (source->GetModify()) {
		int res=wxMessageBox(_Z("Hay cambios sin guardar. ¿Desea guardarlos antes de cerrar el archivo?"), source->filename, wxYES_NO|wxCANCEL|wxICON_QUESTION,this);
		if (res==wxCANCEL) {
			event.Veto();
			return;
		}
		if (res==wxYES) {
			if (source->sin_titulo) {
				wxCommandEvent evt;
				OnFileSaveAs(evt);
				if (source->GetModify()) {
					event.Veto();
					return;
				}
			} else
				source->SaveFile(source->filename);
		}
	}
}

void mxMainWindow::OnSocketEvent(wxSocketEvent &event){
//	if (debug && debug->HasSocket(event.GetEventObject()))
//		debug->SocketEvent(&event);
//	else 
	comm_manager->SocketEvent(event);
}

void mxMainWindow::OnScrollDegugSpeed(wxScrollEvent &evt) {
	config->stepstep_tspeed=evt.GetInt();
	debug->SetSpeed(config->stepstep_tspeed);
}


void mxMainWindow::CreateDesktopTestPanel() {
	desktop_test_panel = new mxDesktopTestPanel(this);
	aui_manager.AddPane(desktop_test_panel, wxAuiPaneInfo().Name("desktop_test_panel").Caption(_Z("Prueba de Escritorio")).CloseButton(true).MaximizeButton(true).Bottom().Hide().Layer(pdkt[0]).Row(pdkt[1]).Position(pdkt[2]));
}

//const wxArrayString &mxMainWindow::GetDesktopVars() {
//	return desktop_test_panel->GetDesktopVars();
//}

//void mxMainWindow::SetDesktopVars(bool do_dt, const wxArrayString &vars) {
//	debug->SetDoDesktopTest(do_dt);
//	desktop_test_grid->SetDesktopVars(vars);
//	ShowDesktopTestGrid(do_dt);
//}

//void mxMainWindow::OnToolbarShowDebugPanel(wxCommandEvent &evt) {
//	if (mi_debug_panel->IsChecked()) {
//		mi_debug_panel->Check(true);
//		aui_manager.GetPane(debug_panel).Show();
//		aui_manager.Update();	
//	} else
//		OnRunStepStep(evt);
//}

void mxMainWindow::SelectLine(mxSource *src, int l) {
	int index = notebook->GetPageIndex(last_source);
	if (index==wxNOT_FOUND) return;
	if (index!=notebook->GetSelection()) notebook->SetSelection(index);
	src->SetSelection(src->GetLineIndentPosition(l-1),src->GetLineEndPosition(l-1));
}

bool mxMainWindow::SelectFirstError() {
	wxTreeItemIdValue v;
	wxTreeItemId item(results_tree_ctrl->GetFirstChild(results_root,v));
	if (item.IsOk()) {
		results_tree_ctrl->SelectItem(item);
		wxTreeEvent evt(0,main_window->results_tree_ctrl,item);
		main_window->OnSelectError(evt);
		main_window->Raise();
	} else
		return false;
	return true;
}

//void mxMainWindow::OnHelpLogger(wxCommandEvent &evt) {
//	ResetInLogMode();
//}
void mxMainWindow::ResetInLogMode() {
	wxFileDialog dlg (this, _Z("Generar Log"),"","pseint-log.txt","*", wxFD_SAVE);
	if (dlg.ShowModal() != wxID_OK) return;
	wxString dir = config->pseint_dir;
#if defined(__WIN32__)
	wxString bin = "\\wxPSeInt.exe";
#elif defined(__APPLE__)
	wxString bin = "/../MacOS/pseint";
#else
	wxString bin = "/wxPSeInt";
#endif
	wxString command = dir+bin;
	command << " --logger \"" << dlg.GetPath() << "\"";
	config->Save();
	Close(); wxYield();
	if (notebook->GetPageCount()!=0) return;
#if defined(__APPLE__)
	system((command+" &").c_str());
#else
	wxExecute( command ); 
#endif
}
	
void mxMainWindow::OnHelpUpdates(wxCommandEvent &evt) {
	if (mxUpdatesChecker::IsAvailable()) {
		new mxUpdatesChecker(true);
	} else {
		wxString msg = 
			_Z("Esta funcionalidad no se está disponible en esta instalación.\n"
			   "\n"
			   "Puede visitar https://pseint.sourceforge.net para verificar si\n"
			   "existen nuevas versiones (su versión actual es VERSION)."
#if defined(__WIN32__) || defined(__WIN64__)
			   "\n\n"
			   "En sistemas Windows, muchos antivirus confunden el módulo\n"
			   "encargado de realizar esta consulta con un virus e impiden el\n"
			   "funcionamiento de PSeInt, o hasta su descarga; dañando además\n"
			   "la reputación del proyecto.\n"
			   "\n"
			   "Dado que no tengo el tiempo necesario para continuar lidiando\n"
			   "con esta estupidez (ya he perdido demasiados días intentandolo),\n"
			   "y que el volúmen de mensaje de usuarios que me escriben (con\n"
			   "motivos más que razonables) preocupados por las alertas sigue\n"
			   "creciendo, lamentablemente no me ha quedado otra opción."
#endif
			   );
		msg.Replace("VERSION",wxString()<<VERSION);
		wxMessageBox(msg,_Z("Búsqueda de Actualizaciones Deshabilitada"),wxOK|wxICON_EXCLAMATION,this);
	}
}

void mxMainWindow::OnConfigLanguage(wxCommandEvent &evt) {
	if (mxProfile(this).ShowModal()) 
		ProfileChanged();
}

void mxMainWindow::SetAccelerators() {
	wxAcceleratorEntry entries[8];
	entries[0].Set(wxACCEL_CTRL|wxACCEL_SHIFT, WXK_TAB, mxID_VIEW_NOTEBOOK_PREV);
	entries[1].Set(wxACCEL_CTRL, WXK_TAB, mxID_VIEW_NOTEBOOK_NEXT);
	entries[2].Set(wxACCEL_CTRL, WXK_PAGEUP, mxID_VIEW_NOTEBOOK_PREV);
	entries[3].Set(wxACCEL_CTRL, WXK_PAGEDOWN, mxID_VIEW_NOTEBOOK_NEXT);
	entries[4].Set(wxACCEL_CTRL|wxACCEL_SHIFT, WXK_F5, mxID_DO_THAT);
	entries[5].Set(0, WXK_F6, mxID_DEBUG_STEP);
	entries[6].Set(wxACCEL_ALT|wxACCEL_SHIFT, WXK_RETURN, mxID_VARS_RENAME);
	entries[7].Set(wxACCEL_CTRL, '0', mxID_ZOOM_TO_SELECTION);
	wxAcceleratorTable accel(8, entries);
	SetAcceleratorTable(accel);
}

void mxMainWindow::OnViewZoomToSelection(wxCommandEvent &evt){
	IF_THERE_IS_SOURCE { CURRENT_SOURCE->ZoomToSelection(); }	
}

void mxMainWindow::OnViewNotebookNext(wxCommandEvent &evt){
	unsigned int i=notebook->GetSelection();
	i++; 
	if (i>=notebook->GetPageCount())
		i=0;
	notebook->SetSelection(i);
}

void mxMainWindow::OnViewNotebookPrev(wxCommandEvent &evt){
	int i=notebook->GetSelection();
	i--; 
	if (i<0)
		i=notebook->GetPageCount()-1;
	notebook->SetSelection(i);
}

void mxMainWindow::OnDoThat (wxCommandEvent &event) {
	LangSettings custom_lang = cfg_lang;
	if (mxConfig(this,custom_lang).ShowModal() && custom_lang!=cfg_lang) {
		config->SetProfile(custom_lang);
		ProfileChanged();
	}
}

void mxMainWindow::OnFilePrint (wxCommandEvent &event) {
	IF_THERE_IS_SOURCE {
		static wxPrintDialogData printDialogData;
		wxPrinter printer(&printDialogData);
		mxPrintOut printout(CURRENT_SOURCE,CURRENT_SOURCE->GetPageText());
		if (!printer.Print(this, &printout, true)) {
			if (wxPrinter::GetLastError() == wxPRINTER_ERROR)
				wxMessageBox(_Z("Ha ocurrido un error al intentar imprimir"),_Z("Error"),wxOK|wxICON_ERROR);
		}
	}
}

void mxMainWindow::OnLink (wxHtmlLinkEvent &event) {
	if (event.GetLinkInfo().GetHref().StartsWith("example:")) {
		main_window->OpenProgram(DIR_PLUS_FILE(config->examples_dir,event.GetLinkInfo().GetHref().Mid(8)),true);
		if (IsMaximized()) Maximize(false);
		main_window->Raise();
	} else if (event.GetLinkInfo().GetHref().StartsWith("goto-error:")) {
		long l; if(!event.GetLinkInfo().GetHref().AfterFirst(':').ToLong(&l)) return;
		if (l<0||l>=int(results_tree_errors.GetCount())) return;
		SelectError(results_tree_errors[l]);
	} else {
		if (!helpw) helpw = new mxHelpWindow();
		helpw->ShowHelp(event.GetLinkInfo().GetHref());
	}
}


void mxMainWindow::OnRunSetInput (wxCommandEvent & evt) {
	IF_THERE_IS_SOURCE {
		mxSource *src=CURRENT_SOURCE;
		if (!src->input) src->input=new mxInputDialog(this);
		src->input->Show();
	}
}

void mxMainWindow::OnFileEditFlow (wxCommandEvent & evt) {
	IF_THERE_IS_SOURCE {
		mxSource *source = CURRENT_SOURCE;
		if (source->GetFlowSocket()) { 
			source->GetFlowSocket()->Write("raise\n",6); 
			return;
		}
		wxString fname=source->SaveTemp();
		if (debug->debugging)
			debug->Stop();
		else {
			mxProcess *flow=new mxProcess(source);
			if (!flow->DrawAndEdit(fname,true)) 
				delete flow;
		}
	}	
}

mxSource * mxMainWindow::FindSourceById (int id) {
	if (id<=0) return NULL;
	for(unsigned int i=0;i<notebook->GetPageCount();i++)
		if (((mxSource*)(notebook->GetPage(i)))->GetId()==id) 
			return ((mxSource*)(notebook->GetPage(i)));
	return NULL;
}

mxSource * mxMainWindow::FindFlowSocket (wxObject *s) {
	if (!s) return NULL;
	for(unsigned int i=0;i<notebook->GetPageCount();i++)
		if (((mxSource*)(notebook->GetPage(i)))->GetFlowSocket()==s) 
			return ((mxSource*)(notebook->GetPage(i)));
	return NULL;
}

void mxMainWindow::SelectSource (mxSource * s) {
	for(unsigned int i=0;i<notebook->GetPageCount();i++)
		if (notebook->GetPage(i)==s) 
			notebook->SetSelection(i);
}

void mxMainWindow::UpdateRealTimeSyntax() {
		if (config->rt_syntax) {
//			for(unsigned int i=0;i<notebook->GetPageCount();i++)
//				((mxSource*)notebook->GetPage(i))->DoRealTimeSyntax();
			IF_THERE_IS_SOURCE 
				CURRENT_SOURCE->DoRealTimeSyntax();
		}
}

void mxMainWindow::OnConfigNassiScheiderman (wxCommandEvent & evt) {
	if (!mi_nassi_shne->IsChecked()) {
		mi_nassi_shne->Check(false);
		config->GetWritableLang()[LS_USE_NASSI_SHNEIDERMAN]=false;
	} else {
		mi_nassi_shne->Check(true);
		config->GetWritableLang()[LS_USE_NASSI_SHNEIDERMAN]=true;
	}
}

void mxMainWindow::CreateButtonSubProceso(wxPanel *panel, wxSizer *sizer){
	if (button_subproc) { sizer->Detach(button_subproc); button_subproc->Destroy(); }
	if (cfg_lang[LS_ENABLE_USER_FUNCTIONS]) {
		wxString but_label = cfg_lang[LS_PREFER_FUNCION]?_Z("Función"):(cfg_lang[LS_PREFER_ALGORITMO]?_Z("SubAlgoritmo"):_Z("SubProceso"));
		button_subproc = utils->AddImgButton(sizer,panel,mxID_CMD_SUBPROCESO,DIR_PLUS_FILE_2("inst",config->big_icons?"52":"35","funcion.png"),but_label);
	} else 
		button_subproc = NULL;
}

void mxMainWindow::ProfileChanged ( ) {
//	mxSource::SetAutocompletion(); // no aca para no hacerlo una vez por fuente... si total los arreglos que llenan son static
//	mxSource::SetCalltips();       // no aca para no hacerlo una vez por fuente... si total los arreglos que llenan son static
	for (unsigned int i=0;i<notebook->GetPageCount();i++) {
		((mxSource*)(notebook->GetPage(i)))->ProfileChanged();
		((mxSource*)(notebook->GetPage(i)))->SetWords();
		((mxSource*)(notebook->GetPage(i)))->Colourise(0,((mxSource*)(notebook->GetPage(i)))->GetLength());
		((mxSource*)(notebook->GetPage(i)))->KillRunningTerminal();
	}
	debug_panel->ProfileChanged();
	mi_nassi_shne->Check(cfg_lang[LS_USE_NASSI_SHNEIDERMAN]);
	if (RTSyntaxManager::IsLoaded()) RTSyntaxManager::Restart();
	CreateButtonSubProceso(commands,commands->GetSizer());
	commands->Layout();
	CreateOpersPanel(); //	opers_window->SetWordOperators();
	status_bar->SetStatus(STATUS_PROFILE);
	aui_manager.Update();
}

mxSource * mxMainWindow::GetCurrentSource ( ) {
	if (notebook->GetPageCount()) return (mxSource*)(notebook->GetPage(notebook->GetSelection()));
	else return NULL;
}

void mxMainWindow::OnNotebookPageChange (wxAuiNotebookEvent & event) {
	event.Skip(); 
	// para que se actualice la lista de variables
	if (notebook->GetPageCount()) {
		mxSource *src=CURRENT_SOURCE;
		if (config->rt_syntax/* && src->IsJustCreated()*/) {
			src->SetStatus();
			src->StartRTSyntaxChecking();
		}
		if (test_panel) {
			if (test_panel->GetSrc()==src) {
				aui_manager.GetPane(test_panel).Show();
			} else {
				aui_manager.GetPane(test_panel).Hide();
			}
			main_window->QuickHelp().HideTestHelp();
			aui_manager.Update();
		}
		main_window->QuickHelp().HideOutput();
	}
}

void mxMainWindow::OnHelperVars (wxCommandEvent & evt) {
	ShowVarsPanel(true,true);
}

void mxMainWindow::OnHelperOpers (wxCommandEvent & evt) {
	ShowOpersPanel(true,true);
}

void mxMainWindow::OnHelperDebug (wxCommandEvent & evt) {
	ShowDebugPanel(true,true);
}

void mxMainWindow::OnHelperCommands (wxCommandEvent & evt) {
	ShowCommandsPanel(true,true);
}

void mxMainWindow::OnDebugShortcut (wxCommandEvent & evt) {
	if (evt.GetId()==mxID_DEBUG_STEP) debug_panel->OnDebugStep(evt);
	IF_THERE_IS_SOURCE CURRENT_SOURCE->SetFocus();
}

void mxMainWindow::CheckIfNeedsRTS() {
	if (config->rt_syntax||config->highlight_blocks||config->show_vars) {
		if (!RTSyntaxManager::IsLoaded()) RTSyntaxManager::Start();
		IF_THERE_IS_SOURCE {
			CURRENT_SOURCE->DoRealTimeSyntax();
			CURRENT_SOURCE->HighLightBlock();
		}
	} else {
		for(unsigned int i=0;i<notebook->GetPageCount();i++)
			((mxSource*)notebook->GetPage(i))->StopRTSyntaxChecking();
		if (RTSyntaxManager::IsLoaded()) RTSyntaxManager::Stop();
	}
}

void mxMainWindow::ShowSubtitles(bool show, bool anim) {
	if (show) ShowPanel(subtitles,anim);
	else HidePanel(subtitles,anim);
}

void mxMainWindow::ShowBacktrace(bool show) {
	if (show) ShowPanel(backtrace,true);
	else HidePanel(backtrace,true);
}

void mxMainWindow::ShowResults(bool show, bool no_error) {
	if (show) {
		results_tree_ctrl->ExpandAll();
		if (no_error) HidePanel(m_quick_help.m_ctrl,false);
		ShowPanel(results_tree_ctrl,!aui_manager.GetPane(m_quick_help.m_ctrl).IsShown());
	} else 
		HidePanel(results_tree_ctrl,!aui_manager.GetPane(m_quick_help.m_ctrl).IsShown());
}

void mxMainWindow::ShowQuickHelp(bool show) {
	m_quick_help.m_visible = show;
	if (show) {
		ShowPanel(m_quick_help.m_ctrl,!aui_manager.GetPane(results_tree_ctrl).IsShown());
	} else {
		HidePanel(m_quick_help.m_ctrl,!aui_manager.GetPane(results_tree_ctrl).IsShown());
		m_quick_help.m_mode = QuickHelpPanelPolicy::QHM_NULL;
		if (test_panel) test_panel->OnShowHideHelp(false);
	}
}

void mxMainWindow::ShowDesktopTestPanel(bool show, bool anim) {
	if (show) ShowPanel(desktop_test_panel,anim);	
	else HidePanel(desktop_test_panel,anim);	
}

void mxMainWindow::ShowDebugPanel (bool show, bool anim) {
	if (show) ShowPanel("helper_debug",debug_panel,anim);
	else HidePanel("helper_debug",debug_panel,anim);
}

void mxMainWindow::ShowVarsPanel (bool show, bool anim) {
	config->show_vars=show; // rt_syntax actualiza o no el arbol de variables segun este bool
	if (show) CheckIfNeedsRTS(); // aunque el codigo ya esté analizado, el arbol está vacio porque si no se muestra no se actualiza
	if (show) ShowPanel("helper_vars",vars_window,anim);
	else HidePanel("helper_vars",vars_window,anim);
}

void mxMainWindow::ShowOpersPanel (bool show, bool anim) {
	if (show) ShowPanel("helper_opers",opers_window,anim);
	else HidePanel("helper_opers",opers_window,anim);
}

void mxMainWindow::ShowCommandsPanel (bool show, bool anim) {
	if (show) ShowPanel("helper_commands",commands,anim);
	else HidePanel("helper_commands",commands,anim);
}

void mxMainWindow::ParseResults(mxSource *source) {
	RTreeReset();
	wxString temp_filename=source->GetTempFilenameOUT();
	wxTextFile fil(temp_filename);
	bool happy_ending = false;
	wxTreeItemId last_item;
	if (fil.Exists()) {
		fil.Open();
		for ( wxString str = fil.GetFirstLine(); !fil.Eof(); str = fil.GetNextLine() ) {
			if (str[0]=='*') {
				if (str.Contains(_Z("Finalizada"))) {
					happy_ending=true;
					if (!_avoid_results_tree) RTreeAdd(source->GetPageText()+_Z(": Ejecución Finalizada"),RTAddType::TreeRoot);
					source->SetStatus(STATUS_RUNNED_OK);
				}
			} else if (str.Len()) {
					RTreeAdd(str,str.AfterFirst(':').StartsWith(" ...")?RTAddType::Level1Node:RTAddType::Level2Node);
			}
		}
		fil.Close();
//		wxRemoveFile(temp_filename);
		if (!happy_ending) {
			source->SetStatus(STATUS_RUNNED_INT);
			RTreeAdd(source->GetPageText()+_Z(": Ejecución Interrumpida"),RTAddType::TreeRoot);
//			Raise(); // comentado porque con la nueva terminal, al presionar f9 se pasa el foco a la terminal, yu si hay error vuelve al editor sin dejar ver que paso
//		} else {
//			source->SetFocus(); // en linux, con wx2+gtk3 hace activa a la ventana, es muy molesto, no llegamos a ver el resultado en la terminal
		}
	}
	RTreeDone(!happy_ending,true);
}

#define _yield Layout(); aui_manager.Update(); wxYield()

void mxMainWindow::ReorganizeForDebugging ( ) {
	if (config->reorganize_for_debug) {
		if (m_fullscreen) SetFullScreen(false);
		int screen_w=wxSystemSettings::GetMetric(wxSYS_SCREEN_X);
		int win_w,win_h;
#ifdef __WIN32__
		GetClientSize(&win_w,&win_h);
#else
		GetSize(&win_w,&win_h);
#endif
		mxSource *src=CURRENT_SOURCE;
		if (src && src->GetFlowSocket()) {
			if (IsMaximized()) Maximize(false);
			_yield;
			SetSize(screen_w/2,win_h);
			Move(screen_w/2,0);
		} else {
			if (!IsMaximized()) Maximize(true);
		}
		ShowDebugPanel(true);
		ShowCommandsPanel(false);
		_yield;
		int notebook_width = notebook->GetSize().GetWidth();
		if (notebook_width-500<400) {
			ShowOpersPanel(false);
			_yield;
			if (notebook_width-500<400) {
				ShowVarsPanel(false);
			}
		}
		if (src && src->GetFlowSocket()) {
			_yield;
//			wxString pos; pos<<"pos ";
//			int notebook_x,notebook_y;
//			notebook->GetScreenPosition(&notebook_x,&notebook_y);
//			pos<<notebook_x<<" "<<notebook_y<<"\n";
//			src->GetFlowSocket()->Write(pos.c_str(),pos.Len());
//			wxString size; size<<"size ";
//			int notebook_width,notebook_height;
//			notebook->GetSize(&notebook_width,&notebook_height);
//			size<<notebook_width-500<<" "<<notebook_height<<"\n";
//			src->GetFlowSocket()->Write(size.c_str(),size.Len());
			
			src->GetFlowSocket()->Write("pos 0 0\n",8);
			wxString size; size<<"size "<<screen_w/2<<" "<<win_h<<"\n";
			src->GetFlowSocket()->Write(size.c_str(),size.Len());
		}
	}
}

void mxMainWindow::OnRTSyntaxAuxTimer (wxTimerEvent & event) {
//	_LOG("mxMainWindow::OnRTSyntaxAuxTimer in");
	RTSyntaxManager::Process();
//	_LOG("mxMainWindow::OnRTSyntaxAuxTimer out");
}

#define _time_ms 250
#define _min_size 1

void mxMainWindow::ShowPanel (wxString helper, wxWindow * panel, bool anim) {
//	IF_THERE_IS_SOURCE CURRENT_SOURCE->Freeze();
	wxAuiPaneInfo &pi=aui_manager.GetPane(panel);
	if (pi.IsShown()) return;
	int final_w = panel->GetSizer()->Fit(panel).GetWidth();
	pi.Show(); 
	if (anim && config->animate_gui) {
		long w = 5; wxStopWatch sw;
		pi.Fixed(); 
		do {
			long d = sw.Time();
			if (d<_time_ms) {
				float x= 1-float(d)/_time_ms;
				w = long((1-x*x)*final_w);
				if (w<5) w=5;
			} else {
				w = final_w;
			}
			pi.MinSize(w,-1);
			pi.MaxSize(w,-1);
			pi.BestSize(w,-1);
			aui_manager.Update(); 
			wxTheApp->Yield(true);
		} while (w!=final_w);
		pi.MinSize(-1,_min_size);
		pi.MaxSize(-1,-1); pi.BestSize(-1,-1);
	} else {
		pi.MinSize(final_w,_min_size);
	}
	pi.Resizable(); 
	if (helper.Len()) aui_manager.GetPane(helper).Hide();	
//	IF_THERE_IS_SOURCE CURRENT_SOURCE->Thaw();
	aui_manager.Update(); 
}

void mxMainWindow::HidePanel(wxString helper, wxWindow * panel, bool anim) {
//	IF_THERE_IS_SOURCE CURRENT_SOURCE->Freeze();
	wxAuiPaneInfo &pi=aui_manager.GetPane(panel);
	if (!pi.IsShown()) return;
	if (anim && config->animate_gui) {
		aui_manager.Update(); 
		int start_w=panel->GetSize().GetWidth();
		pi.Fixed(); 
		long w=5; wxStopWatch sw;
		do {
			long d = sw.Time();
			float x = float(d)/_time_ms;
			w = long(start_w*(1-x*x));
			if (w<5) w=5;
			pi.MinSize(w,_min_size);
			pi.MaxSize(w,-1);
			pi.BestSize(w,-1);
			aui_manager.Update(); 
			wxTheApp->Yield(true);
		} while (w!=5);
	}
	pi.Hide(); 
	if (helper.Len()) aui_manager.GetPane(helper).Show();	
//	IF_THERE_IS_SOURCE CURRENT_SOURCE->Thaw();
	aui_manager.Update(); 
}


void mxMainWindow::ShowPanel (wxWindow * panel, bool anim) {
	wxAuiPaneInfo &pi=aui_manager.GetPane(panel);
	if (pi.IsShown()) return;
	int final_h = panel->GetSizer()?panel->GetSizer()->Fit(panel).GetHeight()
		         :(GetClientSize().GetHeight()*0.3-aui_manager.GetArtProvider()->GetMetric(wxAUI_DOCKART_CAPTION_SIZE));
	pi.Show(); 
	if (anim && config->animate_gui) {
		pi.Fixed(); 
		long h=5; wxStopWatch sw;
		do {
			int d=sw.Time();
			h=(d*final_h)/_time_ms;
			if (h<5) h=5; else if (h>final_h) h=final_h;
			pi.MinSize(_min_size,h);
			pi.MaxSize(-1,h);
			pi.BestSize(-1,h);
			aui_manager.Update(); 
			wxTheApp->Yield(true);
		} while (h!=final_h);
		pi.MinSize(_min_size,final_h);
		pi.MaxSize(-1,-1); pi.BestSize(-1,-1);
	} else {
		pi.MinSize(_min_size,final_h);
	}
	pi.Resizable(); 
	aui_manager.Update(); 
	pi.MinSize(_min_size,final_h/2);
//	IF_THERE_IS_SOURCE CURRENT_SOURCE->SetFocus(); // en linux, con wx3 y gt3, activa la ventana, es muy molesto cuando queda la terminal abierta con un error de tipos en ejecucion
}

void mxMainWindow::HidePanel (wxWindow * panel, bool anim) {
	wxAuiPaneInfo &pi=aui_manager.GetPane(panel);
	if (!pi.IsShown()) return;
	if (anim && config->animate_gui) {
		aui_manager.Update(); 
		int start_h=panel->GetSize().GetHeight();
		pi.Fixed(); 
		long h=5; wxStopWatch sw;
		do {
			int d=sw.Time();
			h=start_h-(d*start_h)/_time_ms;
			if (h<5) h=5;
			pi.MinSize(_min_size,h);
			pi.MaxSize(-1,h);
			pi.BestSize(-1,h);
			aui_manager.Update(); 
			wxTheApp->Yield(true);
		} while (h!=5);
	}
	pi.Hide(); 
	aui_manager.Update(); 
}

void mxMainWindow::RTreeReset ( ) {
	results_tree_errors.Clear();
	result_tree_done = false; 
	result_tree_text_level = 0; 
	results_tree_text.Clear();
	results_tree_ctrl->DeleteChildren(results_root);
	results_last = results_root;
}

wxString mxMainWindow::RTreeAdd_auxHtml(wxString str) {
	bool is_error=str.StartsWith("Lin ");
	if (is_error) main_window->results_tree_errors.Add(str);
	str.Replace("&","&amp;");
	str.Replace(">","&gt;");
	str.Replace("<","&lt;");
	if (is_error)
		return wxString("<A href=\"goto-error:")<<(results_tree_errors.GetCount()-1)<<"\">"<<str<<"</A>";
	return str;
}

/**
* @param type 0=en el root, 1=hijo, 2=hijo del hijo, 3=comentario solo para el html
**/
void mxMainWindow::RTreeAdd (wxString text, RTAddType type, mxSource *source) {
	if (_avoid_results_tree && source) source->MarkError(text);
	switch (type) {
	case RTAddType::TreeRoot:
		results_tree_text=wxString("<B>")<<RTreeAdd_auxHtml(text)<<"</B><BR><BR>"<<results_tree_text;
		results_tree_ctrl->SetItemText(results_root,text);
		break;
	case RTAddType::Level1Node:
		if (result_tree_text_level==0) results_tree_text<<"<UL><LI>";
		else if (result_tree_text_level==2) results_tree_text<<"</LI></UL></LI><LI>";
		else results_tree_text<<"</LI><LI>";
		result_tree_text_level=1;
		results_tree_text<<RTreeAdd_auxHtml(text);
		results_last=results_tree_ctrl->AppendItem(results_root,text,1);
		break;
	case RTAddType::Level2Node:
		if (result_tree_text_level==1) results_tree_text<<"<UL><LI>";
		else results_tree_text<<"</LI><LI>";
		result_tree_text_level=2;
		results_tree_text<<RTreeAdd_auxHtml(text)<<"<BR>";
		results_tree_ctrl->AppendItem(results_last,text,1);
		break;
	case RTAddType::OutOfTree:
		if (result_tree_text_level==2) results_tree_text<<"</LI></UL></LI></UL><BR>";
		else if (result_tree_text_level==1) results_tree_text<<"</LI></UL><BR>";
//		else results_tree_text;
		result_tree_text_level=0;
		results_tree_text<<RTreeAdd_auxHtml(text)<<"<BR>";
		break;
	}	
}

void mxMainWindow::RTreeDone (bool show, bool error) {
	if (result_tree_text_level==2) results_tree_text<<"</LI></UL></LI></UL>";
	else if (result_tree_text_level==1) results_tree_text<<"</LI></UL>";
	result_tree_done=true;
	if (_avoid_results_tree) {
		if (show) {
			if (results_tree_errors.GetCount()==1) {
				SelectError(results_tree_errors[0]);
			} else if (results_tree_text.Len()) {
				QuickHelp().ShowOutput(results_tree_text);
			}
		} else 
			QuickHelp().HideOutput();
	} else {
		ShowResults(show,!error);
		SelectFirstError();
	}
}

// /**
//* @param code 		Indica el texto a mostrar, si es un codigo de error de pseint, muestra la
//*             		ayuda rapida de ese error, sino es alguna de las constantes del enum QH_CODE.
//* @param argument	Si se requieren argumentos (donde esta el error, que archivo de ayuda, 
//*              		etc, van en el segundo parametro)
//**/
//void mxMainWindow::SetQuickHelpText (int code, const wxString &argument, bool force_error) {
//#define _set_quick_help_check_code if (last_code==code) return; last_code=code
//#define _set_quick_help_check_both if (last_code==code && last_argument==argument) return; last_code=code; last_argument=argument
//	static int last_code = QH_NULL;
//	static wxString last_argument;
//	if (code<QH_LASTERR) {
//		if (force_error || (last_code<QH_LASTERR || last_code==QH_RT_NOERROR || last_code==QH_RT_SELECTERROR || last_code==QH_NULL)) { // no pisar ayudas pedidas por el usuario
//			_set_quick_help_check_both;
//			quick_html->SetPage(help->GetErrorText(argument,code));
//		}
//	} else {
//		switch (code) {
//		case QH_HELP_LOAD:
//			_set_quick_help_check_both;
//			quick_html->LoadPage(argument); 
//			break;
//		case QH_HELP_SET:
//			_set_quick_help_check_both;
//			quick_html->SetPage(argument);
//			break;
//		case QH_RT_NOERROR:
//			if (last_code>=QH_LASTERR && last_code!=QH_RT_SELECTERROR) return; // no reemplazar si no era un mensaje de error
//			_set_quick_help_check_code;
//			quick_html->SetPage(_Z("La sintaxis es correcta. Puede presionar F9 para ejecutar el algoritmo."));
//			break;
//		case QH_LINK_SELECTERROR:
//			code = QH_RT_SELECTERROR; last_code=QH_RT_NOERROR;
//		case QH_RT_SELECTERROR:
//			if (last_code>=QH_LASTERR && last_code!=QH_RT_NOERROR) return; // no reemplazar si no era un mensaje de error
//			_set_quick_help_check_code;
//			quick_html->SetPage(_Z("La sintaxis no es correcta. Haga click sobre los errores señalados en el pseudocódigo para más detalles."));
//			break;
//		default:
//			last_code=code; // solo debería pasar para QH_NULL
//		}
//	}
//}

void mxMainWindow::OnKillFocus (wxFocusEvent &event) {
	_LOG("mxMainWindow::OnKillFocus");
//	IF_THERE_IS_SOURCE CURRENT_SOURCE->HideCalltip(true,true);
	IF_THERE_IS_SOURCE CURRENT_SOURCE->FocusKilled();
}

void mxMainWindow::OnActivate(wxActivateEvent&event) {
	if (event.GetActive()) {
		_LOG("mxMainWindow::OnActivated");
	} else {
		_LOG("mxMainWindow::OnDeactivated");
		IF_THERE_IS_SOURCE CURRENT_SOURCE->FocusKilled();
	}
}

void mxMainWindow::OnFileExportPreview (wxCommandEvent & evt) {
	if (export_preview) {
		export_preview->Raise();
		export_preview->UpdatePrev();
	} else {
		export_preview = new mxExportPreview();
		int screen_w = wxSystemSettings::GetMetric(wxSYS_SCREEN_X);
		int win_w, win_h;
	#ifdef __WIN32__
		GetClientSize(&win_w,&win_h);
	#else
		GetSize(&win_w,&win_h);
	#endif
		int x0,y0; GetPosition(&x0,&y0);
		Maximize(false);
		_yield;
		SetSize(screen_w/2,win_h);
		Move(0,y0);
		export_preview->SetSize(screen_w/2,win_h);
		export_preview->Move(screen_w/2,y0);
	}
}

void mxMainWindow::OnInconInstaller (wxCommandEvent & evt) {
#if !defined(__WIN32__) && !defined(__APPLE__)
	new mxIconInstaller(false);
#endif
}

void mxMainWindow::OnSourceClose (mxSource * src) {
	if (last_source==src) src=NULL;
	if (test_panel && test_panel->GetSrc()==src) {
		CloseTestPackage(); aui_manager.Update(); 
		main_window->ShowQuickHelp(false); 
	}
}

void mxMainWindow::QuickHelpPanelPolicy::ShowRTResult (bool errors, bool force) {
	m_aui->GetPane(m_ctrl).Caption("Errores de Sintaxis");
	if ( (!m_visible || (m_mode!=QHM_RT_ERROR && m_mode!=QHM_RT_RESULT) ) && !force) return;
	if (m_mode==QHM_RT_RESULT && errors==(m_last_code!=0)) return;
	m_mode = QHM_RT_RESULT; m_last_code = errors?1:0;
	if (errors) 
		m_ctrl->SetPage(_Z("La sintaxis no es correcta. Haga click sobre los errores señalados en el pseudocódigo para más detalles."));
	else
		m_ctrl->SetPage(_Z("La sintaxis es correcta. Puede presionar F9 para ejecutar el algoritmo."));
	EnsureVisible();
}

void mxMainWindow::QuickHelpPanelPolicy::ShowHelpPage (wxString help_file) {
	m_aui->GetPane(m_ctrl).Caption(_Z("Ayuda Rápida"));
	m_mode = QHM_HELP; m_ctrl->LoadPage(help_file); EnsureVisible();
}

void mxMainWindow::QuickHelpPanelPolicy::ShowHelpText (wxString html_text) {
	m_aui->GetPane(m_ctrl).Caption(_Z("Ayuda Rápida"));
	m_mode = QHM_HELP; m_ctrl->SetPage(html_text); EnsureVisible();
}

void mxMainWindow::QuickHelpPanelPolicy::ShowTestHelp (wxString html_text) {
	m_aui->GetPane(m_ctrl).Caption(_Z("Enunciado del Ejercicio"));
	m_mode = QHM_TEST; m_ctrl->SetPage(html_text); EnsureVisible();
}

void mxMainWindow::QuickHelpPanelPolicy::HideTestHelp () {
	if (m_mode == QHM_TEST) Hide();
}

void mxMainWindow::QuickHelpPanelPolicy::ShowOutput (wxString html_text) {
	m_aui->GetPane(m_ctrl).Caption(_Z("Ayuda Acerda del Error"));
	m_mode = QHM_OUTPUT; m_ctrl->SetPage(html_text); EnsureVisible();
}

void mxMainWindow::QuickHelpPanelPolicy::HideOutput() {
	if (m_mode==QHM_OUTPUT) Hide();
}

void mxMainWindow::QuickHelpPanelPolicy::HideErrors() {
	if (m_mode==QHM_RT_ERROR or m_mode==QHM_OUTPUT) Hide();
}

void mxMainWindow::QuickHelpPanelPolicy::Hide ( ) {
	if (m_visible) main_window->ShowQuickHelp(false);
}

void mxMainWindow::QuickHelpPanelPolicy::EnsureVisible ( ) {
	if (!m_visible) main_window->ShowQuickHelp(true); else m_aui->Update(); // el else es para que actualice el caption del panel
	if (main_window->GetTestPanel()) 
		main_window->GetTestPanel()->OnShowHideHelp(m_mode==QHM_TEST);
}

void mxMainWindow::QuickHelpPanelPolicy::ShowRTError (int code, wxString msg, bool force) {
	m_aui->GetPane(m_ctrl).Caption("Errores de sintaxis");
	if ( (!m_visible || (m_mode!=QHM_RT_ERROR && m_mode!=QHM_RT_RESULT) ) && !force) return;
	if (m_mode==QHM_RT_ERROR && m_last_code==code) return;
	m_mode = QHM_RT_ERROR; m_last_code = code;
	msg = wxString("Error ")<<code<<": "<<(msg.Contains("\n")?msg.BeforeFirst('\n'):msg);
	m_ctrl->SetPage(help->GetErrorText(msg,code)); EnsureVisible();
}

void mxMainWindow::EnableDebugButton (bool enable) {
	toolbar->EnableTool(mxID_RUN_STEP_STEP,enable);
}

void mxMainWindow::SetLambda(void (*lambda_func)()) {
	_LOG("mxMainWindow::SetLambda");	
	if (m_lambda_func) {
		_LOG("mxMainWindow::SetLambda m_lambda_func!=nullptr");	
		m_lambda_func();
	}
	m_lambda_func = lambda_func;
	m_lambda_timer->Start(20,true);
}

void mxMainWindow::OnLambdaTimer(wxTimerEvent &event) {
	_LOG("mxMainWindow::OnLambdaTimer");	
	void (*faux)() = m_lambda_func;
	m_lambda_func = NULL;
	faux();
}

void mxMainWindow::UpdatePSDrawSettings() {
	mi_use_dark_psdraw->Check( config->use_dark_psdraw );
	mi_shape_colors->Check( config->shape_colors );
	mi_psdraw_nocrop->Check( config->psdraw_nocrop );
	mi_nassi_shne->Check( config->GetWritableLang()[LS_USE_NASSI_SHNEIDERMAN] );
}

void mxMainWindow::SetFullScreen(bool on) {
	m_fullscreen = on;
	if (on) aui_manager.GetPane("status_bar").Hide();
	else    aui_manager.GetPane("status_bar").Show();
	aui_manager.Update();
	ShowFullScreen(on,wxFULLSCREEN_ALL&(~wxFULLSCREEN_NOMENUBAR));
}
