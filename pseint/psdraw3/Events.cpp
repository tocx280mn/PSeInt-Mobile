#include <cstdlib>
#include <iostream>
#include <wx/timer.h>
#include "GLstuff.h"
#include "Events.h"
#include "Entity.h"
#include "Global.h"
#include "Comm.h"
#include "Load.h"
#include "Textures.h"
#include "MainWindow.h"
#include "Canvas.h"
#include "ShapesBar.h"
#include "Trash.h"
#include "ProcessSelector.h"
#include "EntityEditor.h"

Entity *to_set_mouse = nullptr; // lo que se va a setear en mouse cuando el cursor se mueva un poco si sigue apretado el botón

void Salir(bool force) {
	if ((not force) and g_state.modified) {
		g_main_window->AskForExit();
		return;
	}
	CloseComm();
	wxExit();
}

void reshape_cb (int w, int h) {
	if (w==0||h==0) return;
	if (g_view.win_w!=0 and g_view.win_h!=0) {
		double zw=double(w)/g_view.win_w;
		double zh=double(h)/g_view.win_h;
		g_view.d_dx*=zw/zh;
//		if (zh>zw) zoom*=zh; else zoom*=zw;
		g_view.zoom*=zh;
	}	
	g_view.win_h=h; g_view.win_w=w;
	glViewport(0,0,w,h);
	glMatrixMode (GL_PROJECTION);
	glLoadIdentity ();
	gluOrtho2D(0,w,0,h);
	glMatrixMode (GL_MODELVIEW);
	glLoadIdentity ();
}

void Raise() {
	if (g_main_window->IsIconized()) g_main_window->Iconize(false);
	else { g_main_window->Hide(); wxYield(); g_main_window->Show(); }
}

void idle_func() {
	ReadComm();
	
	// control FPS
	const int DELTA_T=25000;
	static long long last = wxGetLocalTimeMillis().GetValue();
	long long now = wxGetLocalTimeMillis().GetValue();
	if (now-last<DELTA_T) {
		wxMicroSleep(DELTA_T-(now-last));
		last=now;
	}
	
	g_view.d_zoom = 1/((2*1/g_view.d_zoom+1/g_view.zoom)/3);
	Entity::AllIterator it = Entity::AllBegin();
	while (it!=Entity::AllEnd()) {
		it->Tick();
		++it;
	}
	
	Scene::current->ProcessIddle();
	g_trash->ProcessIdle();
	
	g_canvas->Refresh();
}

void passive_motion_cb(int x, int y) {
	Scene::current->ProcessPassiveMotion(x,y);
}

void motion_cb(int x, int y) {
	g_trash->ProcessMotion(x,y);
	Scene::current->ProcessMotion(x,y); 
}

void ZoomExtend(int x0, int y0, int x1, int y1, double max) {
	if (x1<x0) { int aux=x1; x1=x0; x0=aux; }
	if (y0<y1) { int aux=y1; y1=y0; y0=aux; }
	if (x1-x0<10||y0-y1<10) return;
	int h=y0-y1, w=x1-x0;
	const int margin = 40;
	double zh = double(g_view.win_h-margin)/h; // zoom para ajustar alto
	double zw = double(g_view.win_w-g_shapes_bar->GetWidth()-margin)/w; // zoom para ajustar ancho
	if (zw>zh) g_view.zoom = zh; else g_view.zoom = zw; // ver cual tamaño manda
	if (g_view.zoom>max) g_view.zoom = max;
	g_view.d_dx=(g_view.win_w-g_shapes_bar->GetWidth())/g_view.zoom/2-(x1+x0)/2;
	g_view.d_dy=g_view.win_h/g_view.zoom/2-(y1+y0)/2/*+h/2/zoom*/;
}

