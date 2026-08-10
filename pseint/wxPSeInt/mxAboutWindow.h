#ifndef MX_ABOUT_WINDOW_H
#define MX_ABOUT_WINDOW_H

#include <wx/dialog.h>
#include <wx/html/htmlwin.h>

class mxAboutWindow : public wxDialog {
	wxString version_info;
	wxString GetVersion(wxString exe);
	static mxAboutWindow *the_about_win;
public:
	mxAboutWindow(wxWindow *parent);
	~mxAboutWindow();
	void OnCloseButton(wxCommandEvent &event);
	void OnLink (wxHtmlLinkEvent &event);
	void OnClose(wxCloseEvent &event);
	wxString MakePageText(bool full);
	wxHtmlWindow *html;
	static void Run(wxWindow *parent);
private:
	DECLARE_EVENT_TABLE()

};

#endif
