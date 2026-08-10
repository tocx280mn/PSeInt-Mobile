#include <iostream>
#include <wx/sizer.h>
#include <wx/toolbar.h>
#include <wx/msgdlg.h>
#include <wx/settings.h>
#include "../wxPSeInt/string_conversions.h"
#include "MainWindow.h"
#include "ToolBar.h"
#include "Canvas.h"
#include "Global.h"
#include "Load.h"
#include "Events.h"
#include "Comm.h"
#include <wx/icon.h>

MainWindow *g_main_window = nullptr;

enum { MID_MAIN = wxID_HIGHEST, MID_ZOOM, MID_FULLSCREEN, MID_STYLE, MID_COMMENTS, MID_CROPLABEL, MID_COLORS, MID_SUB, MID_RUN, MID_DEBUG, MID_EXPORT, MID_CLOSE, MID_HELP };

BEGIN_EVENT_TABLE(MainWindow, wxFrame)
//	EVT_KEY_DOWN(MainWindow::OnChar) // no hace falta, los agarra directamente el canvas
#if defined(WIN32) || defined(WIN64)
	EVT_MOUSEWHEEL(MainWindow::OnMouseWheel)
#endif
	EVT_TOOL_ENTER(wxID_ANY,MainWindow::OnToolOver)
	EVT_TOOL(wxID_ANY,MainWindow::OnTool)
	EVT_CLOSE(MainWindow::OnClose)
	EVT_ACTIVATE(MainWindow::OnActivated)
END_EVENT_TABLE()

MainWindow::MainWindow(wxString title):wxFrame(nullptr,wxID_ANY,title,wxDefaultPosition,wxSize(g_view.win_w,g_view.win_h),wxDEFAULT_FRAME_STYLE) {
	
	wxIconBundle bundle;
	wxIcon icon24; icon24.CopyFromBitmap(wxBitmap("imgs/tools/24/flujo.png",wxBITMAP_TYPE_PNG)); bundle.AddIcon(icon24);
	wxIcon icon32; icon32.CopyFromBitmap(wxBitmap("imgs/tools/32/flujo.png",wxBITMAP_TYPE_PNG)); bundle.AddIcon(icon32);
	SetIcons(bundle);
	
	g_main_window=this;
	wxSizer *sizer=new wxBoxSizer(wxVERTICAL);
	
	wxToolBar *toolbar = CreateToolBar(wxTB_HORIZONTAL|wxNO_BORDER|wxTB_FLAT,wxID_ANY);
	SetColors(toolbar->GetBackgroundColour(),
			  wxSystemSettings::GetColour(wxSYS_COLOUR_HIGHLIGHT));
	wxBitmap abmp(g_constants.imgs_path+"tb_sub.png",wxBITMAP_TYPE_PNG);
	toolbar->SetToolBitmapSize(wxSize(abmp.GetWidth(),abmp.GetHeight()));
//	toolbar->AddTool(MID_SETTINGS,"",g_constants.imgs_path+"tb_settings.png");
	if (g_lang[LS_ENABLE_USER_FUNCTIONS]) {
		toolbar->AddTool(MID_SUB  ,"",wxBitmap(g_constants.imgs_path+"tb_sub.png",wxBITMAP_TYPE_PNG));
		toolbar->AddSeparator();
	}
	toolbar->AddTool(MID_EXPORT ,"",wxBitmap(g_constants.imgs_path+"tb_save.png",wxBITMAP_TYPE_PNG));
	toolbar->AddSeparator();
	toolbar->AddTool(MID_ZOOM ,"",wxBitmap(g_constants.imgs_path+"tb_zoom.png",wxBITMAP_TYPE_PNG));
	toolbar->AddTool(MID_FULLSCREEN ,"",wxBitmap(g_constants.imgs_path+"tb_fullscreen.png",wxBITMAP_TYPE_PNG));
	toolbar->AddSeparator();
	toolbar->AddTool(MID_STYLE ,"",wxBitmap(g_constants.imgs_path+"tb_style.png",wxBITMAP_TYPE_PNG));
	toolbar->AddTool(MID_COMMENTS ,"",wxBitmap(g_constants.imgs_path+"tb_comments.png",wxBITMAP_TYPE_PNG));
	toolbar->AddTool(MID_CROPLABEL ,"",wxBitmap(g_constants.imgs_path+"tb_crop.png",wxBITMAP_TYPE_PNG));
	toolbar->AddTool(MID_COLORS,"",wxBitmap(g_constants.imgs_path+"tb_colors.png",wxBITMAP_TYPE_PNG));
	toolbar->AddSeparator();
	toolbar->AddTool(MID_RUN  ,"",wxBitmap(g_constants.imgs_path+"tb_run.png",wxBITMAP_TYPE_PNG));
	toolbar->AddTool(MID_DEBUG,"",wxBitmap(g_constants.imgs_path+"tb_debug.png",wxBITMAP_TYPE_PNG));
	toolbar->AddSeparator();
	toolbar->AddTool(MID_HELP ,"",wxBitmap(g_constants.imgs_path+"tb_help.png",wxBITMAP_TYPE_PNG));
	toolbar->AddTool(MID_CLOSE,"",wxBitmap(g_constants.imgs_path+"tb_close.png",wxBITMAP_TYPE_PNG));
	toolbar->Realize();
	sizer->Add(new Canvas(this),wxSizerFlags().Expand().Proportion(1));
	SetSizer(sizer);
	g_canvas->SetFocusFromKbd();
	Show();
}

