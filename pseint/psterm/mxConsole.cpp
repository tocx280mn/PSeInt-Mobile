#include <iostream>
#include <wx/dcclient.h>
#include <wx/msgdlg.h>
#include <wx/txtstrm.h>
#include <wx/app.h>
#include <wx/menu.h>
#include <wx/clipbrd.h>
#include <wx/dataobj.h>
#include "mxFrame.h"
#include "mxConsole.h"
#include "../wxPSeInt/string_conversions.h"
using namespace std;

enum { CONSOLE_ID_BASE=wxID_HIGHEST, CONSOLE_ID_TIMER_SIZE, CONSOLE_ID_TIMER_CARET, CONSOLE_ID_TIMER_PROCESS, CONSOLE_ID_POPUP_PASTE, CONSOLE_ID_POPUP_COPY, CONSOLE_ID_POPUP_STAY_ON_TOP, CONSOLE_ID_POPUP_CLOSE_AFTER_RUN, CONSOLE_ID_POPUP_RESET, CONSOLE_ID_POPUP_CHANGE_INPUT, CONSOLE_ID_POPUP_FROM_HERE };

BEGIN_EVENT_TABLE(mxConsole,wxPanel)
	EVT_PAINT(mxConsole::OnPaint)
	EVT_SIZE(mxConsole::OnSize)
	EVT_CHAR(mxConsole::OnChar)
	EVT_TIMER(CONSOLE_ID_TIMER_SIZE,mxConsole::OnTimerSize)
	EVT_TIMER(CONSOLE_ID_TIMER_CARET,mxConsole::OnTimerCaret)
	EVT_TIMER(CONSOLE_ID_TIMER_PROCESS,mxConsole::OnTimerProcess)
	EVT_END_PROCESS(wxID_ANY,mxConsole::OnProcessTerminate)
	EVT_MOUSEWHEEL(mxConsole::OnMouseWheel)
	EVT_RIGHT_DOWN(mxConsole::OnMouseRightDown)
	EVT_LEFT_DOWN(mxConsole::OnMouseLeftDown)
	EVT_LEFT_UP(mxConsole::OnMouseLeftUp)
	EVT_LEFT_DCLICK(mxConsole::OnMouseDClick)
	EVT_MOTION(mxConsole::OnMouseMotion)
	EVT_MENU(CONSOLE_ID_POPUP_PASTE,mxConsole::OnPopupPaste)
	EVT_MENU(CONSOLE_ID_POPUP_COPY,mxConsole::OnPopupCopy)
	EVT_MENU(CONSOLE_ID_POPUP_STAY_ON_TOP,mxConsole::OnPopupStayOnTop)
	EVT_MENU(CONSOLE_ID_POPUP_CLOSE_AFTER_RUN,mxConsole::OnPopupCloseAfterRun)
	EVT_MENU(CONSOLE_ID_POPUP_FROM_HERE,mxConsole::OnPopupFromHere)
	EVT_MENU(CONSOLE_ID_POPUP_RESET,mxConsole::OnPopupReset)
	EVT_MENU(CONSOLE_ID_POPUP_CHANGE_INPUT,mxConsole::OnPopupChangeInput)
END_EVENT_TABLE()
	
#define _buffer(i,j) buffer[(i)*buffer_w+(j)]
#define _CARET_TIME 500
#define _SIZE_TIME 100
#define _PROCESS_TIME 10
	
// pseint maneja los colores (16 posibles), esta lista es el mapeo de esos 16 
// en psterm (la primer columna), y sus versiones desvanecidas (las segunda 
// columna) para cuando la terminal quedo desactualizada con respecto al editor, 
// y en ese caso el mensaje se muestra con el color extra 16; y finalmente el 
// 17 es para el color (seleccion,normal)

