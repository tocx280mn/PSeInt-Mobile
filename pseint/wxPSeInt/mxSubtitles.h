#ifndef MXSUBTITLES_H
#define MXSUBTITLES_H
#include <wx/panel.h>

class wxTextCtrl;
class wxBitmapButton;
class wxButton;

class mxSubtitles : public wxPanel {
private:
	wxTextCtrl *text;
//	wxBitmapButton *button_next;
	wxButton *button_next;
//	wxBitmapButton *button_cancel;
	wxArrayString messages;
	int message_counter;
protected:
public:
	void Reset();
	void AddMessage(int linea, int inst, wxString msg);
	mxSubtitles(wxWindow *parent);
	void OnButtonNext(wxCommandEvent &evt);
	void OnButtonStop(wxCommandEvent &evt);
	DECLARE_EVENT_TABLE();
	friend class mxDebugWindow;
};

extern mxSubtitles *subtitles;
#endif

