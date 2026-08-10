#include <wx/sizer.h>
#include <wx/textctrl.h>
#include <wx/button.h>
#include <wx/stattext.h>
#include <wx/checkbox.h>
#include <wx/dirdlg.h>
#include <wx/filename.h>
#include <wx/msgdlg.h>
#include <wx/filedlg.h>
#include <wx/settings.h>
#include "mxCreatorWindow.h"
#include "Package.h"
#include "mxMainWindow.h"
#include "Application.h"
#include "../wxPSeInt/string_conversions.h"

enum { mxSTART=wxID_HIGHEST, mxID_INPUT_PATH_TEXT, mxID_INPUT_RELOAD, mxID_INPUT_PATH_BUTTON, mxID_OUTPUT_PATH_BUTTON, mxID_SHOW_ERRORS, mxID_CREATE_BUTTON, mxID_GENERATE_SOLUTIONS, mxID_USE_PASSWORD, mxID_REQUIRE_PROFILE };

BEGIN_EVENT_TABLE(mxCreatorWindow,wxFrame)
	EVT_BUTTON(mxID_INPUT_PATH_BUTTON,mxCreatorWindow::OnInputPath)
//	EVT_BUTTON(mxID_OUTPUT_PATH_BUTTON,mxCreatorWindow::OnOutputPath)
	EVT_BUTTON(mxID_CREATE_BUTTON,mxCreatorWindow::OnCreate)
	EVT_BUTTON(mxID_GENERATE_SOLUTIONS,mxCreatorWindow::OnGenerateSolutions)
	EVT_TEXT(mxID_INPUT_PATH_TEXT,mxCreatorWindow::OnInputChange)
	EVT_BUTTON(mxID_INPUT_RELOAD,mxCreatorWindow::OnInputReload)
	EVT_TIMER(wxID_ANY,mxCreatorWindow::OnTimer)
	EVT_CHECKBOX(mxID_SHOW_ERRORS,mxCreatorWindow::OnShowErrors)
	EVT_CHECKBOX(mxID_USE_PASSWORD,mxCreatorWindow::OnCheckPassword)
	EVT_CHECKBOX(mxID_REQUIRE_PROFILE,mxCreatorWindow::OnCheckProfile)
END_EVENT_TABLE()