static wxColour colors_black[18][2] = {
	{wxColour(0  ,0  ,0),	wxColour(127,127,127)},
	{wxColour(127,0  ,0),	wxColour(0,0,0)},
	{wxColour(0  ,127,0),	wxColour(0,0,0)},
	{wxColour(127,127,0),	wxColour(0,0,0)},
	{wxColour(0  ,0  ,255),	wxColour(0,0,0)},
	{wxColour(127,0  ,127),	wxColour(0,0,0)},
	{wxColour(0  ,127,127),	wxColour(0,0,0)},
	{wxColour(127,127,127),	wxColour(0,0,0)},
	{wxColour(0  ,  0,0),	wxColour(0,0,0)},
	{wxColour(255,  0,0),	wxColour(0,0,0)},
	{wxColour(0  ,255,0),	wxColour(0,0,0)},
	{wxColour(255,255,0),	wxColour(0,0,0)},
	{wxColour(127,127,255),	wxColour(0,0,0)},
	{wxColour(255,  0,255),	wxColour(0,0,0)},
	{wxColour(0  ,255,255),	wxColour(0,0,0)},
	{wxColour(255,255,255),	wxColour(0,0,0)},
	{wxColour(20,20,20),	wxColour(75,75,75)},
	{wxColour(50,50,50),	wxColour(0,0,0)}  // seleccion,fondo normal
};

static wxColour colors_white[18][2] = {
	{wxColour(255,255,255),	wxColour(127,127,127)},
	{wxColour(255,0  ,0),	wxColour(0,0,0)},
	{wxColour(0  ,255,0),	wxColour(0,0,0)},
	{wxColour(255,255,0),	wxColour(0,0,0)},
	{wxColour(0  ,0  ,255),	wxColour(0,0,0)},
	{wxColour(255,0  ,255),	wxColour(0,0,0)},
	{wxColour(0  ,255,255),	wxColour(0,0,0)},
	{wxColour(255,255,255),	wxColour(0,0,0)},
	{wxColour(255,255,255),	wxColour(0,0,0)},
	{wxColour(127,  0,0),	wxColour(0,0,0)},
	{wxColour(0  ,100 ,0),	wxColour(0,0,0)},
	{wxColour(100,100,0),	wxColour(0,0,0)},
	{wxColour(0  ,0  ,127),	wxColour(0,0,0)},
	{wxColour(127,  0,127),	wxColour(0,0,0)},
	{wxColour(0  ,127,127),	wxColour(0,0,0)},
	{wxColour(127,127,127),	wxColour(0,0,0)},
	{wxColour(110,110,110),	wxColour(225,225,225)},
	{wxColour(200,200,200),	wxColour(255,255,255)}  // seleccion,null
};

static wxColour (*colors)[2]=colors_white;
	
mxConsole::mxConsole(mxFrame *parent, wxScrollBar *scroll, bool dark_theme, const wxString &font_name, int font_size)
	: wxPanel(parent,wxID_ANY,wxDefaultPosition,wxDefaultSize,0) 
{
	
	SetRandSeed();
	
	wxAcceleratorEntry entries[2];
	entries[0].Set(wxACCEL_CTRL, 'v', CONSOLE_ID_POPUP_PASTE);
	entries[1].Set(wxACCEL_CTRL, 'c', CONSOLE_ID_POPUP_COPY);
	wxAcceleratorTable accel(2, entries);
	SetAcceleratorTable(accel);
	
	if (dark_theme) colors=colors_black;
	
	for(int i=0;i<16;i++) 
		colors[i][1] = wxColour((colors[i][0].Red()+colors[0][0].Red())/2,(colors[i][0].Green()+colors[0][0].Green())/2,(colors[i][0].Blue()+colors[0][0].Blue())/2);
	this->scroll = scroll;
	if (scroll) scroll->SetScrollbar(0,1,1,1,false);
	this->parent = parent;
	timer_size = new wxTimer(this,CONSOLE_ID_TIMER_SIZE);
	timer_caret = new wxTimer(this,CONSOLE_ID_TIMER_CARET);
	timer_process = new wxTimer(this,CONSOLE_ID_TIMER_PROCESS);
	Reset(true);
	m_font_size = font_size;
	m_font_name = font_name;
	SetFont();
	SetBackgroundStyle(wxBG_STYLE_CUSTOM); // para evitar el flickering en windows
}

void mxConsole::Reset (bool hard) {
	dimmed=0;
	events.clear();
	cur_event=-1;
	if (hard) {
		input_history.clear();
		current_input="";
	}
	want_input=wait_one_key=false;
	input_history_position=0;
	history=""; last_clear=0; 
	cur_x=cur_y=0;
	bg=0; cur_fg=15;
	blinking_caret_aux=false;
	ClearBuffer();
}
//
//static string int2str(int x) {
//	int cf=0, x2=x;
//	while(x2) { x2=x2/10; cf++; }
//	string r(cf,'0');
//	while(x) { r[--cf]+=(x%10); x=x/10; }
//	return r;
//}

