#include <iostream>
#include <wx/sizer.h>
#include <wx/button.h>
#include <wx/msgdlg.h>
#include <wx/app.h>
#include <wx/checkbox.h>
#include <wx/settings.h>
#include <wx/bitmap.h>
#include <wx/icon.h>
#include "../wxPSeInt/string_conversions.h"
#include "mxFrame.h"
#include "mxConsole.h"
using namespace std;

enum { FRAME_ID_BASE=wxID_HIGHEST+1000, FRAME_ID_PLAY, FRAME_ID_RUN_AGAIN };

BEGIN_EVENT_TABLE(mxFrame,wxFrame)
	EVT_BUTTON(FRAME_ID_PLAY,mxFrame::OnButtonPlay)
	EVT_BUTTON(FRAME_ID_RUN_AGAIN,mxFrame::OnButtonRunAgain)
	EVT_SOCKET(wxID_ANY,mxFrame::OnSocketEvent)
	EVT_SCROLL(mxFrame::OnScroll)
	EVT_CLOSE(mxFrame::OnClose)
	EVT_CHECKBOX(wxID_REPLACE,mxFrame::OnCheckOnTop)
	EVT_ACTIVATE(mxFrame::OnFocus)
END_EVENT_TABLE()

mxFrame::mxFrame(wxString command, int port, int id, bool debug, win_props props)
	: wxFrame(NULL,wxID_ANY,_Z("PSeInt - Ejecutando..."),
			  wxDefaultPosition, wxSize(props.width,props.height),
			  wxDEFAULT_FRAME_STYLE|(props.always_on_top?wxSTAY_ON_TOP:0)) 
{
	
	wxIconBundle bundle;
	wxIcon icon24; icon24.CopyFromBitmap(wxBitmap("imgs/tools/24/ejecutar.png",wxBITMAP_TYPE_PNG)); bundle.AddIcon(icon24);
	wxIcon icon32; icon32.CopyFromBitmap(wxBitmap("imgs/tools/32/ejecutar.png",wxBITMAP_TYPE_PNG)); bundle.AddIcon(icon32);
	SetIcons(bundle);
	
	this->SetBackgroundColour( wxSystemSettings::GetColour( wxSYS_COLOUR_BTNFACE ) );
	
	debug_mode=debug;
	terminated=already_connected=false;
	src_id=id;
	scroll = new wxScrollBar(this,wxID_ANY,wxDefaultPosition,wxDefaultSize,wxSB_VERTICAL);
	console=new mxConsole(this,scroll,props.dark_theme,props.font_name,props.font_size);
	do_not_close = new wxCheckBox(this,wxID_ANY,_Z("No cerrar esta ventana  ")); 
	do_not_close->SetValue(false); do_not_close->Hide();
	stay_on_top = new wxCheckBox(this,wxID_REPLACE,_Z("Siempre visible   ")); 
	stay_on_top->SetValue(props.always_on_top); stay_on_top->Hide();
	play_from_here = new wxButton(this,FRAME_ID_PLAY,_Z(" Ejecutar desde este punto "),wxDefaultPosition,wxDefaultSize,wxBU_EXACTFIT);
	run_again = new wxButton(this,FRAME_ID_RUN_AGAIN,_Z(" Reiniciar "),wxDefaultPosition,wxDefaultSize,wxBU_EXACTFIT);
	is_present=true; play_from_here->Hide(); run_again->Hide();
	
	wxBoxSizer *sizer = new wxBoxSizer(wxHORIZONTAL);
	wxBoxSizer *sizer_in = new wxBoxSizer(wxVERTICAL);
	wxBoxSizer *sizer_buttons = new wxBoxSizer(wxHORIZONTAL);
	sizer_in->Add(console,wxSizerFlags().Proportion(1).Expand());
	sizer_buttons->Add(do_not_close,wxSizerFlags().Center());
	sizer_buttons->Add(stay_on_top,wxSizerFlags().Center());
	sizer_buttons->AddStretchSpacer();
	sizer_buttons->Add(play_from_here);
	sizer_buttons->Add(run_again);
	sizer_in->Add(sizer_buttons,wxSizerFlags().Proportion(0).Expand());
	sizer->Add(sizer_in,wxSizerFlags().Proportion(1).Expand());
	sizer->Add(scroll,wxSizerFlags().Proportion(0).Expand());
	SetSizer(sizer);
	Show();
	if (port!=-1) InitSocket(port); else socket=NULL;
#ifdef __APPLE__
	// en wx para mac, si no abro un puerto no funciona la redireccion de entrada/salida del proceso
	// depurando dentro de wxWidgets-2.8.12, veo que la linea que hace la diferencia (a la cual se llega
	// gracias al connect del socket) es la 198 de src/mac/corefoundation/gsockosx.cpp:
	//     CFRunLoopAddSource(s_mainRunLoop, data->source, kCFRunLoopCommonModes);
	// como no se que argumentos pasarle para llamarla "a mano" sin crear un socket, creo aca
	// un socket que no se usa, pero obliga a hacer esa llamada
	if (port==-1) {
		wxSocketClient *socket=new wxSocketClient(wxSOCKET_NOWAIT);
		socket->SetEventHandler(*(this->GetEventHandler()),wxID_ANY);
		wxIPV4address address;
		address.Hostname("localhost");
		address.Service(80);
		socket->SetNotify(wxSOCKET_LOST_FLAG|wxSOCKET_INPUT_FLAG|wxSOCKET_CONNECTION_FLAG);
		socket->Notify(true);
		socket->Connect(address,false);
	}
#endif
	console->Run(command);
	console->SetFocus();
	if (props.set_left||props.set_right||props.set_bottom||props.set_top) {
		wxYield();
		int x,y,w,h;
		GetScreenPosition(&x,&y); GetSize(&w,&h);
		if (props.set_left||props.set_right) x= props.set_left?props.left:(props.right-w);
		if (props.set_bottom||props.set_top) y= props.set_top?props.top:(props.bottom-h);
		Move(x,y);
	}
}

