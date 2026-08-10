#include <iostream>
#include <cstdlib>
#include <wx/app.h>
#include <wx/bitmap.h>
#include <wx/dcmemory.h>
#include <wx/dcsvg.h>
#include <wx/image.h>
#include <wx/msgdlg.h>
#include <wx/numdlg.h>
#include <wx/filedlg.h>
#include <wx/filename.h>
#include <wx/choicdlg.h>
#include "../psdraw3/Load.h"
#include "../psdraw3/Global.h"
#include "../psdraw3/Entity.h"
#include "../pseint/LangSettings.h"
#include "../wxPSeInt/osdep.h"
#include "Version.h"
#include "mxConfig.h"
#include "../wxPSeInt/string_conversions.h"
#include "../psdraw3/EntityEditor.h"
#include "mxRenderer.h"

void ProcessMenu(int) {}

class mxApplication : public wxApp {
public:
	bool OnInit() override;
	int OnRun() override {
		// hack hasta que se publique wx 2.8 y tengamos SetExitErrorCode...
		
//		wxApp::OnRun(); // no hay ventana, pero entra en el main loop igual y ya no sale
		
		// se ejecuta cuando OnInit retorna true, deberiamos llamar al OnRun
		// original, pero lo reescribo para poder definir codigo de salida del
		// programa como 0 (sino por defecto es otra cosa segun s.o.)
		return 0; 
	}
};

IMPLEMENT_APP(mxApplication)
	
#if (wxUSE_LIBPNG==1)
#	define _IF_PNG(x) x
#else
#	define _IF_PNG(x)
#endif
#if (wxUSE_LIBJPEG==1)
#	define _IF_JPG(x) x
#else
#	define _IF_JPG(x)
#endif
	
LangSettings g_lang(LS_DO_NOT_INIT);

void SetModified() {}

bool mxApplication::OnInit() {
	
	_handle_version_query("psDrawE",false);
	
	OSDep::AppInit();
	
	if (argc==1) {
		std::cerr<<"Use: "<<argv[0]<<" [--use_nassi_shneiderman=1] [--use_alternative_io_shapes=1] [--shape_colors] [--nogui] <input_file> <output_file>"<<std::endl;
	}

	g_lang.Reset();
	
	_IF_PNG(wxImage::AddHandler(new wxPNGHandler));
	_IF_JPG(wxImage::AddHandler(new wxJPEGHandler));
	wxImage::AddHandler(new wxBMPHandler);
	
	// cargar el diagrama
	bool nogui = false;
	g_config.enable_partial_text=false;
	g_config.show_comments=true;
	wxString fin,fout;
	for(int i=1;i<argc;i++) { 
		wxString arg(argv[i]);
		if (arg=="--nogui") {
			nogui = true;
		} else if (arg=="--shapecolors") {
			g_config.shape_colors=true;
		} else if (arg=="--nocroplabels") {
			; // siempre es asi, parsear esto es solo para que no genere error
		} else if (arg.StartsWith("--") && g_lang.ProcessConfigLine(_W2S(arg.Mid(2)))) {
			; // procesado en lang.ProcessConfigLine
		} else if (arg.Len()) {
			if (fin.Len()) fout=arg;
			else { fin=arg; fout=wxFileName(fin).GetName(); }
		}
	}
	g_lang.Fix();
	g_config.nassi_shneiderman = g_lang[LS_USE_NASSI_SHNEIDERMAN];
	g_config.alternative_io = g_lang[LS_USE_ALTERNATIVE_IO_SHAPES];
	GlobalInitPre(); GlobalInitPost();
	if (!Load(fin.ToStdString())) {
		wxMessageBox(_Z("Error al leer pseudocódigo")); return false;
	}
	g_state.edit_on=false;
	if (nogui) mxConfig();
	else if ((new mxConfig())->ShowModal()==wxID_CANCEL) return 0; // opciones del usuairo
	
	// calcular tamaño total
	int h=0,wl=0,wr=0, margin=10;
	Entity *real_start = g_code.start->GetTopEntity();
	real_start->Calculate(wl,wr,h); 
	int x0=real_start->x-wl,y0=real_start->y,x1=real_start->x+wr,y1=real_start->y-h;
	real_start->Calculate();
	
	// hacer que las entidades tomen sus tamaños ideales
	Entity::AllIterator it = Entity::AllBegin();
	while (it!=Entity::AllEnd()) {
		it->Tick();
		++it;
	}
	
	// guardar
	if (not nogui) {
		wxFileName fn(fout);
		wxFileDialog fd(NULL,_Z("Guardar imagen"),fn.GetPath(),fn.GetName(),
						_Z( "Formatos soportados | *.svg;*.SVG" _IF_PNG(";*.png;*.PNG") _IF_JPG(";*.jpg;*.jpeg;*.JPG;*.JPEG") ";*.bmp;*.BMP | " 
						    _IF_PNG("Imagen PNG|*.png;*.PNG|")
						    _IF_JPG("Imagen jpeg|*.jpg;*.jpeg;*.JPG;*.JPEG|")
							"Imagen BMP|*.bmp;*.BMP|"
						    "Dibujo SVG|*.svg;*.SVG"),
						wxFD_SAVE|wxFD_OVERWRITE_PROMPT);
		if (fd.ShowModal()!=wxID_OK) { return false;  }
		fout = fd.GetPath();
	}

	// generar el bitmap
//	int margin=10;
	int bw=((x1-x0)+2*margin)*g_view.zoom;
	int bh=((y0-y1)/*+2*margin*/)*g_view.zoom;
	bool svg = fout.Lower().EndsWith(".svg");
	wxBitmap bmp;
	if (svg) {
		rndr.setDC(new wxSVGFileDC(fout,bw,bh,72,"Diagrama de Flujo"),true);
	} else {
		bmp = wxBitmap(bw,bh);
		rndr.setDC(new wxMemoryDC(bmp),false);
		rndr.clear(g_colors.back);
	}
	
	// dibujar
	Entity *aux=real_start;
	g_constants.line_width_flechas=2*g_view.d_zoom<1?1:int(g_view.d_zoom*2);
	g_constants.line_width_bordes=1*g_view.d_zoom<1?1:int(g_view.d_zoom*1);
	rndr.setWidth(g_constants.line_width_flechas);
	rndr.setTransformation( {wl*g_view.d_zoom+margin*g_view.d_zoom, margin*g_view.d_zoom}, g_view.d_zoom);
	do {
		aux->Draw();
		aux=Entity::NextEntity(aux);
	} while (aux);
	rndr.finish();
	
	wxBitmapType type;
	if (!svg) {
		if (fout.Lower().EndsWith(".bmp")) type=wxBITMAP_TYPE_BMP;
		_IF_PNG(if (fout.Lower().EndsWith(".png")) type=wxBITMAP_TYPE_PNG;)
		_IF_JPG(else if (fout.Lower().EndsWith(".jpg")||fout.Lower().EndsWith(".jpeg")) type=wxBITMAP_TYPE_JPEG;)
		if (!bmp.SaveFile(fout,type)) {
			if (nogui) std::cerr << _Z("Error escribiendo: ")<<fout<<std::endl;
			else wxMessageBox(_Z("No se pudo escribir el archivo"),_Z("PSeInt"));
		}
	}
	if (nogui) std::cerr << _Z("Guardado: ")<<fout<<std::endl;
	else wxMessageBox(_Z("Diagrama guardado"),_Z("PSeInt"));
	
	// SetExitErrorCode(0); no disponible todavía en wx 3.2.6, ver mxApplication::OnRun arriba
	return true;
}
