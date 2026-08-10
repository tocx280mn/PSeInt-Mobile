#include <wx/image.h>
#include "Application.h"
#include "GLstuff.h" // poner este depues de Application, sino genera errores de enlazado en un metodo de wxapp por alguna macro de algun #include
#include "MainWindow.h"
#include "Version.h"
#include "Global.h"
#include "Entity.h"
#include "Comm.h"
#include "Load.h"
#include "../pseint/LangSettings.h"
#include "../wxPSeInt/osdep.h"

LangSettings g_lang(LS_DO_NOT_INIT);

// wxGLCanvas doesn't work on Wayland
#ifdef __linux__
struct FixWaylandAndWXGLProblem { 
	FixWaylandAndWXGLProblem() { 
		setenv("GDK_BACKEND", "x11", 1); 
	} 
} fix_wayland_and_wxgl_problem;
#endif
	
bool mxApplication::OnInit() {
	_handle_version_query("psDraw3",false);
	
	OSDep::AppInit();
	
	g_lang.Reset();
	GlobalInitPre();
	
	int id=-1, port=-1;
	std::string fname;
	for(int i=1;i<argc;i++) { 
		std::string a(argv[i]);
		if (a=="--shapecolors") g_config.shape_colors=true;
		else if (a=="--darktheme") { g_config.dark_theme=true; SetColors(); }
		else if (a=="--noedit") g_state.edit_on=false;
		else if (a=="--nocroplabels") g_config.enable_partial_text=false;
		else if (a=="--bigicons") g_config.big_icons=true;
		else if (a.size()>=5 && a.substr(0,5)=="--id=") {
			id=atoi(a.substr(5).c_str());
		} else if (a.size()>=7 && a.substr(0,7)=="--port=") {
			port=atoi(a.substr(7).c_str());
		} else if (a.substr(0,2)=="--" && g_lang.ProcessConfigLine(a.substr(2))) {
			; // done in lang.ProcessConfigLine
		} else fname=a;
	}
	g_lang.Fix();
	g_config.nassi_shneiderman = g_lang[LS_USE_NASSI_SHNEIDERMAN];
	g_config.alternative_io = g_lang[LS_USE_ALTERNATIVE_IO_SHAPES];
	GlobalInitPost();

	wxImage::AddHandler(new wxPNGHandler);
	GlobalInitGUI();
	
	if (port!=-1 && id!=-1) {
		if (!::Connect(port,id)) g_state.edit_on = false;
	}
	if (fname.length()) Load(fname.c_str());
	else Load();
	
	new MainWindow(wxString("PSDraw - ")<<g_code.start->label.c_str());
	
	return true;
	
}

IMPLEMENT_APP(mxApplication)
