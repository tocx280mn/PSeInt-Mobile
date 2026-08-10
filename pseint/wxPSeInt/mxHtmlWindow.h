#ifndef MXHTMLWINDOW_H
#define MXHTMLWINDOW_H
#include <wx/html/htmlwin.h>

class mxHtmlWindow : public wxHtmlWindow {
private:
protected:
public:
	mxHtmlWindow(wxWindow *parent, wxWindowID id, wxPoint  pos, wxSize size);
	DECLARE_EVENT_TABLE();
	void OnRightClick(wxMouseEvent &evt);
	void OnCopy(wxCommandEvent &evt);
};

#endif

