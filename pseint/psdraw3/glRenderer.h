#ifndef GL_RENDERER_H
#define GL_RENDERER_H

#define circle_steps 40

class glRenderer {
public:
	glRenderer();
	struct Point { float x, y; };
	glRenderer &setColor(const float *color);
	glRenderer &setWidth(float line_width);
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
	void flush();
	void enableStipple(bool on);
private:
	void beginLines();
	void endLines();
	bool lines_mode = false;
	float cosx[circle_steps+1], sinx[circle_steps+1];
};

using Renderer = glRenderer;
extern glRenderer rndr;

#endif