void mxConsole::OnPaint (wxPaintEvent & event) {
	if (!buffer) CalcResize();
	wxPaintDC dc(this);
	PrepareDC(dc);
	dc.SetBackground(colors[17][1]);
	dc.SetTextBackground(colors[17][1]);
	dc.Clear();
	if (selection_end!=-1) {
		dc.SetPen(wxPen(colors[17][0]));
		dc.SetBrush(wxBrush(colors[17][0]));
		if (selection_start<selection_end)
			for(int i=selection_start;i<=selection_end;i++)
				dc.DrawRectangle(margin+(i%buffer_w)*char_w,margin+(i/buffer_w)*char_h,char_w,char_h);
		else
			for(int i=selection_end;i<=selection_start;i++)
				dc.DrawRectangle(margin+(i%buffer_w)*char_w,margin+(i/buffer_w)*char_h,char_w,char_h);
	}
	dc.SetFont(font);
	for(int i=0;i<buffer_h;i++) { 
		int lj=0; wxString line;
		for(int j=0;j<buffer_w;j++) {
			line<<_buffer(i,j).the_char;
			if (j+1==buffer_w||_buffer(i,j).fg!=_buffer(i,j+1).fg) {
				dc.SetTextForeground(colors[_buffer(i,j).fg][dimmed]);
				dc.DrawText(line,margin+lj*char_w,margin+i*char_h);
				line.Clear();
				lj=j+1;
			}
		}
	}
	if (want_input && !wait_one_key && blinking_caret_aux) {
		dc.SetTextForeground(colors[cur_fg][dimmed]);
		dc.DrawText(wxString()<<"|",margin+cur_x*char_w-char_w/2,margin+cur_y*char_h);
	}
	wxString status[2]; // una variable por linea porque en windows el drawtext no hace el salto de linea
	if (dimmed) { status[0]=_Z("El algoritmo fue modificado."); status[1]=_Z("Click aquí para aplicar los cambios."); }
	else if (selection_is_input) { status[0]=_Z("Utilice doble click para"); status[1]=_Z("modificar solo esa lectura."); }
	else if ( (want_input || wait_one_key) && cur_loc.IsValid()) status[1]<<_Z("línea ")<<cur_loc.line<<_Z(" instrucción ")<<cur_loc.inst;
	if (status[1].Len()) {
		wxColour &ct=colors[16][0];
		wxColour &cb=colors[16][1];
		int w=dc.GetSize().GetWidth();
		int h=dc.GetSize().GetHeight();
		for(int i=1;i>=0;i--) { 
			int tw,th,margin=3;
			dc.GetTextExtent(status[i],&tw,&th);
			dc.SetPen(wxPen(cb));
			dc.SetBrush(wxBrush(cb));
			dc.SetTextForeground(ct);
			int x0 = w-tw-2*margin, y0 = cur_y<=2*buffer_h/3?h-th-2*margin:i*th;
			dc.DrawRectangle(x0,y0,tw+2*margin,th+2*margin);
			dc.SetTextForeground(ct);
			dc.DrawText(status[i],x0+margin,y0+margin);
			h-=th;
		}
	}
}

void mxConsole::OnSize (wxSizeEvent & event) {
	timer_size->Start(_SIZE_TIME,true);
}

void mxConsole::OnChar (wxKeyEvent & event) {
//	if (event.GetKeyCode()==3) { parent->ShouldClose(); return; } // esto debe haber sido el ctrl+C, en la version unicode no funciona
	if (the_process) {
		if (!want_input || cur_event!=-1) return;
		wxOutputStream *output=the_process->GetOutputStream();
		auto uni_key = event.GetUnicodeKey();
		auto key_code = event.GetKeyCode();
		if (wait_one_key) {
			if (uni_key=='\r') uni_key='\n';
		 	wait_one_key=false;
			RecordInput(wxString()<<uni_key);
			output->Write(&uni_key,1);
		} else {
			if (key_code==WXK_ESCAPE) { 
				for(int i=current_input.Len();i>0;i--) 
					Process("\b"); 
				current_input="";
				Refresh();
				return;
			} else if (uni_key=='\r'||uni_key=='\n') {
				current_input<<"\n";
				string val((const char*)(current_input.mb_str(wxCSConv("ISO-8859"))));
				output->Write(val.c_str(),val.size());
				RecordInput(current_input);
				uni_key='\n';
			} else if (key_code==WXK_DELETE or key_code==WXK_BACK) {
				if (!current_input.Len()) return;
				current_input.RemoveLast();
				uni_key='\b';
			} else if (uni_key!=WXK_NONE) {
				current_input<<uni_key;
			} else // tecla no soportada (por ej, flechas)
				return;
			Print(wxString()<<uni_key,true/*,true*/);
			Refresh();
		}
	} else parent->ShouldClose();
}