void mxFrame::OnButtonPlay (wxCommandEvent & evt) {
	terminated=false; SetButton(false,false);
	console->PlayFromCurrentEvent();
	console->SetFocus();
}

void mxFrame::OnButtonRunAgain(wxCommandEvent & evt) {
	console->SetTime(0);
	console->SetRandSeed();
	OnButtonPlay(evt);
}

void mxFrame::InitSocket (int port) {
	socket=new wxSocketClient(wxSOCKET_NOWAIT);
	socket->SetEventHandler(*(this->GetEventHandler()),wxID_ANY);
	wxIPV4address address;
	address.Hostname("localhost");
	address.Service(port);
	socket->SetNotify(wxSOCKET_LOST_FLAG|wxSOCKET_INPUT_FLAG|wxSOCKET_CONNECTION_FLAG);
	socket->Notify(true);
	socket->Connect(address,false);
}

void mxFrame::OnSocketEvent (wxSocketEvent & event) {
#ifdef __APPLE__
	if (event.GetSocket()!=socket) return; // hay un socket demas que no se usa, ver nota en el ctor
#endif
	if (event.GetSocketEvent()==wxSOCKET_CONNECTION) {
		wxString msg("hello-run "); msg<<src_id<<"\n";
		socket->Write(msg.c_str(),msg.Len());
		already_connected=true;
		if (tosend_after_hello.Len()) 
			socket->Write(tosend_after_hello.c_str(),tosend_after_hello.Len());
	} if (event.GetSocketEvent()==wxSOCKET_INPUT) {
		// leer datos y procesar
		char buf[256];
		socket->Read(buf,255);
		int n = socket->LastCount();
		while (n>0) {
			buf[n]='\0';
			char *aux=buf;
			for (int i=0;i<n;i++)
				if (buf[i]=='\n') {
					buf[i]='\0';
					buffer<<aux;
					ProcessCommand();
					buffer=_T("");
					aux=buf+i+1;
				}
			socket->Read(buf,255);
			n = socket->LastCount();
		}
	} if (event.GetSocketEvent()==wxSOCKET_LOST) {
		delete socket;
		socket=NULL;
	}
}

void mxFrame::ProcessCommand ( ) {
	if (buffer=="reload") console->Reload();
	else if (buffer=="raise") { Hide(); wxYield(); Show(); } // Hide+Show es más efectivo que Raise
	else if (buffer=="quit") Close();
	else if (buffer=="dimm") { SetButton(false,false); console->Dimm(); }
}

void mxFrame::OnProcessTerminated ( ) {
//	if (debug_mode) Close();
	terminated=true; if (!debug_mode) SetButton(true,true);
	if (!socket) return;
	wxString msg("terminated\n");
	if (already_connected)
		socket->Write(msg.c_str(),msg.Len());
	else 
		tosend_after_hello<<msg;
	
}

void mxFrame::OnClose (wxCloseEvent & event) {
	console->KillProcess();
	wxExit();
}

void mxFrame::OnScroll (wxScrollEvent & event) {
	console->SetTime(scroll->GetThumbPosition());
	console->SetFocus();
}

void mxFrame::SetIsPresent (bool is) {
	if (is==is_present || debug_mode) return;
	is_present=is;
	if (is_present) SetButton(terminated,true);
	else SetButton(true,false);
}

void mxFrame::OnFocus (wxActivateEvent & evt) {
	if (socket && already_connected && evt.GetActive()) {
		wxString msg("activated\n");
		socket->Write(msg.c_str(),msg.Len());
	}
	evt.Skip();
}

void mxFrame::SendLocation (int line, int inst) {
	if (socket && already_connected) {
		wxString msg("location "); msg<<line<<":"<<inst<<"\n";
		socket->Write(msg.c_str(),msg.Len());
	}
}

void mxFrame::SetButton (bool visible, bool button_again) {
	wxButton *to_show = button_again?run_again:play_from_here;
	wxButton *to_hide = button_again?play_from_here:run_again;
	do_not_close->Show(visible&&terminated);
	stay_on_top->Show(visible&&terminated);
	if (visible) {
		if (to_show->IsShown()) return;
		to_hide->Hide(); to_show->Show();
	} else {
		if (!to_show->IsShown() && !to_hide->IsShown()) return;
		to_hide->Hide(); to_show->Hide();
	}
	Layout();
}

void mxFrame::ShouldClose ( ) {
	if (!do_not_close->GetValue()) Close();
}

void mxFrame::OnCheckOnTop (wxCommandEvent & evt) {
	evt.Skip();
	if (stay_on_top->GetValue())
		SetWindowStyleFlag(GetWindowStyleFlag()|wxSTAY_ON_TOP);
	else
		SetWindowStyleFlag(GetWindowStyleFlag()&(~wxSTAY_ON_TOP));
}

bool mxFrame::GetDoNotClose ( ) {
	return do_not_close->GetValue();
}

bool mxFrame::GetStayOnTop ( ) {
	return stay_on_top->GetValue();
}

void mxFrame::ToggleDoNotClose ( ) {
	do_not_close->SetValue(!do_not_close->GetValue());
}

void mxFrame::ToggleStayOnTop ( ) {
	stay_on_top->SetValue(!stay_on_top->GetValue());
	wxCommandEvent cmd; OnCheckOnTop(cmd);
}

