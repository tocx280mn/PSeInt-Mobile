#include <cmath>
#include "GLstuff.h"
#include "glRenderer.h"
#include "Text.h"

glRenderer rndr;

#define PI 3.14159265358979323846

glRenderer::glRenderer() {
	// para no calcular en el DrawShape del Para
	sinx[0]=0; cosx[0]=1;
	for (int i=1;i<circle_steps;i++) {
		sinx[i] = std::sin((i*PI)/circle_steps);
		cosx[i] = std::cos((i*PI)/circle_steps);
	}
	sinx[circle_steps] = 0; cosx[circle_steps] =- 1;
}

glRenderer &glRenderer::setColor(const float * color) {
	glColor3fv(color);
	return *this;
}

glRenderer &glRenderer::setWidth(float line_width) {
	endLines();
	glLineWidth(line_width);
	return *this;
}

void glRenderer::drawQuad (Point p0, Point p1, Point p2, Point p3) {
	endLines();
	glBegin(GL_LINE_LOOP);
	glVertex2f(p0.x,p0.y);
	glVertex2f(p1.x,p1.y);
	glVertex2f(p2.x,p2.y);
	glVertex2f(p3.x,p3.y);
	glEnd();
}

void glRenderer::drawSolidQuad (Point p0, Point p1, Point p2, Point p3) {
	endLines();
	glBegin(GL_QUADS);
	glVertex2f(p0.x,p0.y);
	glVertex2f(p1.x,p1.y);
	glVertex2f(p2.x,p2.y);
	glVertex2f(p3.x,p3.y);
	glEnd();
}

void glRenderer::drawSolidTriangle(Point p0, Point p1, Point p2) {
	endLines();
	glBegin(GL_TRIANGLES);
	glVertex2f(p0.x,p0.y);
	glVertex2f(p1.x,p1.y);
	glVertex2f(p2.x,p2.y);
	glEnd();
}

void glRenderer::drawTriangle(Point p0, Point p1, Point p2) {
	endLines();
	glBegin(GL_LINE_LOOP);
	glVertex2f(p0.x,p0.y);
	glVertex2f(p1.x,p1.y);
	glVertex2f(p2.x,p2.y);
	glEnd();
}

void glRenderer::drawRectangle(Point p, float wl, float wr, float h) {
	drawQuad({p.x-wl,p.y}, {p.x-wl,p.y-h}, {p.x+wr,p.y-h}, {p.x+wr,p.y});
}

void glRenderer::drawSolidRectangle(Point p, float wl, float wr, float h) {
	drawSolidQuad({p.x-wl,p.y}, {p.x-wl,p.y-h}, {p.x+wr,p.y-h}, {p.x+wr,p.y});
}


glRenderer::Point glRenderer::drawText(Point p, const char *str) {
	endLines();
	glPushMatrix();
	glTranslated(p.x,p.y,0);
	glScaled(.105,.15,.1);
	begin_texto();
	while (*str) {
		dibujar_caracter(*(str++));
		p.x += 800.0/7.0 * .105;
	}
	end_texto();
	glPopMatrix();
	return p;
}

glRenderer::Point glRenderer::drawTextEx(Point p, float scale, const char *str, int len, const float colors[][3], const char *highlight) {
	endLines();
	glPushMatrix();
	glTranslated(p.x,p.y,0);
	glScaled(.105*scale,.15*scale,.1);
	begin_texto();
	for (int i=0; i<len; ++i) {
		if (highlight) glColor3fv(colors[highlight[i]]);
		dibujar_caracter(str[i]);
		p.x += 800.0/7.0 * .105 * scale;
	}
	end_texto();
	glPopMatrix();
	return p;
}

void glRenderer::drawLine (Point p0, Point p1) {
	beginLines();
	glVertex2f(p0.x,p0.y);
	glVertex2f(p1.x,p1.y);
}

void glRenderer::beginLines ( ) {
	if (not lines_mode) {
		glBegin(GL_LINES);
		lines_mode = true;
	}
}

void glRenderer::endLines ( ) {
	if (lines_mode) glEnd();
	lines_mode = false;
}

void glRenderer::flush ( ) {
	endLines();
}

void glRenderer::drawSolidCircle (Point p, float r) {
	drawSolidElipse(p,r,r);
}

void glRenderer::drawSolidElipse (Point p, float w, float h) {
	endLines();
	glBegin(GL_QUAD_STRIP);
	for(int i=0;i<=circle_steps;i++) {
		glVertex2f(p.x-sinx[i]*w, p.y+cosx[i]*h);
		glVertex2f(p.x+sinx[i]*w, p.y+cosx[i]*h);
	}
	glEnd();
}

void glRenderer::drawElipse (Point p, float w, float h) {
	endLines();
	glBegin(GL_LINE_LOOP);
	for(int i=0;i<circle_steps;i++)
		glVertex2f(p.x-sinx[i]*w, p.y+cosx[i]*h);
	for(int i=circle_steps;i>=0;i--)
		glVertex2f(p.x+sinx[i]*w, p.y+cosx[i]*h);
	glEnd();
}

void glRenderer::drawHalfElipse (Point p, float w, float h) {
	endLines();
	glBegin(GL_LINE_STRIP);
	for(int i=0;i<=circle_steps;i++)
		glVertex2f(p.x-sinx[i]*w, p.y+cosx[i]*h);
	glEnd();
}

void glRenderer::enableStipple (bool on) {
	endLines();
	(on?glEnable:glDisable)(GL_LINE_STIPPLE);
}