void mxConsole::SetFont() {
	font = wxFont(wxFontInfo(m_font_size).Family(wxFONTFAMILY_MODERN).FaceName(m_font_name));
	wxClientDC dc(this); dc.SetFont(font);
	// calcular el ancho promedio de esta forma, porque si le pregunto el ancho generico de 
	// un caracter me lo da como int, pero en mac parece ser un valor no entero (si calculo
	// mal el cursor se ubica mal y la seleccion se marca mal)
	static wxString sample_text = "ASDFGHJKLqwertyuio12345678 .,-";
	auto tex_ext = dc.GetTextExtent(sample_text);
	char_w = float(tex_ext.GetWidth())/float(sample_text.Len());
	char_h = tex_ext.GetHeight();
}

void mxConsole::Print (wxString text, bool record/*, bool do_print*/) {
	if (record) history<<text;
	if (!buffer) {
		if (record) {
			int l=text.Len();
			for(int i=0;i<l;i++) 
				if (text[i]=='\n') 
					MarkEvent();
		}
		return;
	}
	int l=text.Len();
	for(int i=0;i<l;i++) {
		if (text[i]=='\n') {
			if (record) MarkEvent();
			cur_x=buffer_w;
		} else if (text[i]=='\r') {
			cur_x=0;
		} else if (text[i]=='\b') {
			if (--cur_x<0) {
				cur_x=buffer_w-1;
				if (cur_y>0) cur_y--;
			}
			_buffer(cur_y,cur_x).the_char=' ';
		} else {
			_buffer(cur_y,cur_x).loc=cur_loc;
			_buffer(cur_y,cur_x).the_char=text[i];
			_buffer(cur_y,cur_x).fg=cur_fg;
			cur_x++;
		}
		if (cur_x>=buffer_w) {
			cur_x=0; cur_y++;
			if (cur_y>=buffer_h) { // si hay que hacer scroll
				if (selection_start!=-1) if ( (selection_start-=buffer_w)<0 ) selection_start=0;
				if (selection_end!=-1) if ( (selection_end-=buffer_w)<0 ) selection_end=0;
				cur_y=buffer_h-1;
				for(int i=1;i<buffer_h;i++) { 
					for(int j=0;j<buffer_w;j++) { 
						_buffer(i-1,j)=_buffer(i,j);
					}
				}
				for(int j=0;j<buffer_w;j++) { 
					_buffer(buffer_h-1,j).the_char=' ';
					_buffer(buffer_h-1,j).fg=cur_fg;
				}
			}
		}
	}
}

void mxConsole::ClearBuffer() {
	for(int i=0;i<buffer_h;i++) { 
		for(int j=0;j<buffer_w;j++) { 
			_buffer(i,j).the_char=' ';
			_buffer(i,j).loc.Clear();
		}
	}
}

