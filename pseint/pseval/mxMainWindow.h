#ifndef MXMAINWINDOW_H
#define MXMAINWINDOW_H
#include <wx/frame.h>
#include <wx/process.h>
#include "Package.h"
#include <wx/listctrl.h>
#include <wx/gauge.h>
#include <wx/timer.h>

class wxStaticText;
class wxListBox;
class wxSizer;
class wxProcess;
class TestCase;
class wxButton;
class Package;

class mxMainWindow : public wxFrame {
	wxStaticText *results_title;
	wxGauge *results_bar;
	wxSizer *sizer;
	wxButton *the_button;
	wxTimer m_timer;
	wxString cmdline;
public:
	mxMainWindow();
	bool Start(const wxString &fname, const wxString &passkey, const wxString &cmdline);
	bool RunAllTests(const wxString &cmdline, bool for_create=false);
	bool RunTest(wxString command, TestCase &test, bool for_create);
	void OnTimer(wxTimerEvent &evt);
private:
	void OnProcessTerminate(wxProcessEvent &event);
	void OnButton(wxCommandEvent &event);
	DECLARE_EVENT_TABLE();
};

#endif

