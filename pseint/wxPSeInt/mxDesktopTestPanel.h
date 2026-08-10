#ifndef MX_DESKTOP_TEST_PANEL_H
#define MX_DESKTOP_TEST_PANEL_H
#include <wx/panel.h>

class wxTextCtrl;
class wxButton;
class wxCheckBox;
class mxDesktopTestGrid;

class mxDesktopTestPanel : public wxPanel {
private:
	wxPanel *buttons;
	mxDesktopTestGrid *grid;
	int sel;
public:
	mxDesktopTestPanel(wxWindow *parent);
	void OnAdd(wxCommandEvent &evt);
	void OnClear(wxCommandEvent &evt);
	void OnHelp(wxCommandEvent &evt);
	void SetEditable(bool can_edit);
	DECLARE_EVENT_TABLE();
};

extern mxDesktopTestPanel *desktop_test_panel;

#endif