mxCreatorWindow::mxCreatorWindow(const wxString &cmdline)
	: wxFrame(NULL,wxID_ANY,_Z("PSeInt - Ejercicio"),wxDefaultPosition,wxDefaultSize,wxDEFAULT_FRAME_STYLE)
	,timer(GetEventHandler())
{
	this->SetBackgroundColour( wxSystemSettings::GetColour( wxSYS_COLOUR_BTNFACE ) );
	
	pseint_command=cmdline;
	wxBoxSizer *main_sizer = new wxBoxSizer(wxVERTICAL);
	
	main_sizer->Add(new wxStaticText(this,wxID_ANY,_Z("Directorio de entrada:")),wxSizerFlags().Border(wxTOP|wxLEFT|wxRIGHT,5));
	wxBoxSizer *input_sizer = new wxBoxSizer(wxHORIZONTAL);
	input_path = new wxTextCtrl(this,mxID_INPUT_PATH_TEXT,_Z(""));
	input_sizer->Add(input_path,wxSizerFlags().Proportion(1).Center().Border(wxLEFT|wxTOP|wxBOTTOM,5));
	wxButton *input_button = new wxButton(this,mxID_INPUT_PATH_BUTTON,_Z(" ... "),wxDefaultPosition,wxDefaultSize,wxBU_EXACTFIT);
	input_sizer->Add(input_button,wxSizerFlags().Border(wxRIGHT|wxTOP|wxBOTTOM,5));
	wxButton *reload_button = new wxButton(this,mxID_INPUT_RELOAD,_Z(" R "),wxDefaultPosition,wxDefaultSize,wxBU_EXACTFIT);
	reload_button->SetToolTip(_Z("Recargar información desde el directorio de entrada actual"));
	input_sizer->Add(reload_button,wxSizerFlags().Border(wxRIGHT|wxTOP|wxBOTTOM,5));
	main_sizer->Add(input_sizer,wxSizerFlags().Proportion(0).Expand());
	
	static1 = new wxStaticText(this,wxID_ANY,_Z("Mensaje en caso de éxito:"));
	main_sizer->Add(static1,wxSizerFlags().Border(wxTOP|wxLEFT|wxRIGHT,5));
	msg_ok = new wxTextCtrl(this,wxID_ANY,_Z(""));
	main_sizer->Add(msg_ok,wxSizerFlags().Expand().Border(wxALL,5));
	static2 = new wxStaticText(this,wxID_ANY,_Z("Mensaje en caso de error:"));
	main_sizer->Add(static2,wxSizerFlags().Border(wxTOP|wxLEFT|wxRIGHT,5));
	msg_bad = new wxTextCtrl(this,wxID_ANY,_Z(""));
	main_sizer->Add(msg_bad,wxSizerFlags().Expand().Border(wxALL,5));
	
	show_errors = new wxCheckBox(this,mxID_SHOW_ERRORS,_Z("Mostrar casos fallidos"));
	main_sizer->Add(show_errors,wxSizerFlags().Border(wxALL,5));
	stop_on_first_error = new wxCheckBox(this,wxID_ANY,_Z("Solo el primero que falle"));
	main_sizer->Add(stop_on_first_error,wxSizerFlags().Border(wxALL,5));
	show_solutions = new wxCheckBox(this,wxID_ANY,_Z("Mostrar soluciones correctas"));
	main_sizer->Add(show_solutions,wxSizerFlags().Border(wxALL,5));
	
	wxBoxSizer *tests_sizer = new wxBoxSizer(wxHORIZONTAL);
	num_tests = new wxTextCtrl(this,wxID_ANY,_Z(""));
	static3 = new wxStaticText(this,wxID_ANY,_Z("Casos de prueba: "));
	tests_sizer->Add(static3,wxSizerFlags().Center().Border(wxLEFT,5));
	tests_sizer->Add(num_tests,wxSizerFlags().Proportion(1).Border(wxRIGHT|wxTOP|wxBOTTOM,5));
	main_sizer->Add(tests_sizer,wxSizerFlags().Proportion(0).Expand());
	
	shuffle_tests = new wxCheckBox(this,wxID_ANY,_Z("Probar casos en orden aleatorio"));
	main_sizer->Add(shuffle_tests,wxSizerFlags().Border(wxALL,5));
	
	base_psc = new wxCheckBox(this,wxID_ANY,_Z("Incluir pseudocódigo base"));
	main_sizer->Add(base_psc,wxSizerFlags().Border(wxALL,5));
	
	help_html = new wxCheckBox(this,wxID_ANY,_Z("Incluir ayuda/enunciado html/markdown"));
	main_sizer->Add(help_html,wxSizerFlags().Border(wxALL,5));
	
	solutions_button = new wxButton(this,mxID_GENERATE_SOLUTIONS,_Z(" Generar soluciones... "));
	main_sizer->Add(solutions_button,wxSizerFlags().Border(wxALL,5));
	
	wxBoxSizer *pass1_sizer = new wxBoxSizer(wxHORIZONTAL);
	chk_password = new wxCheckBox(this,mxID_USE_PASSWORD,_Z("Contraseña: "));
	pass1_sizer->Add(chk_password,wxSizerFlags().Center().Border(wxLEFT,5));
	password1 = new wxTextCtrl(this,wxID_ANY,_Z(""));
	pass1_sizer->Add(password1,wxSizerFlags().Proportion(1).Border(wxRIGHT|wxTOP|wxBOTTOM,5));
	main_sizer->Add(pass1_sizer,wxSizerFlags().Proportion(0).Expand());
	
	wxBoxSizer *profile_sizer = new wxBoxSizer(wxHORIZONTAL);
	chk_profile = new wxCheckBox(this,mxID_REQUIRE_PROFILE,_Z("Requerir perfil: "));
	profile_sizer->Add(chk_profile,wxSizerFlags().Center().Border(wxLEFT,5));
	profile = new wxTextCtrl(this,wxID_ANY,_Z(""));
	profile_sizer->Add(profile,wxSizerFlags().Proportion(1).Border(wxRIGHT|wxTOP|wxBOTTOM,5));
	main_sizer->Add(profile_sizer,wxSizerFlags().Proportion(0).Expand());
	
	chk_new_cypher = new wxCheckBox(this,mxID_SHOW_ERRORS,_Z("Cifrado mejorado (requiere PSeInt v20160401 o superior)"));
	main_sizer->Add(chk_new_cypher,wxSizerFlags().Border(wxBOTTOM|wxLEFT|wxRIGHT,5));
	
	ok_button = new wxButton(this,mxID_CREATE_BUTTON,_Z(" Generar... "));
	main_sizer->Add(ok_button,wxSizerFlags().Right().Border(wxALL,5));
	EnableDisable();
	
	SetSizerAndFit(main_sizer);
	Show(); input_path->SetFocus();
}

