#include "../wxPSeInt/string_conversions.h"
#include "Trash.h"
#include "Global.h"
#include "Text.h"

Trash *g_trash = nullptr;

#define trash_size_max 100
#define trash_size_min 30

Trash::Trash() 
	: m_texture(g_constants.imgs_path+"trash.png"),
	  m_visible(false), m_extended(false), m_size(0) 
{
	
}

void Trash::Draw ( ) {
	if (!m_size) return;
	
	if (m_extended) {
		DrawTextRaster(g_colors.status,10+trash_size_max,10,_Z("Eliminar"));
		g_mouse_cursor = Z_CURSOR_DESTROY;
	}
	
	glLineWidth(g_constants.menu_line_width);
	glColor3fv(g_colors.menu_back);
	glBegin(GL_QUADS);
	// trash
	glVertex2i(0,m_size);
	glVertex2i(m_size,m_size);
	glVertex2i(m_size,0);
	glVertex2i(0,0);
	glEnd();
	glColor3fv(g_colors.menu_front);
	glBegin(GL_LINES);
	glVertex2i(0,m_size); glVertex2i(m_size,m_size);
	glVertex2i(m_size,0); glVertex2i(m_size,m_size);
	glEnd();
	glEnable(GL_TEXTURE_2D);
	m_texture.Select();
	glColor3f(1,1,1);
	glBegin(GL_QUADS);
	if (g_trash) {
		glTexCoord2f(0*m_texture.max_s,.77*m_texture.max_t); glVertex2i(0,m_size);
		glTexCoord2f(1*m_texture.max_s,.77*m_texture.max_t); glVertex2i(m_size,m_size);
		glTexCoord2f(1*m_texture.max_s,0*m_texture.max_t); glVertex2i(m_size,0);
		glTexCoord2f(0*m_texture.max_s,0*m_texture.max_t); glVertex2i(0,0);
	} else {
		glTexCoord2f(0*m_texture.max_s,1*m_texture.max_t); glVertex2i(0,m_size);
		glTexCoord2f(.32*m_texture.max_s,1*m_texture.max_t); glVertex2i(m_size,m_size);
		glTexCoord2f(.32*m_texture.max_s,.77*m_texture.max_t); glVertex2i(m_size,0);
		glTexCoord2f(0*m_texture.max_s,.77*m_texture.max_t); glVertex2i(0,0);
	}
	glEnd();
	glDisable(GL_TEXTURE_2D);
}

void Trash::Initialize ( ) {
	g_trash = new Trash();
}

void Trash::ProcessIdle ( ) {
	int dest = m_visible ? ( m_extended ? trash_size_max : trash_size_min ) : 0;
	interpolate(m_size,dest);
}

void Trash::ProcessMotion (int x, int y) {
	m_extended = m_visible && x<m_size && (g_view.win_h-y)<m_size;	
}

