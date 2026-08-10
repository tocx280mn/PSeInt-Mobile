#ifndef MXSINGLECASEWINDOW_H
#define MXSINGLECASEWINDOW_H
#include <wx/frame.h>
#include <wx/listctrl.h>

class wxStyledTextCtrl;

class mxSingleCaseWindow : public wxFrame {
private:
	wxString current_test;
	wxListCtrl *list;
	wxStyledTextCtrl *input;
	wxStyledTextCtrl *output;
	wxStyledTextCtrl *solution;
public:
	void AddCaso(wxString &nombre);
	mxSingleCaseWindow(wxWindow *parent, bool only_first, bool show_solution);
	void Show();
	void Diff();
protected:
	void OnClose(wxCloseEvent &event);
	void OnList(wxListEvent &event);
	void OnCancel(wxCommandEvent &event);
	void OnCopy(wxCommandEvent &event);
	DECLARE_EVENT_TABLE();
};

#endif