void MainWindow::OnClose (wxCloseEvent & evt) {
	if (g_state.modified) AskForExit(); else evt.Skip();
}

//void MainWindow::SetAccelerators() {
//	wxAcceleratorEntry entries[15];
//	entries[0].Set(wxACCEL_CTRL, 'V', AID_PASTE);
//	wxAcceleratorTable accel(14, entries);
//	SetAcceleratorTable(accel);
//}

void MainWindow::OnMouseWheel (wxMouseEvent & event) {
	g_canvas->OnMouseWheel(event);
}

void MainWindow::ToggleFullScreen ( ) {
	bool fs = not g_main_window->IsFullScreen();
	g_main_window->ShowFullScreen(fs);
}

void MainWindow::AskForExit ( ) {
	int ans=wxMessageBox(_Z("Hay cambios sin aplicar al pseudocódigo.\n¿Aplicar los cambios antes de cerrar el editor?"),g_main_window->GetTitle(),wxYES_NO|wxCANCEL|wxICON_QUESTION,this);
	if (ans==wxYES) {
		Save(); SendUpdate(MO_SAVE); Salir(true);
	} else if (ans==wxNO) {
		Salir(true);
	} else {
		return;
	}
}

void MainWindow::OnToolOver (wxCommandEvent & event) {
	g_toolbar->SetSelection( MIDtoMO(event.GetSelection()) ); 
	g_canvas->Refresh();
}

void MainWindow::OnTool (wxCommandEvent & event) {
	ProcessMenu(MIDtoMO(event.GetId()));
}

int MainWindow::MIDtoMO (int mid) {
	switch(mid) {
	case MID_ZOOM: return MO_ZOOM_EXTEND;
	case MID_FULLSCREEN: return MO_TOGGLE_FULLSCREEN;
	case MID_CROPLABEL: return MO_CROP_LABELS;
	case MID_COMMENTS: return MO_TOGGLE_COMMENTS;
	case MID_STYLE: return MO_CHANGE_STYLE;
	case MID_COLORS: return MO_TOGGLE_COLORS;
	case MID_SUB: return MO_FUNCTIONS;
	case MID_RUN: return MO_RUN;
	case MID_DEBUG: return MO_DEBUG;
	case MID_EXPORT: return MO_EXPORT;
	case MID_CLOSE:return MO_CLOSE;
	case MID_HELP:return MO_HELP;
	default: return MO_NONE;
	}
}

void MainWindow::OnActivated(wxActivateEvent &event) {
	event.Skip();
	g_canvas->SetModifiers(0); // para que Alt+Tab no deje el Alt como apretado
}

