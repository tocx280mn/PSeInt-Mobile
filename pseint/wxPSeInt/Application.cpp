#include <iostream>
#include <wx/image.h>
#include <wx/socket.h>
#include <wx/filename.h>
#include <wx/msgdlg.h>
#include <wx/textfile.h>
#include "Logger.h"
#include "string_conversions.h"
#include "Application.h"
#include "mxMainWindow.h"
#include "version.h"
#include "mxUtils.h"
#include "ConfigManager.h"
#include "HelpManager.h"
#include "DebugManager.h"
#include "mxProfile.h"
#include "mxArt.h"
#include "mxUpdatesChecker.h"
#include "mxIconInstaller.h"
#include "CommunicationsManager.h"
#include "error_recovery.h"
#include "mxWelcome.h"
#include "mxStatusBar.h"
#include "osdep.h"
#include "mxSplashScreen.h"

IMPLEMENT_APP(mxApplication)

wxSplashScreen *splash;

bool mxApplication::OnInit() {
	
#ifndef DEBUG
	wxDisableAsserts();
#endif
	
	_handle_version_query("wxPSeInt",false);
	
	OSDep::AppInit();
	
//	if (not wxFont::AddPrivateFont("FiraMono-Regular.ttf"))
//		wxMessageBox("foo fm");	
	
	utils = new mxUtils;
	if (argc==3 && wxString(argv[1])=="--logger") new Logger(argv[2]);
#ifdef FORCE_LOG
	new Logger(DIR_PLUS_FILE(wxFileName::GetHomeDir(),"pseint-log.txt"));
#endif
	_LOG("Application::OnInit");
	_LOG("   cwd="<<wxGetCwd());
	_LOG("   argc: "<<argc);
	for(int i=0;i<argc;i++) 
	{ _LOG("      argv["<<i<<"]: "<<argv[i]); }
	
	wxFileName f_path = wxGetCwd(); 
	f_path.MakeAbsolute();
	wxString cmd_path = f_path.GetFullPath();
	wxFileName f_cmd(argv[0]);
	
	wxFileName f_zpath = f_cmd.GetPathWithSep();
	f_zpath.MakeAbsolute();
	wxString zpath(f_zpath.GetPathWithSep());
	bool flag=false;
	if (f_zpath!=f_path) {
		if ( (flag=(wxFileName::FileExists(DIR_PLUS_FILE(zpath,_T("pseint.dir"))) || wxFileName::FileExists(DIR_PLUS_FILE(zpath,_T("PSeInt.dir")))) ) )
			wxSetWorkingDirectory(zpath);
#ifdef __APPLE__
		else if ( (flag=(wxFileName::FileExists(DIR_PLUS_FILE(zpath,_T("../Resources/pseint.dir"))) ||wxFileName::FileExists(DIR_PLUS_FILE(zpath,_T("../Resources/PSeInt.dir")))) ) ) {
			zpath = DIR_PLUS_FILE(zpath,_T("../Resources"));
			wxSetWorkingDirectory(zpath);
		}
#elif !defined(__WIN32__)
		else if ( (flag=(wxFileName::FileExists(DIR_PLUS_FILE(zpath,_T("../pseint.dir"))) ||wxFileName::FileExists(DIR_PLUS_FILE(zpath,_T("../PSeInt.dir")))) ) ) {
			zpath = DIR_PLUS_FILE(zpath,_T(".."));
			wxSetWorkingDirectory(zpath);
		}
#endif
		else 
			zpath = cmd_path;
	}
	if (!flag && !wxFileName::FileExists(_T("pseint.dir")) && !wxFileName::FileExists(_T("PSeInt.dir"))) {
		_LOG("Error: pseint.dir not found");
		wxMessageBox(_Z("PSeInt no pudo determinar el directorio donde fue instalado. Compruebe que el directorio de trabajo actual sea el correcto."),_T("Error"));
	}
	
	srand(time(0));
	
	wxImage::AddHandler(new wxPNGHandler);
	wxImage::AddHandler(new wxXPMHandler);
	
	// load config
	config = new ConfigManager(zpath);
	if (logger) config->Log();

	/*auto splash = */new mxSplashScreen();
	
	wxSocketBase::Initialize();
	
	bitmaps = new mxArt(config->images_path);
	help = new HelpManager;
	
	// create main window
	if (config->size_x<=0 || config->size_y<=0)
		main_window = new mxMainWindow(wxDefaultPosition, wxSize(800, 600));
	else
		main_window = new mxMainWindow(wxPoint(config->pos_x,config->pos_y), wxSize(config->size_x,config->size_y));
	main_window->Maximize(config->maximized);
	main_window->Show(true);
	if (logger || argc==1) {
		if (config->version) main_window->NewProgram();
	} else {
		for (int i=1;i<argc;i++)
			main_window->OpenProgram(DIR_PLUS_FILE(cmd_path,argv[i]));
	}
	SetTopWindow(main_window);
	
	wxYield();	
	main_window->Refresh();

#if !defined(__WIN32__) && !defined(__APPLE__)	
	if (config->version<20120626) 
		new mxIconInstaller(true);
#endif
	
#if defined(__APPLE__) && defined(__STC_ZASKAR)
	wxSTC_SetZaskarsFlags(ZF_FIXDEADKEYS_ESISO);
#endif
	
	status_bar->SetStatus(STATUS_WELCOME);
	
	if (config->IsFirstRun()) {
		_LOG("mxApplication::OnInit NO_PROFILE");
//		wxMessageBox(_Z(
//			"Bienvenido a PSeInt. Antes de comenzar debes seleccionar un perfil "
//			"para ajustar el pseudolenguaje a tus necesidades. Si tu universidad "
//			"o institución no aparece en la lista, notifica a tu profesor para "
//			"que envíe sus datos a través del sitio web. "
//			),_Z("Bienvenido a PSeInt"),wxOK,main_window);
		mxWelcome(main_window).ShowModal();
		main_window->NewProgram();
		main_window->ProfileChanged();
	} else {
//#ifndef DISABLE_UPDATES_CHECKER
		mxUpdatesChecker::BackgroundCheck();
//#else
//		if (config->version && config->version<20190311) {
//			wxMessageBox("A partir de esta versión la búsqueda\n"
//						 "de actualizaciones automática ya no está\n"
//						 "disponible en sistemas Windows debido a\n"
//						 "varios antivirus confunden al módulo que\n"
//						 "se encarga de esta tarea con un virus y\n"
//						 "generan falsas alarmas.\n"
//						 "\n"
//						 "Por favor, para mantener PSeInt actualizado\n"
//						 "y poder disfrutar de las futuras mejoras y\n"
//						 "correcciones, visite regularmente\n"
//						 "https://pseint.sourceforge.net",
//						 "Actualizaciones Desactivadas",wxID_OK|wxICON_EXCLAMATION);
//		}
//#endif
	}
	
	comm_manager=new CommunicationsManager();
	
	RecoverFromError();
	
	return true;
	
}

