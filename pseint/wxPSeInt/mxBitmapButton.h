#ifndef MXBITMAPBUTTON_H
#define MXBITMAPBUTTON_H

#include <wx/bmpbuttn.h>
#include <wx/statbmp.h>
#include <wx/bitmap.h>
#include <wx/clrpicker.h>

class mxBitmapButton : public wxBitmapButton {
private:
	static wxColour *background_colour;
	wxBitmap *bmp;
	wxString text;
public:
	mxBitmapButton(wxWindow *parent, wxWindowID id, wxBitmap *abmp, wxString atext, wxSize size = wxDefaultSize);
	static wxBitmap GenerateButtonImage(wxString text, wxBitmap *bmp);
};

#endif

