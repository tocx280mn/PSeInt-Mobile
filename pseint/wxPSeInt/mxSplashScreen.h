#ifndef MXSPLASHSCREEN_H
#define MXSPLASHSCREEN_H
#include <wx/frame.h>
#include <wx/bitmap.h>
#include <wx/timer.h>

/**
* @brief Ventana sin marco y siempre visible para mostrar un png como splash
**/
class mxSplashScreen : public wxFrame {
private:
	wxBitmap m_bmp;
	wxTimer m_timer;
public:
	mxSplashScreen();
	void OnPaint(wxPaintEvent& WXUNUSED(evt));
	void OnMouse(wxMouseEvent &evt);
	void OnTimer(wxTimerEvent &evt); 
	void OnClose(wxCloseEvent &evt);
	void DestroyNow();
	DECLARE_EVENT_TABLE();
};

extern mxSplashScreen *splash_screen;

#endif

