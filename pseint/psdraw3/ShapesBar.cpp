#include <cstddef>
#include "ShapesBar.h"
#include "Canvas.h"
#include "Entity.h"
#include "Global.h"
#include "Events.h"
#include "Text.h"

#include "StatusBar.h"

ShapesBar *g_shapes_bar = nullptr;

ShapesBar::ShapesBar() 
	: m_texture_retracted(g_constants.imgs_path+"commands.png"),
	  m_texture_extended( g_config.nassi_shneiderman 
							? g_constants.imgs_path+"shapes_ns.png"
							: ( g_config.alternative_io 
								  ? g_constants.imgs_path+"shapes_alt.png"
								  : g_constants.imgs_path+"shapes.png" ) ),
	  m_visible(true), m_extended(false), m_fixed(false), m_width(0),
	  m_has_mouse(false), m_current_selection(NO_SELECTION)
{
	shapebar_size_min = g_config.big_icons ? 34 : 25;
	shapebar_size_max = g_config.big_icons ? 200 : 150;
}

void ShapesBar::ProcessMotion (int x, int y) {
	if (!m_visible) { m_current_selection = NO_SELECTION; return; }
	m_extended = x > g_view.win_w-m_width;
	if (m_extended) {
		m_current_selection = y/(g_view.win_h/CANT_SHAPES_IN_BAR)+1;
		if (m_current_selection > CANT_SHAPES_IN_BAR) m_current_selection = NO_SELECTION;
	} else {
		m_current_selection=NO_SELECTION; return;
	}
}

bool ShapesBar::ProcessMouse (int button, int state, int x, int y) {
	ProcessMotion(x,y); // ensures updated m_current_selection
	if (m_current_selection==NO_SELECTION) return false;
	if (button!=ZMB_LEFT||state!=ZMB_DOWN) return true;
	m_extended = false;
	
	Entity*new_entity = nullptr;
	switch (m_current_selection) {
	case 1: 
		if (not g_config.show_comments) ProcessMenu(MO_TOGGLE_COMMENTS);
		new_entity = new Entity(ET_COMENTARIO,"");
		if (g_canvas->GetModifiers()&MODIFIER_SHIFT) new_entity->variante=true;
		break;
	case 2: 
		new_entity = new Entity(ET_ASIGNAR,""); 
		if (g_canvas->GetModifiers()&MODIFIER_SHIFT) new_entity->variante=true;
		break;
	case 3: new_entity = new Entity(ET_ESCRIBIR,""); break;
	case 4: new_entity = new Entity(ET_LEER,""); break;
	case 5: new_entity = new Entity(ET_SI,""); break;
	case 6: new_entity = new Entity(ET_SEGUN,""); break;
	case 7: new_entity = new Entity(ET_MIENTRAS,""); break;
	case 8: 
		new_entity = new Entity(ET_REPETIR,""); 
		if ( g_lang[LS_ALLOW_REPEAT_WHILE] and (g_canvas->GetModifiers()&MODIFIER_SHIFT)!=g_lang[LS_PREFER_REPEAT_WHILE] ) new_entity->variante=true;
		break;
	case 9:
		new_entity = new Entity(ET_PARA,""); 
		if (g_lang[LS_ALLOW_FOR_EACH] and (g_canvas->GetModifiers()&MODIFIER_SHIFT)) new_entity->variante=true;
		break;
	}
	if (new_entity) {
		fix_mouse_coords(x,y);
		new_entity->m_x = 0; new_entity->m_y = 0;
		to_set_mouse = new_entity;
		new_entity->SetMouse();					
		new_entity->SetEdit(false);
		new_entity->SetPosition(x,y);
	}
	return true;
}

