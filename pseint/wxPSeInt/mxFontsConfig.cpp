#include "mxFontsConfig.h"
#include "mxMainWindow.h"
#include <wx/sizer.h>
#include "string_conversions.h"
#include <wx/stattext.h>
#include <wx/fontenum.h>
#include <wx/textctrl.h>
#include <wx/combobox.h>
#include "mxArt.h"
#include "ConfigManager.h"

BEGIN_EVENT_TABLE(mxFontsConfig,wxDialog)
	EVT_TEXT(wxID_ANY,mxFontsConfig::OnText)
	EVT_COMBOBOX(wxID_ANY,mxFontsConfig::OnCombo)
	EVT_BUTTON(wxID_OK,mxFontsConfig::OnButtonOk)
	EVT_BUTTON(wxID_CANCEL,mxFontsConfig::OnButtonCancel)
END_EVENT_TABLE()

mxFontsConfig::mxFontsConfig() 
	: wxDialog(main_window, wxID_ANY, _Z("Configurar Fuentes"), wxDefaultPosition,wxDefaultSize),
	  m_constructed(false)
{
	
	wxArrayString fonts = wxFontEnumerator::GetFacenames(wxFONTENCODING_SYSTEM,true);
	
	wxStaticText *wx_label = new wxStaticText(this,wxID_ANY,_Z("Fuente del editor:"));
	m_wx_font_name = new wxComboBox(this,wxID_ANY,config->wx_font_name,wxDefaultPosition,wxDefaultSize,fonts);
	m_wx_font_size = new wxTextCtrl(this,wxID_ANY,wxString()<<config->wx_font_size,wxDefaultPosition,wxDefaultSize);
	m_wx_sample = new wxStyledTextCtrl(this,wxID_ANY,wxDefaultPosition,wxDefaultSize,0);
	m_wx_sample->SetText("Ejemplo");
	
	wxStaticText *print_label = new wxStaticText(this,wxID_ANY,_Z("Tamaño de fuente para imprimir:"));
	m_print_font_size = new wxTextCtrl(this,wxID_ANY,wxString()<<config->print_font_size,wxDefaultPosition,wxDefaultSize);
	
	wxStaticText *term_label = new wxStaticText(this,wxID_ANY,_Z("Fuente de la terminal:"));
	m_term_font_name = new wxComboBox(this,wxID_ANY,config->term_font_name,wxDefaultPosition,wxDefaultSize,fonts);
	m_term_font_size = new wxTextCtrl(this,wxID_ANY,wxString()<<config->term_font_size,wxDefaultPosition,wxDefaultSize);
	m_term_sample = new wxStyledTextCtrl(this,wxID_ANY,wxDefaultPosition,wxDefaultSize,0);
	m_term_sample->SetText("Ejemplo");
	
	wxSizerFlags se = wxSizerFlags().Center().Proportion(1);
	wxSizerFlags sc = wxSizerFlags().Center();
	wxSizerFlags sb = wxSizerFlags().Border(wxALL,5).Expand();
	
	wxSizer *main_sizer = new wxBoxSizer(wxVERTICAL);
	wxSizer *wx_sizer = new wxBoxSizer(wxHORIZONTAL);
	wxSizer *print_sizer = new wxBoxSizer(wxHORIZONTAL);
	wxSizer *term_sizer = new wxBoxSizer(wxHORIZONTAL);
	wx_sizer->Add(m_wx_font_name,se);
	wx_sizer->Add(m_wx_font_size,sc);
	print_sizer->Add(print_label,se);
	print_sizer->Add(m_print_font_size,sc);
	term_sizer->Add(m_term_font_name,se);
	term_sizer->Add(m_term_font_size,sc);
	main_sizer->Add(wx_label,sb);
	main_sizer->Add(wx_sizer,sb);
	main_sizer->Add(m_wx_sample,sb);
	main_sizer->Add(print_sizer,sb);
	main_sizer->AddSpacer(5);
	main_sizer->Add(term_label,sb);
	main_sizer->Add(term_sizer,sb);
	main_sizer->Add(m_term_sample,sb);
	main_sizer->AddSpacer(5);
	
	wxBoxSizer *button_sizer = new wxBoxSizer(wxHORIZONTAL);
	wxButton *ok_button = new wxButton (this, wxID_OK, _T("Aceptar"));
	ok_button->SetBitmap(*bitmaps->buttons.ok);
	wxButton *cancel_button = new wxButton (this, wxID_CANCEL, _T("Cancelar"));
	cancel_button->SetBitmap(*bitmaps->buttons.cancel);
	button_sizer->Add(cancel_button,wxSizerFlags().Border(wxALL,5).Proportion(0).Expand());
	button_sizer->Add(ok_button,wxSizerFlags().Border(wxALL,5).Proportion(0).Expand());
	ok_button->SetDefault();
	SetEscapeId(wxID_CANCEL);
	main_sizer->Add(button_sizer,wxSizerFlags().Right());
	
	SetSizerAndFit(main_sizer);
	
	m_constructed = true; ApplyToSamples();
	
}

void mxFontsConfig::OnText (wxCommandEvent & evt) {
	if (m_constructed) ApplyToSamples();
}

void mxFontsConfig::OnCombo (wxCommandEvent & evt) {
	if (m_constructed) ApplyToSamples();
}

void mxFontsConfig::ApplyToSamples ( ) {
	long wx_size = 10, term_size = 11;
	m_wx_font_size->GetValue().ToLong(&wx_size);
	m_term_font_size->GetValue().ToLong(&term_size);
	wxFont wx_font   (wxFontInfo(wx_size  ).Family(wxFONTFAMILY_MODERN).FaceName(m_wx_font_name->GetValue())  );
	wxFont term_font (wxFontInfo(term_size).Family(wxFONTFAMILY_MODERN).FaceName(m_term_font_name->GetValue()));
	m_wx_sample->StyleSetFont (wxSTC_STYLE_DEFAULT, wx_font);
	m_term_sample->StyleSetFont (wxSTC_STYLE_DEFAULT, term_font);
}

void mxFontsConfig::OnButtonOk (wxCommandEvent & evt) {
	m_wx_font_size->GetValue().ToLong(&config->wx_font_size);
	m_term_font_size->GetValue().ToLong(&config->term_font_size);
	m_print_font_size->GetValue().ToLong(&config->print_font_size);
	config->wx_font_name = m_wx_font_name->GetValue();
	config->term_font_name = m_term_font_name->GetValue();
	EndModal(1);
}

void mxFontsConfig::OnButtonCancel (wxCommandEvent & evt) {
	EndModal(0);
}

