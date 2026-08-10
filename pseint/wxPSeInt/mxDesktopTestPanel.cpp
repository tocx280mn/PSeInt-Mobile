#include <wx/sizer.h>
#include <wx/button.h>
#include <wx/textctrl.h>
#include "mxDesktopTestPanel.h"
#include "ids.h"
#include "mxMainWindow.h"
#include "mxDesktopTestGrid.h"
#include "mxHelpWindow.h"
#include "string_conversions.h"
#include <wx/textdlg.h>

mxDesktopTestPanel *desktop_test_panel=NULL;

BEGIN_EVENT_TABLE(mxDesktopTestPanel, wxPanel)
	EVT_BUTTON(mxID_DESKTOP_LIST_ADD,mxDesktopTestPanel::OnAdd)
	EVT_BUTTON(mxID_DESKTOP_LIST_CLEAR,mxDesktopTestPanel::OnClear)
	EVT_BUTTON(mxID_DESKTOP_LIST_HELP,mxDesktopTestPanel::OnHelp)
END_EVENT_TABLE()

mxDesktopTestPanel::mxDesktopTestPanel(wxWindow *parent) : wxPanel(parent, wxID_ANY) {

	wxBoxSizer *right_sizer = new wxBoxSizer(wxVERTICAL);
	wxBoxSizer *main_sizer = new wxBoxSizer(wxHORIZONTAL);
	
	grid = new mxDesktopTestGrid(this,wxID_ANY);
	main_sizer->Add(grid,wxSizerFlags().Proportion(1).Expand());
	
	buttons=new wxPanel(this,wxID_ANY);
	wxButton *button_add = new wxButton(buttons,mxID_DESKTOP_LIST_ADD,_Z("Agregar..."));
	wxButton *button_clear = new wxButton(buttons,mxID_DESKTOP_LIST_CLEAR,_Z("Limpiar"));
	wxButton *button_help = new wxButton(buttons,mxID_DESKTOP_LIST_HELP,_Z("Ayuda..."));
	right_sizer->Add(button_add,wxSizerFlags().Border(wxALL,5).Proportion(0).Expand());
	right_sizer->Add(button_clear,wxSizerFlags().Border(wxALL,5).Proportion(0).Expand());
	right_sizer->Add(button_help,wxSizerFlags().Border(wxALL,5).Proportion(0).Expand());
	buttons->SetSizerAndFit(right_sizer);
	main_sizer->Add(buttons,wxSizerFlags().Center());
	
	SetSizerAndFit(main_sizer);
	
}

void mxDesktopTestPanel::OnClear(wxCommandEvent &evt) {
	grid->OnClearVars();
}

void mxDesktopTestPanel::OnAdd(wxCommandEvent &evt) {
	grid->AddDesktopVar( wxGetTextFromUser(_Z("Variable o expresión a evaluar:"),_Z("Prueba de Escritorio"),"") );
}

void mxDesktopTestPanel::OnHelp(wxCommandEvent &evt) {
	if (!helpw) helpw = new mxHelpWindow();
	helpw->ShowHelp("prueba.html");
}

void mxDesktopTestPanel::SetEditable (bool can_edit) {
	grid->SetEditable(can_edit);
	buttons->Show(can_edit);
	Layout();
}

