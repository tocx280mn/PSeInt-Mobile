#include "mxRenderer.h"
#include "../wxPSeInt/string_conversions.h"

mxRenderer rndr;

mxRenderer::mxRenderer() :
	m_color(0,0,0),
	m_pen_transparent(wxPenInfo(m_color,0).Style(wxPENSTYLE_TRANSPARENT)),
	m_brush_transparent(m_color,wxBRUSHSTYLE_TRANSPARENT),
	m_font(wxFontInfo(14).Family(wxFONTFAMILY_MODERN))
{
//	m_font.SetPixelSize(wxSize(0,250*.105));
#ifndef __APPLE__
	if (wxFont::AddPrivateFont("Inconsolata-Regular.ttf"))
#endif
		m_font.SetFaceName("Inconsolata");
}

void mxRenderer::setDC(wxDC *target_dc, bool for_svg) {
	m_for_svg = for_svg;
	m_dc = target_dc;
	setFontSize();
}

void mxRenderer::clear(const float *color) {
	setColor(color);
	m_dc->SetBackground(m_color);
	m_dc->Clear();
}

void mxRenderer::finish() {
	delete m_dc; m_dc = nullptr;
}

void mxRenderer::setFontSize() {
	m_font.SetPointSize((m_for_svg?16:13)*m_scale); // por alguna razon el tamaño de fuente se ve diferente en el memorydc vs el svgfiledc
	m_dc->SetFont(m_font);
	wxCoord aux;
	m_dc->GetTextExtent("ABC123",&aux,&m_text_h);
}

void mxRenderer::setTransformation(Point p_orig, float scale) {
	m_orig = p_orig;
	m_scale = scale;
	setFontSize();
}

void mxRenderer::transform(Point &p) {
	p.x *=  m_scale; p.x += m_orig.x;
	p.y *= -m_scale; p.y += m_orig.y;
}



mxRenderer &mxRenderer::setColor(const float * color) {
	m_color.Set(int(255*color[0]),int(255*color[1]),int(255*color[2]));
	return *this;
}

mxRenderer &mxRenderer::setWidth(float line_width) {
	m_linewidth = line_width;
	return *this;
}

void mxRenderer::drawQuad (Point p0, Point p1, Point p2, Point p3) {
	transform(p0); transform(p1); transform(p2); transform(p3);
	m_dc->SetPen(wxPen(m_color,m_linewidth));
	m_dc->SetBrush(m_brush_transparent);
	wxPoint pts[4] = { {p0.x,p0.y}, {p1.x,p1.y}, {p2.x,p2.y}, {p3 .x,p3.y}};
	m_dc->DrawPolygon(4,pts);
}

void mxRenderer::drawSolidQuad (Point p0, Point p1, Point p2, Point p3) {
	transform(p0); transform(p1); transform(p2); transform(p3);
	m_dc->SetPen(m_pen_transparent);
	m_dc->SetBrush(wxBrush(m_color, wxBRUSHSTYLE_SOLID));
	wxPoint pts[4] = { {p0.x,p0.y}, {p1.x,p1.y}, {p2.x,p2.y}, {p3 .x,p3.y}};
	m_dc->DrawPolygon(4,pts);
}

void mxRenderer::drawSolidTriangle(Point p0, Point p1, Point p2) {
	transform(p0); transform(p1); transform(p2);
	m_dc->SetPen(m_pen_transparent);
	m_dc->SetBrush(wxBrush(m_color, wxBRUSHSTYLE_SOLID));
	wxPoint pts[3] = { {p0.x,p0.y}, {p1.x,p1.y}, {p2 .x,p2.y}};
	m_dc->DrawPolygon(3,pts);
}

void mxRenderer::drawTriangle(Point p0, Point p1, Point p2) {
	transform(p0); transform(p1); transform(p2);
	m_dc->SetPen(wxPen(m_color,m_linewidth));
	m_dc->SetBrush(m_brush_transparent);
	wxPoint pts[3] = { {p0.x,p0.y}, {p1.x,p1.y}, {p2 .x,p2.y}};
	m_dc->DrawPolygon(3,pts);
}

