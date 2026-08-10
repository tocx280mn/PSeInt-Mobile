#include <wx/textctrl.h>
#include <wx/sizer.h>
#include <wx/filedlg.h>
#include <wx/bmpbuttn.h>
#include <wx/stattext.h>
#include <wx/checkbox.h>
#include "mxInputDialog.h"
#include "mxArt.h"
#include "string_conversions.h"
#include "ConfigManager.h"

BEGIN_EVENT_TABLE(mxInputDialog,wxDialog)
	EVT_CHECKBOX(wxID_FIND,mxInputDialog::OnCheckUse)
	EVT_BUTTON(wxID_OK,mxInputDialog::OnButtonOk)
	EVT_BUTTON(wxID_CANCEL,mxInputDialog::OnButtonCancel)
	EVT_BUTTON(wxID_OPEN,mxInputDialog::OnButtonLoad)
	EVT_BUTTON(wxID_SAVE,mxInputDialog::OnButtonSave)
END_EVENT_TABLE()

mxInputDialog::mxInputDialog(wxWindow *parent):wxDialog(parent,wxID_ANY,"Predefinir entrada",wxDefaultPosition,wxDefaultSize) {
	wxBoxSizer *sizer=new wxBoxSizer(wxVERTICAL);
	wxStaticText *text=new wxStaticText(this,wxID_ANY,_Z(
		"Puede utilizar esta ventana para ingresar los valores que quiere\n"
		"que reciba su algoritmo cuando ejecute las acciones de lectura,\n"
		"para evitar así tener que ingresarlos en cada ejecución. Debe\n"
		"escribirlos uno por linea. Cada linea secorresponderá con la\n"
		"lectura de una variable o de una posición de un arreglo."
		));
	sizer->Add(text,wxSizerFlags().Proportion(0).Expand().Border(wxALL,5));
	m_check_use=new wxCheckBox(this, wxID_FIND, _Z("Utilizar una entrada predefinida para este algoritmo"));
	sizer->Add(m_check_use,wxSizerFlags().Proportion(0).Expand().Border(wxALL,5));
	m_values = new wxTextCtrl(this, wxID_ANY, "", wxDefaultPosition, wxSize(200,200), wxTE_MULTILINE);
	sizer->Add(m_values,wxSizerFlags().Proportion(1).Expand().Border(wxALL,5));
	m_check_partial=new wxCheckBox(this, wxID_ANY, _Z("Si faltan valores, continuar leyendo desde teclado normalmente."));
	sizer->Add(m_check_partial,wxSizerFlags().Proportion(0).Expand().Border(wxALL,5));
	
	m_check_partial->SetValue(true);
	m_check_partial->Enable(false);
	m_values->Enable(false);
	
	wxBoxSizer *button_sizer = new wxBoxSizer(wxHORIZONTAL);
	
	wxBitmapButton *load_button = new wxBitmapButton(this, wxID_OPEN, *bitmaps->buttons.load);
	load_button->SetToolTip("Cargar entradas desde archivo de texto");
	wxBitmapButton *save_button = new wxBitmapButton(this, wxID_SAVE, *bitmaps->buttons.save);
	save_button->SetToolTip("Guardar entradas en un archivo de texto");
	
	
	wxButton *ok_button = new wxButton (this, wxID_OK, _T("Aceptar"));
	ok_button->SetBitmap(*bitmaps->buttons.ok);
	wxButton *cancel_button = new wxButton (this, wxID_CANCEL, _T("Cancelar"));
	cancel_button->SetBitmap(*bitmaps->buttons.cancel);
	button_sizer->Add(load_button,wxSizerFlags().Border(wxALL,5).Proportion(0).Expand());
	button_sizer->Add(save_button,wxSizerFlags().Border(wxALL,5).Proportion(0).Expand());
	button_sizer->AddStretchSpacer();
	button_sizer->Add(cancel_button,wxSizerFlags().Border(wxALL,5).Proportion(0).Expand());
	button_sizer->Add(ok_button,wxSizerFlags().Border(wxALL,5).Proportion(0).Expand());
	ok_button->SetDefault();
	SetEscapeId(wxID_CANCEL);
	sizer->Add(button_sizer,wxSizerFlags().Expand());
	
	SetSizerAndFit(sizer);
}


void mxInputDialog::OnCheckUse (wxCommandEvent & evt) {
	evt.Skip();
	m_check_partial->Enable(m_check_use->GetValue());
	m_values->Enable(m_check_use->GetValue());
}

void mxInputDialog::OnButtonOk (wxCommandEvent & evt) {
	Hide();
}

void mxInputDialog::OnButtonCancel (wxCommandEvent & evt) {
	m_values->SetValue(m_old_values);
	Hide();
}

void mxInputDialog::Show() {
	m_old_values = m_values->GetValue();
	if (m_check_use->GetValue())
		m_values->SetFocus();
	else
		m_check_use->SetFocus();
	ShowModal();
}

bool mxInputDialog::HasInput ( ) {
	return m_check_use->GetValue();
}

bool mxInputDialog::IsPartial ( ) {
	return m_check_partial->GetValue();
}

wxString mxInputDialog::GetInput ( ) {
	return m_values->GetValue();
}

void mxInputDialog::OnButtonLoad (wxCommandEvent & evt) {
	wxFileDialog dlg (this, _Z("Cargar entradas desde archivo"), config->last_dir, _Z(" "), _Z("Todos los archivos|*"), wxFD_OPEN | wxFD_FILE_MUST_EXIST);
	if (dlg.ShowModal() == wxID_OK and m_values->LoadFile(dlg.GetPath())) {
		m_check_use->SetValue(true); m_values->Enable(true);
	}
}

void mxInputDialog::OnButtonSave (wxCommandEvent & evt) {
	wxFileDialog dlg (this, _Z("Guardar entradas en archivo"), config->last_dir, _Z(" "), _Z("Todos los archivos|*"), wxFD_SAVE | wxFD_OVERWRITE_PROMPT);
	if (dlg.ShowModal() == wxID_OK) m_values->SaveFile(dlg.GetPath());
}

