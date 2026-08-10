#include "GLstuff.h"
#include "StatusBar.h"
#include "Global.h"
#include "Text.h"

StatusBar *g_status_bar = nullptr;

void StatusBar::Initialize ( ) {
	g_status_bar = new StatusBar;
}

void StatusBar::Set(const std::string &text) {
	Set(g_colors.status,text);
}

void StatusBar::Set(const float *color, const std::string &text) {
	status_color = color;
	status_text = text;
}

void StatusBar::Clear ( ) {
	status_color = nullptr;
	status_text.clear();
}

void StatusBar::Draw ( ) const {
	if (not status_color) return;
	glColor3fv(g_colors.back);
	int w=status_text.size()*get_char_width(),bh=10,bw=10,h=15;
	bw-=3; w+=6; h+=6; bh-=6;
	glBegin(GL_QUADS);
		glVertex2i(bw,bh);
		glVertex2i(bw,bh+h);
		glVertex2i(bw+w,bh+h);
		glVertex2i(bw+w,bh);
	glEnd();
	DrawTextRaster(status_color,10,10,status_text.c_str());
}