void mxRenderer::drawRectangle(Point p, float wl, float wr, float h) {
	drawQuad({p.x-wl,p.y}, {p.x-wl,p.y-h}, {p.x+wr,p.y-h}, {p.x+wr,p.y});
}

void mxRenderer::drawSolidRectangle(Point p, float wl, float wr, float h) {
	drawSolidQuad({p.x-wl,p.y}, {p.x-wl,p.y-h}, {p.x+wr,p.y-h}, {p.x+wr,p.y});
}

void mxRenderer::drawLine (Point p0, Point p1) {
	transform(p0); transform(p1);
	m_dc->SetPen(wxPen(m_color,m_linewidth));
	m_dc->DrawLine(p0.x,p0.y,p1.x,p1.y);
}

void mxRenderer::drawSolidCircle (Point p, float r) {
	drawSolidElipse(p,r,r);
}

void mxRenderer::drawSolidElipse (Point p, float w, float h) {
	transform(p); w*=m_scale; h*=m_scale;
	m_dc->SetPen(m_pen_transparent);
	m_dc->SetBrush(wxBrush(m_color, wxBRUSHSTYLE_SOLID));
	m_dc->DrawEllipse(p.x-w, p.y-h, 2*w, 2*h);
}

void mxRenderer::drawElipse (Point p, float w, float h) {
	transform(p); w*=m_scale; h*=m_scale;
	m_dc->SetPen(wxPen(m_color,m_linewidth));
	m_dc->SetBrush(m_brush_transparent);
	m_dc->DrawEllipse(p.x-w, p.y-h, 2*w, 2*h);
}

void mxRenderer::drawHalfElipse (Point p, float w, float h) {
	bool left = w<0; if (left) w=-w;
	transform(p); w*=m_scale; h*=m_scale;
	m_dc->SetPen(wxPen(m_color,m_linewidth));
	m_dc->SetBrush(m_brush_transparent);
	if (left) m_dc->DrawEllipticArc(p.x-w,p.y-h,2*w,2*h,-90,90);
	else      m_dc->DrawEllipticArc(p.x-w,p.y-h,2*w,2*h, 90,270);
}


// copiar de mxSource.cpp
#define UOP_ASIGNACION L'\u2190'
#define UOP_LEQUAL L'\u2264'
#define UOP_GEQUAL L'\u2265'
#define UOP_NEQUAL L'\u2260'
#define UOP_AND L'\u2227'
#define UOP_OR L'\u2228'
#define UOP_NOT L'\u00AC'
#define UOP_POWER L'\u2191'

// copiar de Entity.cpp
static const unsigned char SC_FLECHA = 27;
static const unsigned char SC_DISTINTO = 29;
static const unsigned char SC_MEN_IGUAL = 30;
static const unsigned char SC_MAY_IGUAL = 31;

mxRenderer::Point mxRenderer::drawChar(Point p, char c) {
	wxString s = _S2W(std::string(1,c)); 
	switch (c){
	case SC_FLECHA:    s = wxString(UOP_ASIGNACION,1); break;
	case SC_DISTINTO:  s = wxString(UOP_NEQUAL,1); break;
	case SC_MEN_IGUAL: s = wxString(UOP_LEQUAL,1); break;
	case SC_MAY_IGUAL: s = wxString(UOP_GEQUAL,1); break;
	}
//	wxCoord tw, th;
//	m_dc->GetTextExtent(s,&tw,&th);
	auto p2 = p; transform(p2);
	m_dc->DrawText(s, p2.x, p2.y-m_text_h*0.85);
	p.x += 12;
	return p;
}

mxRenderer::Point mxRenderer::drawText(Point p, const char *str) {
	m_dc->SetTextForeground(m_color);
	while (*str) p = drawChar(p, *(str++));
	return p;
}

mxRenderer::Point mxRenderer::drawTextEx(Point p, float scale, const char *str, int len, const float colors[][3], const char *highlight) {
	m_dc->SetTextForeground(m_color);
	for (int i=0; i<len; ++i) {
		if (highlight) {
			setColor(colors[highlight[i]]);
			m_dc->SetTextForeground(m_color);
		}
		p = drawChar(p, str[i]);
	}
	return p;
}

