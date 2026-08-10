#ifndef MXPROFILE_H
#define MXPROFILE_H
#include <wx/dialog.h>
#include <wx/textctrl.h>
#include <wx/listctrl.h>
#include "ConfigManager.h"
#include <wx/imaglist.h>
#include <wx/arrstr.h>
#include <wx/vlbox.h>
#include <wx/brush.h>

class mxProfilesListCtrl : public wxVListBox {
	struct ProfInfo {
		wxString name, desc;
		mutable const wxBitmap *icon = nullptr;
	};
	std::vector<ProfInfo> m_full_list;
	std::vector<int> m_filtered;
	wxBitmap m_null_icon;
	// auxiliares para OnDrawItem
	mutable int m_item_heigth = 0, m_text_height = 0;
	wxColour m_color_sel, m_color_back, m_color_front;
	wxFont m_font; wxBrush m_brush_sel, m_brush_norm;
public:
	mxProfilesListCtrl(wxWindow *parent);
	wxString GetCurrentDescription() const;
	wxString GetCurrentName() const;
	void Search(const wxString &text, const wxString &to_sel_name = "");
	void OnDrawItem(wxDC &dc, const wxRect &rect, size_t n) const override;
	wxCoord OnMeasureItem(size_t n) const override;
	~mxProfilesListCtrl();
};

class mxProfile: public wxDialog {
private:
//	wxListCtrl *list;
	mxProfilesListCtrl *list;
	wxTextCtrl *text;
	wxTextCtrl *search;
	wxArrayString perfiles;
	wxArrayString descripciones;
	LangSettings lang;
public:
	mxProfile(wxWindow *parent);
	void OnClose(wxCloseEvent &evt);
	void OnListSelect(wxCommandEvent &evt);
	void OnListActivate(wxCommandEvent &evt);
	void OnOptionsButton(wxCommandEvent &evt);
	void OnLoadButton(wxCommandEvent &evt);
	void OnOkButton(wxCommandEvent &evt);
	void OnCancelButton(wxCommandEvent &evt);
	void OnSearchText(wxCommandEvent &evt);
	void UpdateDetails();
	DECLARE_EVENT_TABLE();
};

#endif

