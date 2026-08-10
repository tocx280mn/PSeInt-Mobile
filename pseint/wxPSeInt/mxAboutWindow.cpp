#include <wx/clipbrd.h>
#include <wx/sizer.h>
#include <wx/textctrl.h>
#include <wx/button.h>
#include <wx/statbmp.h>
#include <wx/stattext.h>

#include "mxAboutWindow.h"
#include "ConfigManager.h"
#include "mxUtils.h"
#include "version.h"
#include <wx/html/htmlwin.h>
#include <wx/msgdlg.h>
#include <wx/textfile.h>
#include "mxHelpWindow.h"
#include <wx/dataobj.h>
#include "mxMainWindow.h"

mxAboutWindow *mxAboutWindow::the_about_win = NULL;

BEGIN_EVENT_TABLE(mxAboutWindow, wxDialog)

	EVT_BUTTON(wxID_OK,mxAboutWindow::OnCloseButton)
	EVT_HTML_LINK_CLICKED(wxID_ANY, mxAboutWindow::OnLink)
	EVT_CLOSE(mxAboutWindow::OnClose)

END_EVENT_TABLE()

#define eggcode \
	"Proceso Dibujo\n\tEscribir \"\";\n\tPara i<-1 Hasta 15 Hacer\n\t\tSegun i Hacer\n\t\t\t6: Escribir \" R   P         @@               ##\";\n" \
	"\t\t\t4: Escribir \" C   G     @@      @@         ######\";\n\t\t\t13: Escribir \"   G .   @@  @@@@@@  @@\";\n\t\t\t11: Escribir \" S I O   @@  @@@@@@  @@     ##########\";\n" \
	"\t\t\t2: Escribir \" C   L                        ##  ##\";\n\t\t\t5: Escribir \" A   S       @@  @@           ######\";\n\t\t\t15: Escribir \"     R   @@    @@    @@\";\n" \
	"\t\t\t8: Escribir \" C R T       @@@@@@       ##  ######  ##\";\n\t\t\t1: Escribir \"     B                       ##     ##\";\n\t\t\t14: Escribir \"   . A     @@@@@@@@@@\";\n" \
	"\t\t\t9: Escribir \" H A .         @@           ##########\";\n\t\t\t10: Escribir \" A C C     @@@@@@@@@@     ##  ######  ##\";\n\t\t\t3: Escribir \" U   O                          ##\";\n" \
	"\t\t\t12: Escribir \"   N M     @@@@@@@@@@     ##    ##    ##\";\n\t\t\t7: Escribir \" A   O       @@@@@@         ##########\";\n\t\tFinSegun\n\tFinPara\n\tEscribir \"\";\nFinProceso"
	
mxAboutWindow::mxAboutWindow(wxWindow *parent) 
	: wxDialog(parent, wxID_ANY, "Acerca de...", wxDefaultPosition, wxDefaultSize, wxALWAYS_SHOW_SB | wxALWAYS_SHOW_SB | wxDEFAULT_FRAME_STYLE | wxSUNKEN_BORDER) 
{
		
	wxBoxSizer *mySizer = new wxBoxSizer(wxVERTICAL);

	wxButton *ok_button = new wxButton (this, wxID_OK, "Cerrar");
	ok_button->SetDefault(); 
	SetEscapeId(wxID_OK);
	
	mySizer->Add(
		//new wxStaticBitmap(this,wxID_ANY, wxBitmap(DIR_PLUS_FILE(config->Files.skin_dir,"about.png"), wxBITMAP_TYPE_PNG))
		html = new wxHtmlWindow(this, wxID_ANY, wxDefaultPosition, wxSize(600,300))
		,wxSizerFlags().Proportion(1).Expand());
	mySizer->Add(ok_button,wxSizerFlags().Expand());
	SetSizerAndFit(mySizer);
	html->SetPage(MakePageText(false));
	Show(); wxYield();
	html->SetPage(MakePageText(true));
}

void mxAboutWindow::OnCloseButton(wxCommandEvent &event){
	Hide();
}

void mxAboutWindow::OnClose(wxCloseEvent &event){
	Hide();
}

