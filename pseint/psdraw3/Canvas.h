#ifndef CANVAS_H
#define CANVAS_H
#include <wx/glcanvas.h>
#include <wx/timer.h>

class MainWindow;

#define MODIFIER_SHIFT 1
#define MODIFIER_ALT 2
#define MODIFIER_CTRL 4

#define MOUSE_RIGHT 1
#define MOUSE_LEFT 2
#define MOUSE_MIDDLE 4

class Canvas : public wxGLCanvas {
private:
	unsigned int mouse_buttons;
	unsigned int modifiers;
	wxGLContext *m_context;
public:
	Canvas(wxWindow *parent);
	~Canvas();
	void OnPaint(wxPaintEvent& event);
	void OnSize(wxSizeEvent& event);
	void OnEraseBackground(wxEraseEvent& event);
	void OnMouseRightDown(wxMouseEvent& event);
	void OnMouseRightUp(wxMouseEvent& event);
	void OnMouseLeftDown(wxMouseEvent& event);
	void OnMouseLeftUp(wxMouseEvent& event);
	void OnMouseMove(wxMouseEvent& event);
	void OnMouseWheel(wxMouseEvent& event);
	void OnMouseMiddleUp(wxMouseEvent& event);
	void OnMouseMiddleDown(wxMouseEvent& event);
	void OnMouseDClick(wxMouseEvent& event);
	void OnIdle(wxIdleEvent &event);
	void OnKeyDown(wxKeyEvent &event);
	void OnKeyUp(wxKeyEvent &event);
	void OnChar(wxKeyEvent &event);
	void SetModifiers(unsigned int mods);
	int GetModifiers() { return modifiers; }
	DECLARE_EVENT_TABLE();
};

extern Canvas *g_canvas;

#endif
