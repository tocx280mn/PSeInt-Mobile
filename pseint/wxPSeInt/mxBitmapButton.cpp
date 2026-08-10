#include <wx/dcmemory.h>
#include <wx/settings.h>
#include "mxBitmapButton.h"
#include "mxUtils.h"
#include "ConfigManager.h"

#if defined(__APPLE__) || defined(__WIN32__)
wxColour *mxBitmapButton::background_colour = NULL;
#endif

mxBitmapButton::mxBitmapButton(wxWindow *parent, wxWindowID id, wxBitmap *abmp, wxString atext, wxSize size) : wxBitmapButton(parent,id,mxBitmapButton::GenerateButtonImage(atext,abmp),wxDefaultPosition,size) {
	bmp = abmp;
	text = atext;
}

wxBitmap mxBitmapButton::GenerateButtonImage(wxString text, wxBitmap *bmp) {
#if defined(__APPLE__) || defined(__WIN32__)
	if (!background_colour)
		background_colour = new wxColour(wxButton(NULL,wxID_ANY,_T("lala")).GetBackgroundColour());
#endif
	wxColour back_color = wxSystemSettings::GetColour( wxSYS_COLOUR_BTNFACE );
	wxBitmap full(200,100,32);
	wxMemoryDC *dc=new wxMemoryDC(full);
	wxRect r;
	dc->SetBackground(wxBrush(back_color));
	dc->Clear();
	int p = text.Find('&');
	if (p==wxNOT_FOUND) p=-1;
	else text.Remove(p++,1);
	wxFont font = wxSystemSettings::GetFont(wxSYS_DEFAULT_GUI_FONT);
//	if (config->big_icons) font.SetPointSize(font.GetPointSize()*1.3);
	dc->SetFont(font);
	dc->SetTextForeground(wxSystemSettings::GetColour(wxSYS_COLOUR_BTNTEXT));
	dc->DrawLabel(wxString(_T(" "))<<text,*bmp,wxRect(wxPoint(0,0),wxPoint(200,100)), wxALIGN_LEFT | wxALIGN_TOP,p,&r);
	r.height = r.height>bmp->GetHeight()?r.height:bmp->GetHeight()+1;
	r.height += r.y+1;
	r.y = 0;
	r.width += /*bmp->GetWidth()+*/r.x+1;	
	r.x = 0;
	delete dc;
	wxMask *m=new wxMask();
	m->Create(full,back_color);
	full.SetMask(m);
#ifdef __WIN32__
	full.SetWidth(r.width);
	full.SetHeight(r.height<bmp->GetHeight()?bmp->GetHeight():r.height);
	return full;
#else
	return wxBitmap(full.GetSubBitmap(r));
#endif
}

