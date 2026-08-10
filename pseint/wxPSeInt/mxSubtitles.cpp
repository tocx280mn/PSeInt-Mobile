#include "mxSubtitles.h"
#include <wx/textctrl.h>
#include <wx/sizer.h>
#include "mxArt.h"
#include "DebugManager.h"
#include "mxDebugWindow.h"
#include <iostream>
#include "string_conversions.h"

mxSubtitles *subtitles = NULL;

BEGIN_EVENT_TABLE(mxSubtitles,wxPanel)
	EVT_BUTTON(wxID_OK,mxSubtitles::OnButtonNext)
	EVT_BUTTON(wxID_CANCEL,mxSubtitles::OnButtonStop)
END_EVENT_TABLE()

mxSubtitles::mxSubtitles(wxWindow *parent):wxPanel(parent) {
	wxSizer *sizer=new wxBoxSizer(wxHORIZONTAL);
	wxSizer *sizer_bt=new wxBoxSizer(wxVERTICAL);
	text=new wxTextCtrl(this,wxID_ANY,"",wxDefaultPosition,wxDefaultSize,wxTE_CENTRE|wxTE_READONLY|wxTE_MULTILINE);
	button_next=new wxButton(this,wxID_OK,_Z("*Continuar*"));
	sizer->Add(text,wxSizerFlags().Expand().Proportion(1));
	sizer_bt->Add(button_next,wxSizerFlags().Border(wxALL,10).Proportion(0).Expand());
	sizer->Add(sizer_bt,wxSizerFlags().Center());
	text->SetBackgroundColour(wxColour(255,255,150));
	text->SetForegroundColour(wxColour(0,0,100));
	text->SetFont(wxFont(wxFontInfo(11).Family(wxFONTFAMILY_MODERN).Style(wxFONTSTYLE_NORMAL).Weight(wxFONTWEIGHT_BOLD)));
	SetSizer(sizer);
	SetSize(100,button_next->GetSize().GetHeight()*3);
}

void mxSubtitles::AddMessage (int linea, int inst, wxString msg) {
	wxString s; s<<_Z("Linea ")<<linea<<_Z(", instrucción ")<<inst<<_Z("\n")<<msg;
	messages.Add(s); text->SetValue(s); button_next->SetFocus();
}

void mxSubtitles::OnButtonNext (wxCommandEvent & evt) {
	if (debug_panel->GetState()==DS_FINALIZED) {
		debug_panel->SetSubtitles(false);
		debug_panel->OnDebugButton(evt);
	}
	else debug_panel->OnDebugStep(evt);
}

void mxSubtitles::OnButtonStop (wxCommandEvent & evt) {
	debug_panel->OnDebugButton(evt);
}

void mxSubtitles::Reset ( ) {
	messages.Clear();
	message_counter=0;
}

