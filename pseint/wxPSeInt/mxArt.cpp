#include "mxArt.h"
#include "mxUtils.h"

#include <wx/dcmemory.h>
#include <wx/settings.h>
#include <wx/button.h>
#include <wx/msgdlg.h>
#include "ConfigManager.h"
#include <wx/textctrl.h>
#include <wx/sizer.h>
#include <iostream>
	
mxArt *bitmaps = NULL;

mxArt::mxArt(wxString img_dir) {
	
	wxString bpath  = DIR_PLUS_FILE_2(config->images_path,"bot",config->big_icons?"24":"16");
	buttons.ok      = new wxBitmap(DIR_PLUS_FILE(bpath,_T("aceptar.png")),  wxBITMAP_TYPE_PNG);
	buttons.cancel  = new wxBitmap(DIR_PLUS_FILE(bpath,_T("cancelar.png")), wxBITMAP_TYPE_PNG);
	buttons.help    = new wxBitmap(DIR_PLUS_FILE(bpath,_T("ayuda.png")),    wxBITMAP_TYPE_PNG);
	buttons.find    = new wxBitmap(DIR_PLUS_FILE(bpath,_T("buscar.png")),   wxBITMAP_TYPE_PNG);
	buttons.replace = new wxBitmap(DIR_PLUS_FILE(bpath,_T("remplazar.png")),wxBITMAP_TYPE_PNG);
	buttons.options = new wxBitmap(DIR_PLUS_FILE(bpath,_T("opciones.png")), wxBITMAP_TYPE_PNG);
	buttons.load    = new wxBitmap(DIR_PLUS_FILE(bpath,_T("abrir.png")),    wxBITMAP_TYPE_PNG);
	buttons.save    = new wxBitmap(DIR_PLUS_FILE(bpath,_T("guardar.png")),  wxBITMAP_TYPE_PNG);
	buttons.eval    = new wxBitmap(DIR_PLUS_FILE(bpath,_T("eval.png")),     wxBITMAP_TYPE_PNG);
	
}


mxArt::~mxArt() {
	delete buttons.ok;
	delete buttons.cancel;
	delete buttons.help;
	delete buttons.replace;
	delete buttons.find;
	delete buttons.options;
	
//	delete icons.info;
//	delete icons.error;
//	delete icons.warning;
//	delete icons.question;
}