void mxConsole::Process (wxString input, bool record/*, bool do_print*/) {
	int i,l;
	if (record) { // normal, viene un comando nuevo, se procesa todo
		i=0; l=input.Len();
	} else if (cur_event==-1) { // tiempo actual, se reprocesa desde el ultimo last_clear hasta el final
		i=last_clear; l=input.Len();
	} else { // viaje en el tiempo, se reprocesa hasta cierto evento
		i=events[cur_event].last_clear; l=events[cur_event].pos;
	}
	int i0=i;
	while (i<l) {
		if (input[i]=='\033' && input[i+1]=='[') {
			if (i>i0) Print(input.Mid(i0,i-i0),record/*,do_print*/);	
			if (input[i+2]=='z' && (input[i+3]=='p'||input[i+3]=='e')) { // receives code location
				cur_loc.is_error = input[i+3]=='e';
				int j=i+4, i0=i;
				cur_loc.line=0, cur_loc.inst=0;
				while (input[j]>='0'&&input[j]<='9') {
					cur_loc.line=cur_loc.line*10+(input[j]-'0');
					j++;
				}
				i=j+1;
				while (input[i]>='0'&&input[i]<='9') {
					cur_loc.inst=cur_loc.inst*10+(input[i]-'0');
					i++;
				}
				if (record) history<<input.Mid(i0,i-i0+1);
			} else if (input[i+2]=='z' && input[i+3]=='r') { // raise window
				GetParent()->Raise(); i+=3;
			} else if (input[i+2]=='z' && input[i+3]=='t') { // change window title
				int j=i+4; while (j<int(input.Len()) && input[j]!='\n') j++;
				wxString title=input.SubString(i+4,j-1); if (title.Last()=='\r') title.RemoveLast();
				parent->SetTitle(_ZZ("PSeInt - Ejecutando proceso ")+title);
				i=j;
			} else if (input[i+2]=='z' && input[i+3]=='k') { // getKey
				if (input_history_position>=int(input_history.size())) { 
					want_input=true; wait_one_key=true;
				} else {
					input_history[input_history_position].loc = cur_loc; // cambiar una entrada puede hacer que este input ahora sea para otro leer
					wxString aux=input_history[input_history_position++].text;
					wxOutputStream *output=the_process->GetOutputStream();
					output->Write(aux.c_str(),aux.Len());
				}
				i+=3;
			} else if (input[i+2]=='z' && input[i+3]=='l') { // getLine
				if (input_history_position>=int(input_history.size()) || input_history[input_history_position].force_user_input) { 
					want_input=true; wait_one_key=false;
//					wxOutputStream *output=the_process->GetOutputStream();
//					output->Write(current_input.c_str(),current_input.Len());
					Print(current_input,true/*,true*/); // true,true, porque estas cosas solo llegan en vivo, no se guardan en el historial
				} else {
					input_history[input_history_position].loc = cur_loc; // cambiar una entrada puede hacer que este input ahora sea para otro leer
					wxString aux=input_history[input_history_position++].text;
					wxOutputStream *output=the_process->GetOutputStream();
					output->Write(aux.c_str(),aux.Len());
					Print(aux,true/*,true*/);  // true,true, porque estas cosas solo llegan en vivo, no se guardan en el historial
				}
				i+=3;
			} else if (input[i+2]=='2' && input[i+3]=='J') {
				ClearBuffer(); i+=3;
				if (record) { history<<"\033[2J"; last_clear=history.Len(); }
			} else if (input[i+2]=='?' && input[i+3]=='2' && input[i+4]=='5' && (input[i+5]=='l'||input[i+5]=='h')) {
				// hide/show caret, ignored
				i+=5;
			} else if (input[i+2]=='4' && input[i+4]=='m') {
				// background color, ignored
				i+=4; 
			} else if (input[i+2]=='0' && input[i+3]=='m') {
				if (cur_fg>7) cur_fg-=8; 
				if (record) history<<"\033[0m";
				i+=3;
			} else if (input[i+2]=='1' && input[i+3]=='m') {
				if (cur_fg<8) cur_fg+=8; 
				if (record) history<<"\033[1m";
				i+=3;
			} else if (input[i+2]=='3' && (input[i+3]>='0'&&input[i+3]<='7') && input[i+4]=='m') {
				if (record) history<<"\033[3"<<input[i+3]<<"m";
				cur_fg=(cur_fg/8)*8+(input[i+3]-'0');
				i+=4;
			} else {
				int x0=i+2; int x1=x0;
				while (input[x1]>='0'&&input[x1]<='9') x1++;
				char c=input[x1];
				int y0=x1+1; int y1=y0;
				while (input[y1]>='0'&&input[y1]<='9') y1++;
				if (x0!=x1 && y0!=y1 && c==';' && input[y1]=='H') {
					long x; input.Mid(x0,x1-x0).ToLong(&x);
					long y; input.Mid(y0,y1-y0).ToLong(&y);
					GotoXY(x,y,record);
				}
				i=y1;
			}
			i0=i+1;
		}
		i++;
	}
	if (i>i0) Print(input.Mid(i0,i-i0),record/*,do_print*/);	
}

void mxConsole::OnTimerCaret (wxTimerEvent & event) {
	if (!want_input || wait_one_key) return; // no redibujar si no cambia nada
	blinking_caret_aux=!blinking_caret_aux;
	Refresh();
}

void mxConsole::OnTimerSize (wxTimerEvent & event) {
	CalcResize();
}

