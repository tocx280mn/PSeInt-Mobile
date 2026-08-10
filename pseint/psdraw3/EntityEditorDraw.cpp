#include <cstring>
#include "GLstuff.h"
#include <wx/cursor.h>
#include "MainWindow.h"
#include "ProcessSelector.h"
#include "Global.h"
#include "Entity.h"
#include "Events.h"
#include "Textures.h"
#include "Canvas.h"
#include "Text.h"
#include "ShapesBar.h"
#include "ToolBar.h"
#include "Trash.h"
#include "../pseint/strFuncs.hpp"
#include "EntityEditor.h"
#include "StatusBar.h"

class RaiiColorChanger {
	float *p[5], v[5]; int n;
public:
	RaiiColorChanger() : n(0) {}
	void Change(float &c, float x) { p[n]=&c; v[n]=c; c=x; n++; }
	~RaiiColorChanger() { for(int i=0;i<n;i++) *(p[i])=v[i]; }
};


#define mouse_link_delta 250
static int mouse_link_x=0,mouse_link_y=0; 

// dibuja una marca al lado de la entidad que indica que tiene un error de sintaxis
void draw_error_mark(Entity *e, double delta) {
	double x=e->d_fx-e->w/2+delta;//-2*delta;
	double y=e->d_fy-e->h/2+2*delta;
	glColor3f(1,.85,.85);
	glBegin(GL_QUADS);
		glVertex2d(x+delta,y);
		glVertex2d(x,y-2*delta);
		glVertex2d(x-2*delta,y-2*delta);
		glVertex2d(x,y+2*delta);
		glVertex2d(x-2*delta,y-2*delta);
		glVertex2d(x-3*delta,y);
		glVertex2d(x-2*delta,y+2*delta);
		glVertex2d(x,y+2*delta);
	glEnd();
	glColor3fv(g_colors.error);
	glBegin(GL_LINE_LOOP);
		glVertex2d(x+delta,y);
		glVertex2d(x,y-2*delta);
		glVertex2d(x-2*delta,y-2*delta);
		glVertex2d(x-3*delta,y);
		glVertex2d(x-2*delta,y+2*delta);
		glVertex2d(x,y+2*delta);
	glEnd();
	glBegin(GL_LINES);
		glVertex2d(x,y-delta);
		glVertex2d(x-2*delta,y+delta);
		glVertex2d(x,y+delta);
		glVertex2d(x-2*delta,y-delta);
	glEnd();
}

void draw_error_mark_simple(Entity *e, double delta) {
	double x=e->d_fx-e->w/2-delta;
	double y=e->d_fy-e->h/2+2*delta;
	glColor3fv(g_colors.error);
	glBegin(GL_LINES);
		glVertex2d(x,y-delta);
		glVertex2d(x-2*delta,y+delta);
		glVertex2d(x,y+delta);
		glVertex2d(x-2*delta,y-delta);
	glEnd();
}

// dibuja una flecha al lado de la entidad que indica que es el punto actual de la ejecucion paso a paso
void draw_debug_arrow(Entity *e, double delta) {
	static double prev_x = 0, prev_y = 0;
	double cur_x=e->d_fx-e->w/2-2*delta;
	double cur_y=e->d_fy-e->h/2;
	double x = (2*prev_x+cur_x)/3; prev_x = x;
	double y = (2*prev_y+cur_y)/3; prev_y = y;
	glColor3f(0,1,0);
	glBegin(GL_TRIANGLES);
		glVertex2d(x,y-2*delta);
		glVertex2d(x,y+2*delta);
		glVertex2d(x+delta,y);
	glEnd();
	glBegin(GL_QUADS);
		glVertex2d(x-delta,y-delta);
		glVertex2d(x,y-delta);
		glVertex2d(x,y+delta);
		glVertex2d(x-delta,y+delta);
	glEnd();
	glColor3f(0,.5,0);
	glBegin(GL_LINE_LOOP);
		glVertex2d(x-delta,y-delta);
		glVertex2d(x,y-delta);
		glVertex2d(x,y-2*delta);
		glVertex2d(x+delta,y);
		glVertex2d(x,y+2*delta);
		glVertex2d(x,y+delta);
		glVertex2d(x-delta,y+delta);
	glEnd();
}