void ProcessMenu(int op) {
	if (g_state.edit) g_state.edit->UnsetEdit();
	if (op==MO_ZOOM_EXTEND) {
		int h=0,wl=0,wr=0;
		Entity *real_start = g_code.start->GetTopEntity();
		real_start->Calculate(wl,wr,h); // calcular tamaño total
		ZoomExtend(real_start->x-wl,real_start->y,real_start->x+wr,real_start->y-h,1.5);
	} else if (op==MO_TOGGLE_FULLSCREEN) {
		g_main_window->ToggleFullScreen();
	} else if (op==MO_FUNCTIONS) {
		if (g_state.edit) g_state.edit->UnsetEdit();
		if (g_state.mouse) g_state.mouse->UnSetMouse();
		g_process_selector->MakeCurrent();
	} else if (op==MO_SAVE||op==MO_RUN||op==MO_EXPORT||op==MO_DEBUG) {
		SendUpdate(op);
//	} else if (op==MO_SAVE_CLOSE) {
//		SendUpdate(); Salir();
	} else if (op==MO_CLOSE) {
		Salir();
	} else if (op==MO_HELP) {
		SendHelp();
	} else if (op==MO_CROP_LABELS) {
		g_config.enable_partial_text = not g_config.enable_partial_text;
		SendConfig("crop",g_config.enable_partial_text);
		Entity::CalculateAll(true);
	} else if (op==MO_TOGGLE_COMMENTS) {
		g_config.show_comments= not g_config.show_comments;
		Entity::CalculateAll(true);
	} else if (op==MO_TOGGLE_COLORS) {
		if (g_config.dark_theme) {
			g_config.shape_colors = g_config.dark_theme = false;
		} else if (g_config.shape_colors) {
			g_config.dark_theme = true;
		} else {
			g_config.shape_colors = true;
		}
		SendConfig("dark",g_config.dark_theme);
		SendConfig("color",g_config.shape_colors);
		SetColors();
	} else if (op==MO_CHANGE_STYLE) {
		g_config.nassi_shneiderman = not g_config.nassi_shneiderman;
		SendConfig("ns",g_config.nassi_shneiderman);
		Entity::CalculateAll(true);
		ProcessMenu(MO_ZOOM_EXTEND);
	}
}	

void fix_mouse_coords(int &x, int &y) {
	y = g_view.win_h-y; y /= g_view.zoom; x /= g_view.zoom;
}

void mouse_dcb(int x, int y) {
	Scene::current->ProcessDoubleClick(x,y);
}

void mouse_cb(int button, int state, int x, int y) {
	Scene::current->ProcessClick(button,state,x,y);
}

void keyboard_cb(unsigned char key/*, int x, int y*/) {
	Scene::current->ProcessKey(key);
}

void ToggleEditable() {
	static bool old_edit_on;
	if (g_state.edit_on) {
		old_edit_on=true;
		g_state.edit_on=false; if (g_state.edit) g_state.edit->UnsetEdit();
	} else {
		g_state.edit_on = old_edit_on;
	}
	Entity::CalculateAll();
}

void keyboard_esp_cb(int key/*, int x, int y*/) {
	Scene::current->ProcessSpecialKey(key);
}

void FocusEntity(LineInfo *li) {
	if (not li) g_state.debug_current = nullptr;
	if (li->proceso) {
		if (g_code.start!=li->proceso) SetProc(li->proceso);
		g_state.debug_current = li->entidad;
		if (li->entidad) {
			double fx = (g_view.win_w/2)/g_view.zoom;
			double fy = (g_view.win_h/2)/g_view.zoom;
			// que se vea el centro en x de la entidad
			int dx=-li->entidad->x+fx;
			if (dx<g_view.d_dx-fx or dx>g_view.d_dx+fy) g_view.d_dx = dx;
			if (li->entidad->bwl+li->entidad->bwr+fx/20<fx*2) { // si se puede ver todo el ancho... 
				// ..asegurar que se ven los bordes laterales
				int dx0 = -(li->entidad->x-li->entidad->bwl-fx/40);
				int dx1 = -(li->entidad->x+li->entidad->bwr+fx/40-2*fx);
				if (dx1<g_view.d_dx) g_view.d_dx = dx1;
				if (dx0>g_view.d_dx) g_view.d_dx = dx0;
			}
			// que se vea el centro en y de la entidad
			int dy=-li->entidad->y+li->entidad->bh/2+fy;
			if (dy<g_view.d_dy-fy or dy>g_view.d_dy+fy) g_view.d_dy = dy;
		}
	}
	// este else esconde la flecha cuando pasamos por una instruccion
	// que no tiene entidad en el diagrama (como un finsi)
//	else if (not li->proceso) debug_current=nullptr; 
}

void SetModified( ) {
	if (g_state.modified) return;
	g_state.modified = true;
	if (not g_state.loading) NotifyModification();
}

void display_cb() {
	// required somewhere once per main loop iteration
	if (g_code.entity_to_del) delete g_code.entity_to_del;
	
	g_mouse_cursor = Z_CURSOR_CROSSHAIR; // default
	
	glClearColor(g_colors.back[0],g_colors.back[1],g_colors.back[2],1.f);
	glClear(GL_COLOR_BUFFER_BIT);
	
	Scene::current->Draw();
}
