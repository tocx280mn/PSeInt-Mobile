#ifndef MXHELPWINDOW_H
#define MXHELPWINDOW_H

#include <wx/frame.h>
#include <wx/sashwin.h>
#include <wx/html/htmlwin.h>
#include "mxUtils.h"

class wxHtmlWindow;
class wxTextCtrl;
class wxTreeCtrl;
class wxSashLayoutWindow;
class wxBoxSizer;

class mxHelpWindow:public wxFrame {
private:
	DECLARE_EVENT_TABLE();
	wxBoxSizer *bottomSizer;
	wxHtmlWindow *html;
	wxTextCtrl *search_text;
	wxTreeCtrl *tree;
	wxButton *m_button_tree, *m_button_atop, *m_button_index, *m_button_prev, *m_button_next, *m_button_search;
	wxSashLayoutWindow *index_sash;
	HashStringTreeItem items;
	void OnTree(wxTreeEvent &event);
	void OnLink (wxHtmlLinkEvent &event);
	void RepaintButtons();
public:
	void ShowIndex();
	void ShowHelp(wxString file);
	void OnShowTree(wxCommandEvent &evt);
	void OnAlwaysOnTop(wxCommandEvent &evt);
	void OnClose(wxCloseEvent &evt);
	void OnHome(wxCommandEvent &event);
	void OnPrev(wxCommandEvent &event);
	void OnNext(wxCommandEvent &event);
	void OnCopy(wxCommandEvent &event);
	void OnSearch(wxCommandEvent &event);
	void OnSearchAll(wxCommandEvent &event);
	void OnSashDrag (wxSashEvent& event);
	void OnCharHook(wxKeyEvent &evt);
	void OnText(wxCommandEvent &evt);
	void OnMaximize(wxSizeEvent &evt);
	mxHelpWindow(wxString file=_T("index.html"));
};

extern mxHelpWindow *helpw;

#endif