wxString mxAboutWindow::MakePageText(bool full) {
	wxString text("<HTML><HEAD><TITLE>PSeInt</TITLE></HEAD><BODY>");
	text<<_Z("<CENTER><TABLE><TR><TD>");
	text<<_Z("<A href=\"lala\"><IMG src=\"")<<DIR_PLUS_FILE(config->images_path,"logo.png")<<_Z("\"/></A>");
	text<<_Z("</TD><TD>");
	text<<_Z("<CENTER><B>Copyleft 2003-2025<BR>por Pablo Novara<BR>");
	text<<_Z("zaskar_84@yahoo.com.ar<BR>");
	text<<_Z("<A href=\"about\">(ver más...)</A><BR><BR>");
	text<<_Z("Este software es Libre y gratuito.<BR>Se distribuye bajo licencia GPL</B><BR>(<A href=\"gpl\">General Public License</A>)");
	text<<_Z("<BR><BR><B><A href=\"pseint\">https://pseint.sourceforge.net</A></B><BR></CENTER>");
	text<<_Z("</TD></TR></TABLE></CENTER><HR><BR>");
	
//	text<<_Z("<A href=\"about\">Click aqui</A> para ver información más detallada sobre la ");
//	text<<_Z("licencia de PSeInt y de las bibliotecas y otros componentes de terceros que utiliza.");
//	text<<_Z("<BR><HR>");
	
	version_info=_Z("Versión general de la instalación: ");
	wxTextFile fil("version"); 
	if (fil.Exists()) {
		fil.Open();
		version_info<<fil.GetFirstLine();;
		fil.Close();
	} else {
		version_info<<_Z("Error: No se pudo determinar.");
	}
	text<<version_info;
	version_info<<_Z("\n");
	
	if (!full) return text;
	
	text<<_Z("<BR><BR> Versiones individuales:<BR>");
	
	text<<_Z("&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;wxPSeInt ")<<VERSION<<"-" ARCHITECTURE ARCH_EXTRA"<BR>";
	
	text<<"&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;"<<GetVersion(config->pseint_command)<<"<BR>";
	text<<"&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;"<<GetVersion(config->psterm_command)<<"<BR>";
	text<<"&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;"<<GetVersion(config->psdraw3_command)<<"<BR>";
	text<<"&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;"<<GetVersion(config->psdrawe_command)<<"<BR>";
	text<<"&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;"<<GetVersion(config->psexport_command)<<"<BR>";
	text<<"&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;"<<GetVersion(config->pseval_command)<<"<BR>";
	
	text<<_Z("<BR><A href=\"copy\">click aquí para copiar esta información al portapapeles</A>");
	text<<_Z("<BR><BR>");
	return text;
}

void mxAboutWindow::OnLink (wxHtmlLinkEvent &event) {
	if (event.GetLinkInfo().GetHref()=="pseint") {
		wxLaunchDefaultBrowser("https://pseint.sourceforge.net");
	} else if (event.GetLinkInfo().GetHref()=="about") {
		if (!helpw) helpw = new mxHelpWindow();
		Close(); wxYield();
		helpw->ShowHelp("about.html");
	} else if (event.GetLinkInfo().GetHref()=="gpl") {
		if (!helpw) helpw = new mxHelpWindow();
		Close(); wxYield();
		helpw->ShowHelp("gpl.html");
	} else if (event.GetLinkInfo().GetHref()=="lala") {
		static int clicks=0;
		if (++clicks==42) {
			version_info=eggcode;
			clicks=0;
		}
	} else if (event.GetLinkInfo().GetHref()=="copy") {
		if (wxTheClipboard->Open()) {
			wxTheClipboard->SetData(new wxTextDataObject(version_info));
			wxTheClipboard->Close();
		}
	}
}



	

wxString mxAboutWindow::GetVersion (wxString exe) {
	wxString retval=utils->GetVersion(exe);
	version_info<<retval<<"\n";
	return retval;
}

void mxAboutWindow::Run (wxWindow *parent) {
	if (!the_about_win) the_about_win=new mxAboutWindow(parent);
	else the_about_win->Show(); 
}
mxAboutWindow::~mxAboutWindow ( ) {
	the_about_win = NULL;
}