void mxCreatorWindow::OnTimer (wxTimerEvent & event) {
	LoadDirectory();
}

void mxCreatorWindow::OnCreate (wxCommandEvent & event) {
	
	if (chk_password->GetValue() && password1->GetValue().IsEmpty()) {
		wxMessageBox(_Z("Debe completar la contraseña"),_Z("Error"),wxOK|wxICON_ERROR,this);
		return;
	}
	
	if (chk_profile->GetValue() && profile->GetValue().IsEmpty()) {
		wxMessageBox(_Z("Debe completar el nombre del perfil requerido"),_Z("Error"),wxOK|wxICON_ERROR,this);
		return;
	}
	
	wxArrayString tests;
	int tests_count = pack.GetNames(tests);
	if (tests_count==0) {
		wxMessageBox(_Z("No hay casos de prueba"),_Z("Error"),wxOK|wxICON_ERROR,this);
		return;
	}
	
	wxString all_output;
	for(unsigned int i=0;i<tests.GetCount();i++) { 
		all_output << pack.GetTest(tests[i]).solution;
	}
	
	if (all_output.IsEmpty()) {
		if (wxNO==wxMessageBox(_Z("Las soluciones están vacías. ¿Continuar igualmente?"),_Z("Advertencia"),wxYES_NO|wxICON_ERROR,this))
			return;
	}
	
	pack.SetConfigStr  ( "mensaje exito" , msg_ok->GetValue() );
	pack.SetConfigStr  ( "mensaje error" , msg_bad->GetValue() );
	pack.SetConfigBool ( "mostrar soluciones" , show_solutions->GetValue() );
	pack.SetConfigStr  ( "mostrar casos fallidos" , show_errors->GetValue()
												?(stop_on_first_error->GetValue()?"primero":"todos")
												:"ninguno" );
	pack.SetConfigBool ( "mezclar casos", shuffle_tests->GetValue() );
	if (chk_profile->GetValue()) pack.SetConfigStr("perfil requerido",profile->GetValue());
	else                         pack.SetConfigStr("perfil requerido","");
	pack.SetConfigInt  ( "version requerida" , chk_profile->GetValue()?PACKAGE_VERSION:PACKAGE_VERSION_NO_PROFILE );

	static wxString fdir;
	static wxString fname;
	wxString fext = chk_password->GetValue()?"psx":"psz";
	wxFileDialog dlg (this, _Z("Guardar"),fdir,fname,wxString(_Z("Ejercicio|*."))+fext, wxFD_SAVE | wxFD_OVERWRITE_PROMPT);
	if (dlg.ShowModal() == wxID_OK) {
		wxFileName file(dlg.GetPath());
		fdir=file.GetPath(); fname=file.GetName();
		wxString ext = file.GetExt().Lower();
		if (ext=="") { file.SetExt(chk_password->GetValue()?"psx":"psz"); }
		else if ( (ext=="psx") != chk_password->GetValue() ) {
			wxMessageBox(_Z("Nombre de archivo incorrecto. La extensión debe ser \".psz\" para\n"
						"ejercicios sin contraseña, y \".psx\" para ejercicios con contraseña."),
						 _Z("Error"),wxOK|wxICON_ERROR,this);
			return;
		}
		if (!pack.Save(file.GetFullPath(),chk_password->GetValue()?password1->GetValue():_Z(""),!chk_new_cypher->GetValue())) {
			wxMessageBox(wxString(_Z("Error al guardar el archivo \""))+file.GetFullPath()+_Z("\"."),_Z("Error"),wxOK|wxICON_ERROR,this);
		} else {
			wxMessageBox(_Z("Ejercicio generado correctamente"),_Z("PSeInt"),wxOK|wxICON_INFORMATION,this);
			pack.SaveConfig( GetFilePath("config.ini") );
		}
	}
}