void mxConsole::CalcResize() {
	wxSize size = GetClientSize();
	buffer_w = (size.x - 2*margin) / char_w;
	buffer_h = (size.y - 2*margin) / char_h;
	if (buffer_w<1) buffer_w=1;
	if (buffer_h<1) buffer_h=1;
	delete [] buffer;
	buffer=new console_char[buffer_w*buffer_h];
	RebuildBuffer();
}

void mxConsole::Run (wxString command) {
	this->command = command;
	command << " --seed=" << fixed_rand_seed;
	KillProcess();
	the_process=new wxProcess(this->GetEventHandler(),wxID_ANY);
	the_process->Redirect();
	the_process_pid=wxExecute(command,wxEXEC_ASYNC,the_process);
	if (the_process_pid>0) {
		timer_process->Start(_PROCESS_TIME,false);
		timer_caret->Start(_CARET_TIME,false);
	} else {
		delete the_process;
		the_process=NULL;
	}
}

void mxConsole::OnTimerProcess (wxTimerEvent & event) {
	GetProcessOutput();
}

void mxConsole::GetProcessOutput (bool refresh) {
	if (!the_process) return;
	wxTextInputStream input(*(the_process->GetInputStream()));
	wxString line;
	int watchdog=0;
	while (the_process->IsInputAvailable() && ++watchdog<100000) {
		line<<input.GetChar();
	}
	if (line.Len()) { 
		if (cur_event!=-1) SetTime(int(events.size()));
		Process(line,true/*,cur_event==-1*/); 
		if (refresh) { Refresh(); Yield(); }
		// el wxYield es neceasario para procesos tipo "mientras verdero hacer escribir "Hola"; finmientras" porque sino no tiene oportunidad de redibujar por el intenso procesamiento
	}
}

void mxConsole::OnProcessTerminate( wxProcessEvent &event ) {
	if (event.GetPid()==the_process_pid) {
		GetProcessOutput(false);
		want_input=false;
		Refresh();
		the_process->Detach();
		the_process=NULL;
		timer_process->Stop();
		parent->OnProcessTerminated();
	}	
}

void mxConsole::GotoXY (int x, int y, bool record) {
	if (record) history<<"\033["<<y<<';'<<x<<'H';
	cur_x=x-1; if (cur_x>=buffer_w) cur_x=buffer_w;
	cur_y=y-1; if (cur_y>=buffer_h) cur_y=buffer_h;
}

void mxConsole::Reload () {
	KillProcess();
	Reset(false);
	Run(command);
	parent->SetIsPresent(true);
}

void mxConsole::KillProcess ( ) {
	if (the_process) { 
		the_process->Kill(the_process_pid,wxSIGKILL);
		the_process->Detach();
	}
	the_process=NULL;
}

void mxConsole::RecordInput (wxString input) {
	if (input_history_position==int(input_history.size())) {
		input_history.push_back(input_history_entry(input,cur_loc));
	} else {
		input_history[input_history_position]=input_history_entry(input,cur_loc);
	}
	input_history_position++;
	current_input.Clear();
	want_input=false;
}


void mxConsole::OnMouseWheel (wxMouseEvent & evt) {
	if (evt.ControlDown()) {
		if (evt.m_wheelRotation>0) {
			m_font_size++;
		} else if (m_font_size>4) {
			m_font_size--;
		}
		SetFont();
		timer_size->Start(_SIZE_TIME,true);
		Refresh();
	} else {
		int i=scroll->GetThumbPosition();
		if (evt.m_wheelRotation<0) {
			if (i<int(events.size())) {
				scroll->SetThumbPosition(++i);
				SetTime(scroll->GetThumbPosition());
				Refresh();
			}
		} else {
			if (i>0) {
				scroll->SetThumbPosition(--i);
				SetTime(scroll->GetThumbPosition());
				Refresh();
			}
		}
	}
	
}

void mxConsole::MarkEvent ( ) {
	if (!scroll) return;
	history_event h;
	h.pos=history.Len();
	h.last_clear=last_clear;
	h.input_count=input_history.size();
	if (input_history_position<h.input_count) 
		h.input_count=input_history_position;
	events.push_back(h);
//	int pos=cur_event==-1?h.pos:cur_event;
	scroll->SetScrollbar(h.pos,1,events.size()+1,events.size()/10,true);
}

void mxConsole::SetTime (int t) {
	if (t>=int(events.size())) {
		t=-1; parent->SetIsPresent(true);
	} else {
		parent->SetIsPresent(false);
	}
	cur_event=t;
	RebuildBuffer();
}

