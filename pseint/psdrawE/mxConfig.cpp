#include <wx/sizer.h>
#include <wx/stattext.h>
#include <wx/textctrl.h>
#include <wx/checkbox.h>
#include <wx/combobox.h>
#include <wx/button.h>
#include <wx/bitmap.h>
#include <wx/icon.h>
#include "mxConfig.h"
#include "../psdraw3/Global.h"
#include "../psdraw3/Load.h"
#include "../wxPSeInt/string_conversions.h"

enum { MID_NULL = wxID_HIGHEST, MID_ANCHO, MID_ALTO, MID_ZOOM, MID_CROP, MID_COMPACT, MID_COMMENTS, MID_COLORS, MID_STYLE, MID_PROC };

BEGIN_EVENT_TABLE(mxConfig,wxDialog)
	EVT_TEXT(MID_ANCHO,mxConfig::OnAncho)
	EVT_TEXT(MID_ALTO,mxConfig::OnAlto)
	EVT_TEXT(MID_ZOOM,mxConfig::OnZoom)
	EVT_COMBOBOX(MID_COLORS,  mxConfig::OnColors)
	EVT_CHECKBOX(MID_COMMENTS,mxConfig::OnComments)
	EVT_CHECKBOX(MID_CROP,    mxConfig::OnCrop)
	EVT_CHECKBOX(MID_COMPACT, mxConfig::OnCompact)
	EVT_COMBOBOX(MID_PROC,    mxConfig::OnProc)
	EVT_COMBOBOX(MID_STYLE,   mxConfig::OnStyle)
END_EVENT_TABLE()

static wxSizerFlags szflag;

static void AddWithLabel(wxWindow *parent, wxSizer *sizer, wxString text, wxWindow *control) {
	wxSizer *aux_sizer = new wxBoxSizer(wxHORIZONTAL);
	aux_sizer->Add( new wxStaticText(parent,wxID_ANY,text),szflag );
	aux_sizer->Add( control,szflag );
	sizer->Add (aux_sizer, szflag);
}

// enums para ordenar los combos
enum { CLR_BW, CLR_SOBER, CLR_DEFAULT, CLR_DARK, CLR_COUNT }; // estilos de colores
enum { ST_DEFAULT, ST_AL_IO, ST_NS, ST_COUNT }; // tipos de diagrama

mxConfig::mxConfig():wxDialog(NULL,wxID_ANY,_Z("Guardar diagrama de flujo"),wxDefaultPosition,wxDefaultSize) {
	
	wxIconBundle bundle;
	wxIcon icon24; icon24.CopyFromBitmap(wxBitmap("imgs/tools/24/guardar.png",wxBITMAP_TYPE_PNG)); bundle.AddIcon(icon24);
	wxIcon icon32; icon32.CopyFromBitmap(wxBitmap("imgs/tools/32/guardar.png",wxBITMAP_TYPE_PNG)); bundle.AddIcon(icon32);
	SetIcons(bundle);
	
	ignore_events=true;
	
	wxSizer *sizer = new wxBoxSizer(wxVERTICAL);
	// el FixedMinSize no parece necesario, pero sin eso en windows los combos dejan un espacio inexplicable
	szflag = wxSizerFlags().Border(wxALL,4).Center().FixedMinSize();
	
	wxArrayString procs;
	for(unsigned int i=0;i<g_code.procesos.size();i++) 
		procs.Add((g_code.procesos[i]->lpre+g_code.procesos[i]->label).c_str());
	cm_proc = new wxComboBox(this,MID_PROC,_Z(""),wxDefaultPosition,wxDefaultSize,procs,wxCB_READONLY|wxCB_SIMPLE);
	AddWithLabel(this, sizer, _Z("Algoritmo:"),cm_proc);

	tx_zoom = new wxTextCtrl(this,MID_ZOOM,_Z("100"));
	AddWithLabel(this, sizer, _Z("Zoom(%):"),tx_zoom);
	tx_ancho = new wxTextCtrl(this,MID_ANCHO,_Z(""));
	AddWithLabel(this, sizer, _Z("Ancho(px):"),tx_ancho);
	tx_alto = new wxTextCtrl(this,MID_ALTO,_Z(""));
	AddWithLabel(this, sizer, _Z("Alto(px):"),tx_alto);
	
	ch_comments = new wxCheckBox(this,MID_COMMENTS,_Z("Incluir comentarios"));
	ch_comments->SetValue(g_config.show_comments);
	sizer->Add (ch_comments,szflag);
	ch_crop = new wxCheckBox(this,MID_CROP,_Z("Cortar textos largos"));
	ch_crop->SetValue(false);
	sizer->Add (ch_crop,szflag);
	ch_compact = new wxCheckBox(this,MID_COMPACT,_Z("Reducir alturas"));
	ch_compact->SetValue(false);
	sizer->Add (ch_compact,szflag);
	
	wxArrayString acolors; acolors.Add("",CLR_COUNT);
	acolors[CLR_BW]      = _Z("Blanco y Negro");
	acolors[CLR_SOBER]   = _Z("Sobrio");
	acolors[CLR_DEFAULT] = _Z("Normal");
	acolors[CLR_DARK]    = _Z("Invertido");
	cm_colors = new wxComboBox(this,MID_COLORS,_Z(""),wxDefaultPosition,wxDefaultSize,acolors,wxCB_READONLY|wxCB_SIMPLE);
	cm_colors->SetSelection( g_config.shape_colors ? (g_config.dark_theme ? CLR_DARK : CLR_DEFAULT) : CLR_SOBER );
	AddWithLabel(this, sizer, _Z("Colores:"), cm_colors);
	
	wxArrayString astyles; astyles.Add("",ST_COUNT);
	astyles[ST_DEFAULT] = _Z("Clásico");
	astyles[ST_AL_IO]   = _Z("Clásico (i/o alternativa)");
	astyles[ST_NS]      = _Z("Nassi-Shneiderman");
	cm_style = new wxComboBox(this,MID_STYLE,_Z(""),wxDefaultPosition,wxDefaultSize,astyles,wxCB_READONLY|wxCB_SIMPLE);
	cm_style->SetSelection( g_config.nassi_shneiderman ? ST_NS : (g_config.alternative_io ? ST_AL_IO : ST_DEFAULT) );
	AddWithLabel(this, sizer, _Z("Tipo:"), cm_style);
	
	wxButton *ok = new wxButton(this,wxID_OK,_Z("Guardar"));
	wxButton *cancel = new wxButton(this,wxID_CANCEL,_Z("Cancelar"));
	wxSizer *aux_sizer = new wxBoxSizer(wxHORIZONTAL);
	aux_sizer->Add(cancel,szflag);
	aux_sizer->Add(ok,szflag);
	sizer->Add(aux_sizer,szflag);
	
	ok->SetDefault();
	
	tx_zoom->SetFocus();
	cm_proc->SetSelection(0);
	SetProceso(0);
	
	SetSizerAndFit(sizer);
	ignore_events=false;
}

