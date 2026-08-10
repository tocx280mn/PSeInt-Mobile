#include <wx/dcclient.h>
#include <wx/clrpicker.h>
#include <wx/timer.h>
#include <wx/dcmemory.h>

#include "mxSplashScreen.h"
#include "ConfigManager.h"
#include "mxUtils.h"

mxSplashScreen *splash_screen = nullptr;

BEGIN_EVENT_TABLE(mxSplashScreen, wxFrame)
	EVT_PAINT(mxSplashScreen::OnPaint)
	EVT_TIMER(wxID_ANY,mxSplashScreen::OnTimer)
	EVT_LEFT_DOWN(mxSplashScreen::OnMouse)
	EVT_CLOSE(mxSplashScreen::OnClose)
END_EVENT_TABLE()

mxSplashScreen::mxSplashScreen() : 
	wxFrame(nullptr,wxID_ANY,_T("Cargando PSeInt..."),wxDefaultPosition,wxDefaultSize, wxNO_BORDER | wxSTAY_ON_TOP | wxFRAME_NO_TASKBAR),
	m_timer(this->GetEventHandler(),wxID_ANY)
{
	// sin este estilo extra, los dialogos que crea mxMainWindow en su constructor
	// (como el de buscar, o evaluar expresiones) no se van a ver en Windows (tampoco 
	// genera un error, solo no se muestran)... aunque no deberían tener nada que ver 
	// con esta ventana porque debería ser hijos de main_window, y esta se crea antes...
	// algo en wx no anda bien y cuando esta ventana se destruye se "destruyen" (en
	// realidad no se ejecuta el destructor, pero dejan de "funcionar") tambien
	// esos dialogos hijos de main_window
	SetExtraStyle(GetExtraStyle() | wxWS_EX_TRANSIENT);

	splash_screen = this;
	
	m_bmp = wxBitmap(DIR_PLUS_FILE(config->images_path, (config->use_dark_theme 
														 ? "splash_dark.png" 
														 : "splash_light.png") ),wxBITMAP_TYPE_PNG);
	SetSize(m_bmp.GetWidth(),m_bmp.GetHeight());
	if (config->IsFirstRun() or config->maximized) CenterOnScreen(); 
	else Move(config->pos_x+(config->size_x-m_bmp.GetWidth() )/2,
			  config->pos_y +(config->size_y-m_bmp.GetHeight())/2 );
	
	
	Show(true);
	Update(); // Without this, you see a blank screen for an instant (from wxSplashScreen, for WIN32)
	wxYield();
	
	m_timer.Start(3000,true);
}

void mxSplashScreen::OnPaint(wxPaintEvent& WXUNUSED(evt)) {
	wxPaintDC dc(this);
	PrepareDC(dc);
	dc.DrawBitmap(m_bmp, 0, 0, true);
}

void mxSplashScreen::OnTimer(wxTimerEvent &evt) {
	DestroyNow();
}

void mxSplashScreen::OnMouse(wxMouseEvent &evt) {
	DestroyNow();
}

void mxSplashScreen::OnClose(wxCloseEvent &evt) {
	DestroyNow();
}

void mxSplashScreen::DestroyNow ( ) {
	m_timer.Stop();
	Destroy();
	splash_screen = nullptr;
}