void mxConsole::RebuildBuffer ( ) {
	selection_end=-1;
	ClearBuffer(); cur_x=cur_y=0; cur_loc.line=cur_loc.inst=-1;
	Process(history, false);
	Refresh();
}

void mxConsole::PlayFromCurrentEvent ( ) {
	if (!events.size()) return;
	if (cur_event!=-1) {
		input_history.erase(input_history.begin()+events[cur_event].input_count,input_history.end());
	}
	Reload();
}

void mxConsole::Dimm ( ) {
	dimmed=1;
	Refresh();
}

inline int auxGetPosition(const wxMouseEvent &evt, int margin, int char_w, int char_h, int buffer_w, int buffer_h) {
	int x=(evt.GetX()-margin)/char_w, y=(evt.GetY()-margin)/char_h;
	int r=x+buffer_w*y;	
	if (r<0) r=0; else if (r>=buffer_w*buffer_h) r=buffer_w*buffer_h-1;
	return r;
}

void mxConsole::OnMouseLeftDown (wxMouseEvent & evt) {
	selecting=true; 
	selection_start=auxGetPosition(evt,margin,char_w,char_h,buffer_w,buffer_h);
	selection_end=-1;
	Refresh();
	evt.Skip();
}

void mxConsole::OnMouseLeftUp (wxMouseEvent & evt) {
	if (selecting) { // si por ej maximizamos con doble-click, los 2 click bajan el boton fuera de la consola, pero el 2do click suelta dentro
		selecting=false;
		if (selection_end==-1) 
			GetSourceLocationFromOutput(selection_start);
	}
	evt.Skip();
}

void mxConsole::OnMouseMotion (wxMouseEvent & evt) {
	if (selecting) {
		selection_end=auxGetPosition(evt,margin,char_w,char_h,buffer_w,buffer_h);
		Refresh();
	}
	evt.Skip();
}

inline wxString GetClipboardText() {
	if (!wxTheClipboard->Open()) return "";
	wxTextDataObject data;
	if (!wxTheClipboard->GetData(data)) {
		wxTheClipboard->Close();
		return "";
	}
	wxTheClipboard->Close();
	return data.GetText();
}

static int popup_src_pos=-1;

void mxConsole::OnMouseRightDown (wxMouseEvent & evt) {
	popup_src_pos = auxGetPosition(evt,margin,char_w,char_h,buffer_w,buffer_h);
	wxMenu menu;
	wxMenuItem *mcopy=menu.Append(CONSOLE_ID_POPUP_COPY,_Z("&Copiar"));
	if (selection_end==-1) mcopy->Enable(false);
	wxMenuItem *mpaste=menu.Append(CONSOLE_ID_POPUP_PASTE,_Z("&Pegar"));
	if (!GetClipboardText().Len()) mpaste->Enable(false);
	menu.AppendSeparator();
//	menu.Append(CONSOLE_ID_POPUP_RESET,"Reiniciar ejecución");
//	menu.Append(CONSOLE_ID_POPUP_FROM_HERE,"Continuar desde aquí");
	if (popup_src_pos!=-1 && GetInputPositionFromBufferPosition(popup_src_pos)!=-1) {
		menu.Append(CONSOLE_ID_POPUP_CHANGE_INPUT,_Z("Cambiar valor ingresado"));
		menu.AppendSeparator();
	}
	menu.AppendCheckItem(CONSOLE_ID_POPUP_STAY_ON_TOP,_Z("Ventana siempre visible"))->Check(parent->GetStayOnTop());
	menu.AppendCheckItem(CONSOLE_ID_POPUP_CLOSE_AFTER_RUN,_Z("Cerrar al finalizar la ejecución"))->Check(!parent->GetDoNotClose());
	PopupMenu(&menu);
}


void mxConsole::OnPopupPaste(wxCommandEvent &evt) {
	wxString str=GetClipboardText();
	str.Replace("\r","");
	wxKeyEvent k(wxEVT_CHAR); 
	if (want_input) {
		for(unsigned int i=0;i<str.Len();i++) { 
			k.m_keyCode=str[i];
			OnChar(k);
			if (str[i]=='\n') {
				str=str.AfterFirst('\n');
				break;
			}
		}
	}
	str<<"\n";
	while (str.Len()) {
		input_history.push_back(input_history_entry(str.BeforeFirst('\n')+"\n",cur_loc));
		str=str.AfterFirst('\n');
	}
}