void mxCreatorWindow::OnInputPath (wxCommandEvent & event) {
	wxDirDialog dlg(this,_Z("Seleccione un directorio"),"");
	if (dlg.ShowModal()==wxID_OK) {
		input_path->SetValue(dlg.GetPath());
		timer.Start(10,true);
	}
}

void mxCreatorWindow::OnInputChange (wxCommandEvent & event) {
	timer.Start(500,true);
}

void mxCreatorWindow::OnShowErrors (wxCommandEvent & event) {
	EnableDisable();	
}

void mxCreatorWindow::OnGenerateSolutions (wxCommandEvent & event) {
	wxString cmd = pseint_command; 
	cmd.Replace("--create_new_test_package=1",wxString("\"")+GetFilePath("solution.psc")+"\"",false);
	(new mxMainWindow())->RunAllTests(cmd,true);
}

void mxCreatorWindow::OnCheckPassword (wxCommandEvent & event) {
	chk_new_cypher->SetValue(chk_password->GetValue());
	EnableDisable();
}

void mxCreatorWindow::OnCheckProfile (wxCommandEvent & event) {
	EnableDisable();
}

void mxCreatorWindow::EnableDisable ( ) {
	
	bool is_ok=pack.IsOk();
	
	msg_ok->Enable(is_ok);
	msg_bad->Enable(is_ok);
	chk_password->Enable(is_ok);
	password1->Enable(is_ok&&chk_password->GetValue());
	chk_profile->Enable(is_ok);
	profile->Enable(is_ok&&chk_profile->GetValue());
	chk_new_cypher->Enable(is_ok&&chk_password->GetValue());
	show_errors->Enable(is_ok);
	show_solutions->Enable(is_ok&&show_errors->GetValue());
	stop_on_first_error->Enable(is_ok&&show_errors->GetValue());
	shuffle_tests->Enable(is_ok);
	solutions_button->Enable(is_ok);
	ok_button->Enable(is_ok);
	static1->Enable(is_ok);
	static2->Enable(is_ok);
	ok_button->Enable(pack.IsOk());
	solutions_button->Enable(pack.IsOk()&&wxFileName::FileExists(GetFilePath("solution.psc")));
	static3->Enable(false);
	num_tests->Enable(false);
	help_html->Enable(false);
	base_psc->Enable(false);
}

wxString mxCreatorWindow::GetFilePath (const wxString &name) {
	wxString ipath = input_path->GetValue();
	if (ipath.EndsWith("\\")||ipath.EndsWith("/")) ipath.RemoveLast();
	return ipath+wxFileName::GetPathSeparator()+name;
}

void mxCreatorWindow::OnInputReload (wxCommandEvent & event) {
	LoadDirectory();
}

void mxCreatorWindow::LoadDirectory ( ) {
	timer.Stop();
	if (!input_path->GetValue().IsEmpty()) {
		if (pack.Load(input_path->GetValue())) {
			wxArrayString names;
			num_tests->SetValue(wxString()<<pack.GetNames(names));
			shuffle_tests->SetValue(pack.GetConfigBool("mezclar casos"));
			show_errors->SetValue(pack.GetConfigStr("mostrar casos fallidos")=="primero"||pack.GetConfigStr("mostrar casos fallidos")=="todos");
			stop_on_first_error->SetValue(pack.GetConfigStr("mostrar casos fallidos")=="primero");
			show_solutions->SetValue(pack.GetConfigBool("mostrar soluciones"));
			msg_bad->SetValue(pack.GetConfigStr("mensaje error"));
			msg_ok->SetValue(pack.GetConfigStr("mensaje exito"));
			base_psc->SetValue(!pack.GetBaseSrc().IsEmpty());
			help_html->SetValue(!pack.GetHelp().IsEmpty());
			profile->SetValue(pack.GetConfigStr("perfil requerido"));
			chk_profile->SetValue(!pack.GetConfigStr("perfil requerido").IsEmpty());
		}
	} else pack.Reset();
	EnableDisable();
}

