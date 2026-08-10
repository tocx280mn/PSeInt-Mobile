#include "mxWelcome.h"
#include "string_conversions.h"
#include <wx/sizer.h>
#include <wx/stattext.h>
#include <wx/button.h>
#include "ids.h"
#include "mxProfile.h"
#include <wx/filename.h>
#include "mxConfig.h"

BEGIN_EVENT_TABLE(mxWelcome,wxDialog)
	EVT_BUTTON(mxID_WELCOME_LIST,mxWelcome::OnProfileFromList)
	EVT_BUTTON(mxID_WELCOME_FILE,mxWelcome::OnProfileFromFile)
	EVT_BUTTON(mxID_WELCOME_NONE,mxWelcome::OnDefaultProfile)
//	EVT_BUTTON(mxID_WELCOME_,mxWelcome::OnCustomProfile)
	EVT_CLOSE(mxWelcome::OnClose)
END_EVENT_TABLE()

mxWelcome::mxWelcome(wxWindow *parent) 
	: wxDialog(parent,wxID_ANY,_Z("Bienvenido a PSeInt"),wxDefaultPosition,wxDefaultSize)
{
	wxSizer *main_sizer = new wxBoxSizer(wxVERTICAL);
	
	main_sizer->Add(new wxStaticText(this,wxID_ANY,""));
	main_sizer->Add(new wxStaticText(this,wxID_ANY,_Z("Antes de comenzar debe seleccionar un perfil para\n"
													  "ajustar el pseudocódigo a sus necesidades")),wxSizerFlags().Expand().Proportion(0).Border(wxLEFT|wxRIGHT,15));
	
	main_sizer->Add(new wxStaticText(this,wxID_ANY,""));
	main_sizer->Add(new wxButton(this,mxID_WELCOME_LIST,_Z("Opción 1: Perfiles precargados")),wxSizerFlags().Expand().Proportion(0).Border(wxLEFT|wxRIGHT,15));
	main_sizer->Add(new wxStaticText(this,wxID_ANY,_Z("Si utiliza este sofware como parte de un curso, es\n"
													  "probable que ya exista un perfil predefinido por el\n"
													  "docente.")),wxSizerFlags().Expand().Proportion(0).Border(wxLEFT|wxRIGHT,15));
	
	main_sizer->Add(new wxStaticText(this,wxID_ANY,""));
	main_sizer->Add(new wxButton(this,mxID_WELCOME_FILE,_Z("Opción 2: Cargar desde un archivo")),wxSizerFlags().Expand().Proportion(0).Border(wxLEFT|wxRIGHT,15));
	main_sizer->Add(new wxStaticText(this,wxID_ANY,_Z("Utilice esta opción si su docente le ha facilitado\n"
													  "un archivo de perfil.")),
					wxSizerFlags().Expand().Proportion(0).Border(wxLEFT|wxRIGHT,15));
	
	main_sizer->Add(new wxStaticText(this,wxID_ANY,""));
	main_sizer->Add(new wxButton(this,mxID_WELCOME_NONE,_Z("Opción 3: No seleccionar perfil")),wxSizerFlags().Expand().Proportion(0).Border(wxLEFT|wxRIGHT,15));
	main_sizer->Add(new wxStaticText(this,wxID_ANY,_Z("Si solo está explorando el software, esta opción\n"
													  "selecciona el perfil más simple para comenzar.")),wxSizerFlags().Expand().Proportion(0).Border(wxLEFT|wxRIGHT,15));
	
//	main_sizer->AddSpacer(15);
//	main_sizer->Add(new wxButton(this,wxID_ANY,_Z("Personalizar/crear un perfil nuevo")),wxSizerFlags().Expand().Proportion(0).Border(wxLEFT|wxRIGHT,15));
//	main_sizer->Add(new wxStaticText(this,wxID_ANY,_Z(" Utilice esta opción si usted es docente y desea crear\n"
//													  "un nuevo perfil para su curso")),wxSizerFlags().Expand().Proportion(0).Border(wxLEFT|wxRIGHT,15));
	
	
	main_sizer->Add(new wxStaticText(this,wxID_ANY,""));
	SetSizerAndFit(main_sizer);
}

void mxWelcome::OnProfileFromList (wxCommandEvent & e) {
//	Hide(); // si lo oculto, no se vuelve a mostrar
	if (mxProfile(GetParent()).ShowModal())
		EndModal(1);
//	else 
//		Show();
}

void mxWelcome::OnProfileFromFile (wxCommandEvent & e) {
//	Hide(); // si lo oculto, no se vuelve a mostrar
	wxString file = mxConfig::LoadFromFile(GetParent());
	if (file.IsEmpty()) return;
	if (config->LoadProfileFromFile(file)) {
		EndModal(1);
	}
//	Show();
}

void mxWelcome::OnDefaultProfile (wxCommandEvent & e) {
	config->LoadListedProfile(DEFAULT_PROFILE);
	EndModal(1);
}

void mxWelcome::OnClose (wxCloseEvent & evt) {
	wxCommandEvent e;
	OnDefaultProfile(e);
}

