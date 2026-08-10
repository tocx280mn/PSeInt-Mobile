#include <cstddef>
#include "ProcessSelector.h"
#include "Trash.h"
#include "Global.h"
#include "Text.h"
#include "Load.h"
#include "Events.h"
#include "EntityEditor.h"

ProcessSelector *g_process_selector = nullptr;

void ProcessSelector::MakeCurrent ( ) {
	m_anim_base = m_anim_delta = 0; // reset animation
	ClearSelection();
	Scene::MakeCurrent();
}

void ProcessSelector::Draw ( ) {
	glClearColor(g_colors.menu_back[0],g_colors.menu_back[1],g_colors.menu_back[2],1);
	glClear(GL_COLOR_BUFFER_BIT);
	
	glLineWidth(g_constants.menu_line_width);
	DrawTextRaster(g_colors.menu_front,10,g_view.win_h-25,
				   (g_lang[LS_PREFER_FUNCION]
					? (g_lang[LS_PREFER_ALGORITMO]
					   ? "Seleccione un algoritmo/función para visualizar:"
					   : "Seleccione un proceso/función para visualizar:")
					: (g_lang[LS_PREFER_ALGORITMO]
					   ? "Seleccione un proceso/subproceso para visualizar:"
					   : "Seleccione un algoritmo/subalgoritmo para visualizar:") ) );
	
	int base=g_view.win_h-m_anim_base, delta=m_anim_delta;
	glColor3fv(g_colors.menu_front); 
	glBegin(GL_LINES); 
	glVertex2i(0,base); glVertex2i(g_view.win_w,base);
	glEnd();
	for(int i=0;i<=int(g_code.procesos.size()-(g_state.edit_on?0:1));i++) {
		
		if (m_selection==i) {
			glColor3fv(g_colors.menu_sel_back);
			glBegin(GL_QUADS);
			glVertex2i(0,base);
			glVertex2i(g_view.win_w,base);
			glVertex2i(g_view.win_w,base-delta);
			glVertex2i(0,base-delta);
			glEnd();
		}
		base-=delta;
		
		if (i==int(g_code.procesos.size())) {
			DrawTextRaster(/*m_selection==i?g_colors.selection:*/g_colors.menu_front,20,base+10,
						   g_lang[LS_PREFER_FUNCION]?"Agregar Nueva Función"
						   :(g_lang[LS_PREFER_ALGORITMO]?"Agregar Nuevo SubAlgoritmo"
							 :"Agregar Nuevo SubProceso") );
		} else {
			
			const float *color = /*m_selection==i?g_colors.selection:*/g_colors.menu_front;
			glColor3fv(color);
			glPushMatrix();
			int px=20, py=base+10;
			if (m_mouse_down and m_selection==i) glTranslated(g_state.cur_x+(px-g_state.m_x0),g_state.cur_y+(py-g_state.m_y0),0);
			else glTranslated(px,py,0);
			double sf = g_config.big_icons ? 1.5 : 1; glScaled(.08*sf,.12*sf,.1*sf);
			
			glLineWidth(g_config.big_icons?2:1);
			begin_texto();
			{
				std::string &sp = g_code.procesos[i]->lpre;
				int p=0; int l=sp.size();
				while (p<l)
					dibujar_caracter(sp[p++]);
			}
			
			{
				std::string &s = g_code.procesos[i]->label;
				int l=s.size(),p=0; size_t f=s.find('<');
				if (f==std::string::npos) f=s.find('='); else f++;
				if (f==std::string::npos) f=0; else f++;
				int t=f; while (t<l && s[t]!=' ' && s[t]!='(') t++;
				while (p<l) {
					if (p==int(f)) glColor3fv(g_colors.menu_front_bold);
					else if (p==t) glColor3fv(color);
					dibujar_caracter(s[p++]);
				}
			}
			
			end_texto();
			glPopMatrix();		
		}
		
		glLineWidth(1);
		glColor3fv(g_colors.menu_front); 
		glBegin(GL_LINES); 
		glVertex2i(0,base); glVertex2i(g_view.win_w,base);
		glEnd();
	}
	glEnd();
	g_mouse_cursor = Z_CURSOR_INHERIT;
	g_trash->Draw();
}

void ProcessSelector::ProcessMotion (int x, int y) {
	g_state.cur_x = x; g_state.cur_y = g_view.win_h-y;
	int aux_selection = (y-m_anim_base)/m_anim_delta;
	if (m_mouse_down) {
		if (aux_selection!=m_selection) m_dragged = true; // para que si arrastramos, al soltar no pase a editarlo
	} else if (m_anim_delta) {
		m_selection=aux_selection;
		if (y<m_anim_base or m_selection<0 or m_selection>int(g_code.procesos.size()-(g_state.edit_on?0:1)))
			m_selection=NO_SELECTION;
	}
}

void ProcessSelector::ProcessPassiveMotion(int x, int y) {
	ProcessMotion(x,y);
}

void ProcessSelector::ProcessClick (int button, int state, int x, int y) {
	if (state==ZMB_UP and m_selection==int(g_code.procesos.size())) {
		CreateEmptyProc(g_lang[LS_PREFER_FUNCION]?"Funcion":(g_lang[LS_PREFER_ALGORITMO]?"SubAlgoritmo":"SubProceso"));
		button = ZMB_RIGHT; // para que al mostrar el nuevo proceso comience editando el nombre
	}
	if (m_selection!=NO_SELECTION) {
		if (state==ZMB_DOWN) {
			m_mouse_down = true; m_dragged = false;
			if (g_state.edit_on) g_trash->Show();
			g_state.cur_x = g_state.m_x0 = x; g_state.cur_y = g_state.m_y0 = g_view.win_h-y;
		} else if (g_trash->IsSelected()) {
			if (g_state.edit_on and (g_code.procesos[m_selection]->lpre!="Proceso " and g_code.procesos[m_selection]->lpre!="Algoritmo "))
				g_code.procesos.erase(g_code.procesos.begin()+m_selection); // no lo quita de la memoria, solo del arreglo, con eso alcanza, algun día corregiré el memory leak
			ClearSelection();
		} else if (not m_dragged) {
			SetProc(g_code.procesos[m_selection]);
			ClearSelection();
			g_entity_editor->MakeCurrent();
			if (button==ZMB_RIGHT) g_code.start->SetEdit();
		} else
			ClearSelection();
	}
}

void ProcessSelector::Initialize ( ) {
	g_process_selector = new ProcessSelector();
}

void ProcessSelector::ProcessIddle ( ) {
	if (g_config.big_icons) {
		interpolate_good(m_anim_base,50);
		interpolate_good(m_anim_delta,40);
	} else {
		interpolate_good(m_anim_base,40);
		interpolate_good(m_anim_delta,30);
	}
}

void ProcessSelector::ProcessKey(unsigned char key) {
	if (key==27) g_entity_editor->MakeCurrent();
}

void ProcessSelector::ClearSelection ( ) {
	m_mouse_down = false;
	m_selection = NO_SELECTION;
	g_trash->Hide();
}