void mxApplication::RecoverFromError ( ) {
	
	wxTextFile fil(er_get_recovery_fname());	
	if (!fil.Exists()) return;
	
	wxArrayString rec_names, rec_files;
	fil.Open();
	fil.GetFirstLine(); // hora de explosion
	wxString str;
	while (!fil.Eof()) {
		str = fil.GetNextLine(); // nombre de la pestaña
		if (str.Len() && !fil.Eof()) {
			fil.GetNextLine(); // nombre real del archivo antes de la explosion
			rec_names.Add(str);
			rec_files.Add(str=fil.GetNextLine()); // nombre del archivo de segurida generado en la explosion
		}
	}
	fil.Close();
	
	if ( !rec_names.GetCount() ) return;
	wxCommandEvent evt; main_window->OnFileClose(evt);
	for (unsigned int i=0;i<rec_files.GetCount();i++) {
		mxSource *src =	main_window->OpenProgram(rec_files[i],false);
		src->SetPageText(rec_names[i]);
		src->SetModified(true);
		src->sin_titulo = true;
	}
	wxMessageBox(_Z("PSeInt no se cerró correctamente durante su última ejecución.\n"
					"Algunos algoritmos en los que trabajaba fueron guardados,\n"
					"automaticamente y ahora han sido recuperados."),
				_Z("PSeInt - Recuperación ante errores"),wxOK|wxICON_WARNING);
	
	wxRemoveFile(er_get_recovery_fname());
}

