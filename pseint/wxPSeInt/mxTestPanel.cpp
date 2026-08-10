#include <wx/button.h>
#include <wx/sizer.h>
#include "mxTestPanel.h"
#include <wx/stattext.h>
#include "string_conversions.h"
#include "ids.h"
#include "mxSource.h"
#include "ConfigManager.h"
#include "mxProcess.h"
#include "Logger.h"
#include "mxMainWindow.h"
#include "version.h"
#include <wx/fs_mem.h>
#include <wx/msgdlg.h>
#include <cstdlib>
#include "mxArt.h"

BEGIN_EVENT_TABLE(mxTestPanel,wxPanel)
	EVT_BUTTON(mxID_TESTPACK_RUN,mxTestPanel::OnRun)
	EVT_BUTTON(wxID_HELP,mxTestPanel::OnHelp)
END_EVENT_TABLE()
	
bool mxTestPanel::first_time = true;

mxTestPanel::mxTestPanel(wxWindow *parent) : wxPanel(parent,wxID_ANY) {
	
	if (first_time) wxFileSystem::AddHandler(new wxMemoryFSHandler); first_time=false;
	
	src = NULL;
	sizer = new wxBoxSizer(wxHORIZONTAL);
	eval_button = new wxButton (this, mxID_TESTPACK_RUN, _Z("Evaluar..."));
	eval_button->SetBitmap(*bitmaps->buttons.eval);
	label = new wxStaticText(this,wxID_ANY,_Z("Cargando ejercicio..."),wxDefaultPosition,wxDefaultSize);
	sizer->Add(eval_button,wxSizerFlags().Border(wxALL,5));
	sizer->Add(label,wxSizerFlags().Center().Centre().Border(wxALL,5));
	sizer->AddStretchSpacer();
	help_button = new wxButton(this,wxID_HELP,_Z("Mostrar Enunciado"),wxDefaultPosition,wxDefaultSize);
//	help_button->SetBitmap(*bitmaps->buttons.help);
	sizer->Add(help_button,wxSizerFlags().Border(wxALL,5));
	SetSizerAndFit(sizer);
}

bool mxTestPanel::Load (const wxString & path, const wxString &key) {
	this->path=path; this->key=key; src=NULL;
	if (!pack.Load(path,key)) {
		wxMessageBox(_Z("No se pudo cargar correctamente el ejercicio"),_Z("Error"),wxOK|wxICON_ERROR,this);
		return false;
	}
	if (pack.GetConfigInt("version requerida")>PACKAGE_VERSION) {
		wxMessageBox(_Z("Debe actualizar PSeInt para poder abrir este ejercicio"),_Z("Error"),wxID_OK|wxICON_ERROR,this);
		return false;
	}
	if (pack.GetConfigBool("creator")) {
		Run("--create_new_test_package=1");
		return false;
	}
	wxString req_profile = pack.GetConfigStr("perfil requerido");
	if (!req_profile.IsEmpty() && req_profile.Upper()!=wxString(_S2W(cfg_lang.name)).Upper()) {
		wxMessageBox(_ZZ("Debe utilizar el perfil \"")+req_profile+_Z("\" para este ejercicio.\n\nVaya al menú \"Configurar\" y seleccione \"Opciones\ndel lenguaje\" para cambiar su perfil."),_Z("Error"),wxID_OK|wxICON_ERROR,this);
		return false;
	}
	label->SetLabel(_Z(" <- click aquí para evaluar su respuesta"));
	if (pack.GetHelp().IsEmpty()) {
		main_window->QuickHelp().Hide();
		help_button->Hide();
	} else {
		help_button->Show();
	}
	sizer->Layout();
	src = main_window->NewProgram("<Ejercicio>");
	src->SetText(pack.GetBaseSrc());
	src->Analyze();
	return true;
}

void mxTestPanel::OnRun (wxCommandEvent & event) {
	main_window->SelectSource(src); src->SaveTemp();
	Run(src->GetTempFilenamePSC());
}

void mxTestPanel::OnHelp (wxCommandEvent & event) {
	main_window->QuickHelp().ShowTestHelp(GetHelp());
	OnShowHideHelp(true);
}


void mxTestPanel::Run (const wxString & source_fname) {
	wxString cmd = config->pseval_command +" \""+path+"\" \""+(key.Len()?key:"--nokey")+"\" "+ config->pseint_command+" "+mxProcess::GetProfileArgs() + " \""+source_fname+"\"";
	_LOG("mxTestPanel::Run");
	_LOG("    "<<cmd);
#ifdef __APPLE__
	system((cmd+" &").c_str());
#else
	wxExecute(cmd,wxEXEC_ASYNC);
#endif
}

bool mxTestPanel::Destroy ( ) {
	pack.UnloadImages();
	return wxPanel::Destroy();
}

void mxTestPanel::OnShowHideHelp (bool show) {
	help_button->Enable(!show);
}

const wxString mxTestPanel::GetHelp ( ) const {
	wxString s = pack.GetHelp();
	// wxHtmlWindow no reconoce el tag Q
	s.Replace("&quot;","\"",true);
	s.Replace("<q>","\"",true);
	s.Replace("</q>","\"",true);
	return s;
}

