#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include <wx/frame.h>

class Canvas;
class wxToolBar;

class MainWindow : public wxFrame {
public:
	MainWindow(wxString title);
	void OnClose(wxCloseEvent &event);
	void OnMouseWheel(wxMouseEvent &event);
	void OnToolOver(wxCommandEvent &event);
	void OnTool(wxCommandEvent &event);
	void OnActivated(wxActivateEvent &event);
	void ToggleFullScreen();
	void AskForExit();
	int MIDtoMO(int mid);
	DECLARE_EVENT_TABLE();
};

extern MainWindow *g_main_window;
#endif

