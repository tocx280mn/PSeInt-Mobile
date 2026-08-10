#ifndef WX_RENDERER_H
#define WX_RENDERER_H

#include <wx/dc.h>

#define circle_steps 40

class mxRenderer {
public:
	struct Point { float x, y; };
	
	mxRenderer();
	void setDC(wxDC *target_dc, bool for_svg);
	void setTransformation(Point p_orig, float scale);
	void finish();
	void clear(const float *color);
	
	mxRenderer &setColor(const float *color);
	mxRenderer &setWidth(float line_width);
	void drawQuad(Point p0, Point p1, Point p2, Point p3);
	void drawSolidQuad(Point p0, Point p1, Point p2, Point p3);
	void drawTriangle(Point p0, Point p1, Point p2);
	void drawSolidTriangle(Point p0, Point p1, Point p2);
	void drawRectangle(Point p, float wl, float wr, float h);
	void drawSolidRectangle(Point p, float wl, float wr, float h);
	Point drawText(Point p, const char *str);
	Point drawTextEx(Point p, float scale, const char *str, int len, const float colors[][3]=nullptr, const char *highlight=nullptr);
	void drawLine(Point p0, Point p1);
	void drawSolidCircle(Point p, float r);
	void drawElipse(Point p, float w, float h);
	void drawHalfElipse(Point p, float w, float h);
	void drawSolidElipse(Point p, float w, float h);
	void flush() { } 
	void enableStipple(bool on) { }
private:
	wxDC *m_dc = nullptr;
	int m_linewidth = 0;
	wxColour m_color;
	wxPen m_pen_transparent;
	wxBrush m_brush_transparent;
	wxFont m_font;
	
	bool m_for_svg;
	wxCoord m_text_h;
	void setFontSize();
	Point drawChar(Point p, char c);
	
	Point m_orig;
	float m_scale;
	void transform(Point &p);
};

using Renderer = mxRenderer;
extern mxRenderer rndr;

#endif