void mxConsole::OnPopupCopy (wxCommandEvent & evt) {
	if (selection_end<selection_start) { int aux=selection_end; selection_end=selection_start; selection_start=aux; }
	int i0=selection_start%buffer_w, i1=selection_end%buffer_w;
	int j0=selection_start/buffer_w, j1=selection_end/buffer_w;
	wxString res;
	for(int j=j0;j<=j1;j++) {
		wxString aux;
		int k0=j==j0?i0:0, k1=j==j1?i1:buffer_w;
		for (int k=k0;k<k1;k++) {
			aux<<_buffer(j,k).the_char;
		}
		while (aux.Len() && aux.Last()==' ') aux.RemoveLast();
		{ if (j!=j1) aux<<"\n"; } res<<aux;
	}
	wxTheClipboard->Open();
	wxTheClipboard->SetData(new wxTextDataObject(res));
	wxTheClipboard->Close();
}

void mxConsole::GetSourceLocationFromOutput (int pos) {
	if (!buffer[pos].loc.IsValid()) return;
	selection_start=pos; selection_end=pos;
	while (selection_start>0 && buffer[selection_start-1].loc==buffer[pos].loc) selection_start--;
	while (selection_end+1<buffer_w*buffer_h && buffer[selection_end+1].loc==buffer[pos].loc) selection_end++;
	// buscar en los eventos de entrada, si justo seleccionamos uno para ofrecer modificarla
	selection_is_input = (buffer[pos].loc!=cur_loc||cur_loc.is_error) && GetInputPositionFromBufferPosition(pos)!=-1;
	// mostrar seleccion y marcar en el pseudocódigo la instrucción correspondiente
	Refresh();
	parent->SendLocation(buffer[pos].loc.line,buffer[pos].loc.inst);
}

void mxConsole::OnMouseDClick (wxMouseEvent & evt) {
	if (selection_start==-1) { evt.Skip(); return; }
	int input_pos = GetInputPositionFromBufferPosition(selection_start);
	if (input_pos==-1) { evt.Skip(); return; }
	input_history[input_pos].force_user_input = true;
	selection_start=selection_end=-1; selecting=false; selection_is_input=false;
	Reload();
}

int mxConsole::GetInputPositionFromBufferPosition (int pos) {
	code_location &loc = buffer[pos].loc; // linea de codigo que genero el caracter de esa pos, para ver si es una entrada
	// puede haber varias entradas generadas por la misma linea de codigo, ver cuantas saltear (buscando de atras para adelante)
	// entre lectura y lectura va a haber en el buffer al menos un lugar con loc(-1,-1), cuento esos cambios para distinguir
	// en cual de las varias para un mismo loc estoy... el problema es que puede haber un cambio adicional debido a un mensaje 
	// de error debido a la lectura (tendra su mismo loc)
	int cant_skip = 0; int cpos = cur_y*buffer_w+cur_x-1;
	code_location cloc = loc;
	while(cpos>=pos) {
		if (cloc!=buffer[cpos].loc) {
			if (cloc==loc && !cloc.is_error) cant_skip++;
			cloc=buffer[cpos].loc;
		}
		cpos--;
	}
	// buscar la entrada que corresponde
	for(int i=input_history_position-1;i>=0;i--)
		if (input_history[i].loc==loc)
			if (--cant_skip==0) return i;
	return -1;
}

void mxConsole::Yield ( ) {
	static bool yielding = false;
	if (yielding) return;
	yielding=true; wxYield(); yielding=false;
}

void mxConsole::SetRandSeed ( ) {
	fixed_rand_seed = rand();
}

void mxConsole::OnPopupCloseAfterRun (wxCommandEvent & evt) {
	parent->ToggleDoNotClose();
}

void mxConsole::OnPopupStayOnTop (wxCommandEvent & evt) {
	parent->ToggleStayOnTop();
}

void mxConsole::OnPopupReset (wxCommandEvent & evt) {
	parent->OnButtonRunAgain(evt);
}

void mxConsole::OnPopupChangeInput (wxCommandEvent & evt) {
	selection_start=selection_end=popup_src_pos;
	wxMouseEvent mevt; OnMouseDClick(mevt);
}

void mxConsole::OnPopupFromHere (wxCommandEvent & evt) {
	parent->OnButtonPlay(evt);
}