void MoveToNext(Entity *mouse, Entity *aux, bool calculate = true){
	Entity *mouse_next = mouse->GetNext();
	mouse->UnLink();
	aux->LinkNext(mouse);
	if (mouse_next && mouse_next->nolink) {
		MoveToNext(mouse_next,mouse,false);
	}
	if (calculate) Entity::CalculateAll();
}

void MoveToChild(Entity *mouse, Entity *aux, int i){
	Entity *mouse_next = mouse->GetNext();
	mouse->UnLink();
	aux->LinkChild(i,mouse);
	if (mouse_next && mouse_next->nolink==mouse) {
		MoveToNext(mouse,mouse_next,false);
	}
	Entity::CalculateAll();
}

void EntityEditor::Draw() {
	g_status_bar->Clear();
	// dibujar el diagrama
	double mx = g_state.cur_x/g_view.zoom, my = g_state.cur_y/g_view.zoom;
	Entity *aux = g_code.start->GetTopEntity();
	Entity *my_start = aux;
	bool found=false;
	g_constants.line_width_flechas = 2*g_view.d_zoom<1?1:int(g_view.d_zoom*2);
	g_constants.line_width_bordes = 1*g_view.d_zoom<1?1:int(g_view.d_zoom*1);
	glLineWidth(g_constants.line_width_flechas);
	glPushMatrix();
	glScalef(g_view.d_zoom,g_view.d_zoom,1);
	do {
		if ((not found) and g_state.mouse and g_state.mouse->type!=ET_OPCION and (g_state.cur_x-mouse_link_x)*(g_state.cur_x-mouse_link_x)+(g_state.cur_y-mouse_link_y)*(g_state.cur_y-mouse_link_y)>mouse_link_delta) {
			if (aux->CheckLinkNext(g_state.cur_x,g_state.cur_y) and (not g_state.mouse->Contains(aux))) {
				mouse_link_x = g_state.cur_x; mouse_link_y = g_state.cur_y;
				MoveToNext(g_state.mouse,aux);
				found=true;
			} else if (aux->GetChildCount()) {
				int i=aux->CheckLinkChild(g_state.cur_x,g_state.cur_y);
				if (i!=-1) {
					mouse_link_x = g_state.cur_x; mouse_link_y = g_state.cur_y;
					MoveToChild(g_state.mouse,aux,i);
					found=true;
				}
			}
		}
		if (g_state.edit_on and (g_state.mouse?(aux==g_state.mouse):aux->CheckMouse(mx,my,false))) {
			RaiiColorChanger rcc;
			rcc.Change(g_colors.shape[g_config.shape_colors?aux->type:ET_COUNT][2],.75); 
			rcc.Change(g_colors.arrow[1],.5); rcc.Change(g_colors.arrow[2],.5); // rcc.Change(color_arrow[0],1);
			g_constants.line_width_bordes*=2;
			aux->Draw(aux->type==ET_OPCION);
			g_constants.line_width_bordes/=2;
			if (aux->error.size()) g_status_bar->Set(g_colors.error,aux->error);
			else if ((not g_state.mouse) and aux->IsLabelCropped()) g_status_bar->Set(g_config.dark_theme?g_colors.ghost:g_colors.label_high[0],aux->label);
		} else if (g_state.debugging and g_state.debug_current==aux) {
			RaiiColorChanger rcc;
			g_constants.line_width_bordes*=2;
			if (not g_config.nassi_shneiderman) {
				rcc.Change(g_colors.shape[g_config.shape_colors?aux->type:ET_COUNT][2],.65); rcc.Change(g_colors.arrow[1],.4);
				rcc.Change(g_colors.arrow[2],.4); // rcc.Change(color_arrow[0],1);
			}
			aux->Draw();
			g_constants.line_width_bordes /= 2;
			draw_debug_arrow(g_state.debug_current,5);
		} else {
			aux->Draw();
		}
		if (not aux->error.empty()) draw_error_mark/*_simple*/(aux,4);
		if ((not g_state.mouse) and g_state.edit==aux and aux->CheckMouse(mx,my,false)) g_mouse_cursor = Z_CURSOR_TEXT;
		aux = Entity::NextEntity(aux);
	} while (aux);
	if (g_state.mouse and (not g_state.mouse->GetPrev()) and (not g_state.mouse->GetParent())) 
		g_state.mouse->Draw(); // cuando recien salen de la shapebar no esta linkeadas al algoritmo, no los toma la recorrida anterior
	if (g_state.mouse and g_state.mouse->type==ET_OPCION) {
		Entity *segun = g_state.mouse->GetParent();
		int new_id = segun->CheckLinkOpcion(g_state.cur_x,g_state.cur_y);
		if (new_id!=-1) {
			int old_id = g_state.mouse->GetChildId();
			segun->RemoveChild(old_id,false);
			if (old_id<new_id) --new_id;
			segun->InsertChild(new_id,g_state.mouse);
			segun->Calculate();
		}
	}
	
	if (g_state.selecting_zoom or g_state.selecting_entities) {
		glColor3fv(g_colors.menu_front);
		glBegin(GL_LINE_LOOP);
			glVertex2i(g_state.m_x0,g_state.m_y0);
			glVertex2i(g_state.m_x0,g_state.cur_y);
			glVertex2i(g_state.cur_x,g_state.cur_y);
			glVertex2i(g_state.cur_x,g_state.m_y0);
		glEnd();
	}
	glPopMatrix();
	// dibujar menues y demases
	glLineWidth(2);
	g_shapes_bar->Draw(); 
	g_toolbar->Draw(); 
	g_trash->Draw();
	if (g_state.edit and (not g_state.mouse) and g_status_bar->IsEmpty()) {
		switch (g_state.edit->type) {
		case ET_LEER:       g_status_bar->Set("? Lista de variables a leer, separadas por coma."); break;
		case ET_PROCESO:    g_status_bar->Set(LeftCompare(g_state.edit->lpre,g_lang.keywords[KW_ALGORITMO].get(false))?"? Nombre del algoritmo.":"? Prototipo de la función."); break;
		case ET_COMENTARIO: g_status_bar->Set("? Texto libre, será ignorado por el interprete."); break;
		case ET_ESCRIBIR:   g_status_bar->Set("? Lista de expresiones a mostrar, separadas por comas."); break;
		case ET_SI:         g_status_bar->Set("? Expresión lógica."); break;
		case ET_SEGUN:      g_status_bar->Set("? Expresión de control para la estructura."); break;
		case ET_OPCION:     g_status_bar->Set("? Posible valor para la expresión de control."); break;
		case ET_PARA: 
			if (g_state.edit->variante)	g_status_bar->Set("? Identificador temporal para el elemento del vector/matriz.");
			else                        g_status_bar->Set("? Identificador de la variable de control (contador)."); 
			break;
		case ET_MIENTRAS: g_status_bar->Set("? Expresión de control (lógica)."); break;
		case ET_REPETIR:  g_status_bar->Set("? Expresión de control (lógica)."); break;
		case ET_ASIGNAR:  g_status_bar->Set("? Asignación o instruccion secuencial."); break;
		case ET_AUX_PARA: 
			if (g_state.edit->GetParent()->variante)
				g_status_bar->Set("? Identificador del vector/matriz a recorrer.");
			else g_status_bar->Set(
						   g_state.edit->GetParent()->GetChild(1)==g_state.edit
								? "? Valor inicial para el contador."
								: ( g_state.edit->GetParent()->GetChild(2)==g_state.edit
									? "? Paso, incremento del contador por cada iteración."
								    : "? Valor final para el contador." ) ); 
			break;
		default:;
		}
	}
	// dibujar la seleccion para que quede delante de todo
	if (g_state.mouse and (not g_trash)) {
		glLineWidth(g_view.zoom*2);
		glPushMatrix();
		glScalef(g_view.d_zoom,g_view.d_zoom,1);
		g_state.mouse->Draw();
		glPopMatrix();
		g_mouse_cursor = Z_CURSOR_NONE;
	}
	// barra de estado
	g_status_bar->Draw();
}