void ShapesBar::Draw() {
	if (m_width == 0) return;
	glLineWidth(g_constants.menu_line_width);
	glColor3fv(g_colors.menu_back);
	// shapebar
	glBegin(GL_QUADS);
	glVertex2i(g_view.win_w-m_width,0);
	glVertex2i(g_view.win_w-m_width,g_view.win_h);
	glVertex2i(g_view.win_w,g_view.win_h);
	glVertex2i(g_view.win_w,0);
	glEnd();
	// shapebar
	glColor3fv(g_colors.menu_front);
	glBegin(GL_LINES);
	glVertex2i(g_view.win_w-m_width,0);
	glVertex2i(g_view.win_w-m_width,g_view.win_h);
	glEnd();
	if (m_extended||m_fixed) {
		g_mouse_cursor = Z_CURSOR_INHERIT;
		double sh = double(g_view.win_h)/CANT_SHAPES_IN_BAR;
		// resaltado(fondo) de la entidad seleccionada
		if(m_current_selection!=NO_SELECTION) {
			glColor3fv(g_colors.menu_sel_back);
			glBegin(GL_QUADS);
			glVertex2d(g_view.win_w,        g_view.win_h-sh*(m_current_selection-1)); 
			glVertex2d(g_view.win_w-m_width,g_view.win_h-sh*(m_current_selection-1)); 
			glVertex2d(g_view.win_w-m_width,g_view.win_h-sh*(m_current_selection-0)); 
			glVertex2d(g_view.win_w,        g_view.win_h-sh*(m_current_selection-0)); 
			glEnd();
		}
		
		// imagenes de cada entidad
		glEnable(GL_TEXTURE_2D);
		m_texture_extended.Select();
		if (m_visible) glColor3f(1,1,1); 
		else glColor4f(1,1,1,.5); // dimm when fixed but not enabled
		glBegin(GL_QUADS);
		double dy=sh, th0=0, th1=1.0/double(CANT_SHAPES_IN_BAR), dth=1.0/double(CANT_SHAPES_IN_BAR);
		double x0=g_view.win_w-m_width, x1=g_view.win_w, y0=0, y1=double(g_view.win_h)/double(CANT_SHAPES_IN_BAR); 
		double ratio = double(m_width)/double(g_view.win_h)/double(CANT_SHAPES_IN_BAR);
		if (ratio>(m_texture_extended.r/8)) {
			double dx=(x1-x0)*(1-(m_texture_extended.r/8)/ratio)/2;
			x0+=dx; x1-=dx;
		} else {
			double dy=(y1-y0)*(1-ratio/(m_texture_extended.r/8))/2;
			y0+=dy; y1-=dy;
		}
		for(int i=0;i<CANT_SHAPES_IN_BAR;i++) {
			glTexCoord2f(0*m_texture_extended.max_s,th0*m_texture_extended.max_t); glVertex2d(x0,y0);
			glTexCoord2f(1*m_texture_extended.max_s,th0*m_texture_extended.max_t); glVertex2d(x1,y0);
			glTexCoord2f(1*m_texture_extended.max_s,th1*m_texture_extended.max_t); glVertex2d(x1,y1);
			glTexCoord2f(0*m_texture_extended.max_s,th1*m_texture_extended.max_t); glVertex2d(x0,y1);
			y0+=dy; y1+=dy; th0+=dth; th1+=dth;
		}
		glEnd();
		glDisable(GL_TEXTURE_2D);
		// lineas que separan las entidades
		glBegin(GL_LINES);
		glColor3fv(g_colors.menu_front);
		y0 = g_view.win_h-dy;
		for(int i=0;i<CANT_SHAPES_IN_BAR-1;i++,y0-=dy) {
			glVertex2d(g_view.win_w-m_width,y0);
			glVertex2d(g_view.win_w,y0);
		}
		glEnd();
	} else { 
		glEnable(GL_TEXTURE_2D);
		m_texture_retracted.Select();
		glColor3f(1,1,1);
		glBegin(GL_QUADS);
		glTexCoord2f(0*m_texture_retracted.max_s,0*m_texture_retracted.max_t); glVertex2i(g_view.win_w-m_width,(g_view.win_h-m_texture_retracted.h)/2);
		glTexCoord2f(1*m_texture_retracted.max_s,0*m_texture_retracted.max_t); glVertex2i(g_view.win_w-m_width+m_texture_retracted.w,(g_view.win_h-m_texture_retracted.h)/2);
		glTexCoord2f(1*m_texture_retracted.max_s,1*m_texture_retracted.max_t); glVertex2i(g_view.win_w-m_width+m_texture_retracted.w,(g_view.win_h+m_texture_retracted.h)/2);
		glTexCoord2f(0*m_texture_retracted.max_s,1*m_texture_retracted.max_t); glVertex2i(g_view.win_w-m_width,(g_view.win_h+m_texture_retracted.h)/2);
		glEnd();
		glDisable(GL_TEXTURE_2D);
	}
	
	// shapebar
	if (m_visible) {
		switch(m_current_selection) {
		case 1:
			if (g_canvas->GetModifiers()&MODIFIER_SHIFT)
				g_status_bar->Set("Comentario en instrucción (texto libre que el interprete ignora)");
			else
				g_status_bar->Set("Comentario independiente (texto libre que el interprete ignora)");
			break;
		case 2: 
			if (g_canvas->GetModifiers()&MODIFIER_SHIFT)
				g_status_bar->Set("Invocación de un subproceso");
			else
				g_status_bar->Set("Asignación/Dimensión/Definición"); 
			break;
		case 3: g_status_bar->Set("Escribir (instrucción para generar salidas)"); break;
		case 4: g_status_bar->Set("Leer (instrucción para obtener entradas)"); break;
		case 5: g_status_bar->Set("Si-Entonces (estructura condicional simple)"); break;
		case 6: g_status_bar->Set("Según (estructura de selección múltiple)"); break;
		case 7: g_status_bar->Set("Mientras (estructura repetitiva)"); break;
		case 8: 
			if ((g_canvas->GetModifiers()&MODIFIER_SHIFT)!=g_lang[LS_PREFER_REPEAT_WHILE])
				g_status_bar->Set("Repetir-Mientras que (estructura repetitiva)"); 
			else
				g_status_bar->Set("Repetir-Hasta que (estructura repetitiva)");
			break;
		case 9: 
			if (g_canvas->GetModifiers()&MODIFIER_SHIFT)
				g_status_bar->Set("Para Cada (estructura repetitiva)"); 
			else
				g_status_bar->Set("Para (estructura repetitiva)"); 
			break;
		default:;
		}
	}
}

void ShapesBar::Initialize ( ) {
	g_shapes_bar = new ShapesBar();
}


void ShapesBar::ProcessIdle ( ) {
	int dest = (m_visible||m_fixed) ? ((m_fixed||m_extended) ? shapebar_size_max : shapebar_size_min ) : 0;
	interpolate(m_width, dest);
}

int ShapesBar::GetWidth() const {
	return (m_visible||m_fixed) ? ((m_fixed||m_extended) ? shapebar_size_max : shapebar_size_min ) : 0;
};