static void get_wh(float z, int &rw, int &rh) {
	g_view.zoom = g_view.d_zoom = z;
	int h=0,wl=0,wr=0, margin=10;
	Entity::CalculateAll(true);
	Entity *real_start = g_code.start->GetTopEntity();
	real_start->Calculate(wl,wr,h); 
	int x0=real_start->x-wl,y0=real_start->y,x1=real_start->x+wr,y1=real_start->y-h;
	rw=( (x1-x0)+2*margin )*z;
	rh=( (y0-y1)+2*margin )*z;
}

void mxConfig::SetZoom (float f, int noup) {
	float prev_f=100;
	if (f==0) f=prev_f;
	int rw,rh; get_wh(f/100.f,rw,rh);
	wxString sz; sz.Printf("%.2f",f);
	if (noup!=0) tx_zoom->SetValue(sz);
	if (noup!=1) tx_ancho->SetValue(wxString()<<rw);
	if (noup!=2) tx_alto->SetValue(wxString()<<rh);
	prev_f=f;
}

void mxConfig::OnAncho (wxCommandEvent & evt) {
	if (ignore_events) return;
	ignore_events=true;
	evt.Skip();
	long w;
	if (tx_ancho->GetValue().ToLong(&w)) {
		int w1,h1; get_wh(1,w1,h1);
		float z=100.f*(float(w)/float(w1));
		SetZoom(z,1);
	}
	ignore_events=false;
}

void mxConfig::OnAlto (wxCommandEvent & evt) {
	if (ignore_events) return;
	ignore_events=true;
	evt.Skip();
	long h;
	if (tx_alto->GetValue().ToLong(&h)) {
		int w1,h1; get_wh(1,w1,h1);
		float z=100.f*(float(h)/float(h1));
		SetZoom(z,2);
	}
	ignore_events=false;
}

void mxConfig::OnZoom (wxCommandEvent & evt) {
	evt.Skip();
	if (ignore_events) return;
	ignore_events=true;
	double f=100;
	if (tx_zoom->GetValue().ToDouble(&f)) {
		SetZoom(f,0);
	}
	ignore_events=false;
}

void mxConfig::OnCompact (wxCommandEvent & evt) {
	evt.Skip();
	if (ch_compact->GetValue()) {
		flecha_h = 15;
		margin = 4;
	} else {
		// debe coincidir con Entity.cpp
		flecha_h = 25;
		margin = 6;
	}
	Entity::CalculateAll(true);
	SetZoom();
}

void mxConfig::OnComments (wxCommandEvent &evt) {
	evt.Skip();
	if (ignore_events) return;
	g_config.show_comments = ch_comments->GetValue();
	Entity::CalculateAll();
	SetZoom();
}

void mxConfig::OnCrop (wxCommandEvent &evt) {
	evt.Skip();
	if (ignore_events) return;
	g_config.enable_partial_text = ch_crop->GetValue();
	Entity::CalculateAll(true);
	SetZoom();
}

void mxConfig::SetProceso (int i) {
	SetProc(g_code.procesos[i]);
	SetZoom();
}

void mxConfig::OnProc (wxCommandEvent & evt) {
	evt.Skip();
	if (ignore_events) return;
	SetProceso(cm_proc->GetSelection());
}

void mxConfig::OnColors (wxCommandEvent & evt) {
	evt.Skip();
	int icolor = cm_colors->GetSelection();
	switch (icolor) {
	case CLR_BW:
		g_config.syntax_highlight = false;
		g_config.dark_theme = false;
		g_config.shape_colors = false;
		break;
	case CLR_SOBER:
		g_config.syntax_highlight = true;
		g_config.dark_theme = false;
		g_config.shape_colors = false;
		break;
	case CLR_DEFAULT:
		g_config.syntax_highlight = true;
		g_config.dark_theme = false;
		g_config.shape_colors = true;
		break;
	case CLR_DARK:
		g_config.syntax_highlight = true;
		g_config.dark_theme = true;
		g_config.shape_colors = true;
		break;
	}
	SetColors();
}

void mxConfig::OnStyle (wxCommandEvent & evt) {
	evt.Skip();
	int istyle = cm_style->GetSelection();
	g_config.nassi_shneiderman = istyle==ST_NS;
	g_config.alternative_io    = istyle==ST_AL_IO;
	Entity::CalculateAll(true);
	SetZoom();	
}

