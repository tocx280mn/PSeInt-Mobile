#ifndef EVENTS_H
#define EVENTS_H
#include <cstddef>

enum MENU_OPTIONS { MO_NONE=0,
	MO_ZOOM_EXTEND, MO_TOGGLE_FULLSCREEN, MO_TOGGLE_COLORS,
	MO_CROP_LABELS, MO_TOGGLE_COMMENTS, MO_CHANGE_STYLE, 
	MO_FUNCTIONS, 
	MO_RUN, MO_DEBUG, MO_EXPORT, /*MO_SAVE_CLOSE,*/ MO_CLOSE, MO_HELP, 
	MO_SAVE, MO_UPDATE};

void Salir(bool force=false);

void Raise();

void ZoomExtend(int x0, int y0, int x1, int y1, double max=15);

void ProcessMenu(int op);

void ToggleEditable();

class LineInfo;
void FocusEntity(LineInfo *li);

void SetModified();


enum MOUSE_BUTTONS { ZMB_MIDDLE, ZMB_LEFT, ZMB_RIGHT, ZMB_WHEEL_UP, ZMB_WHEEL_DOWN, ZMB_DOWN, ZMB_UP };

// old glut callbacks
void reshape_cb (int w, int h);
void idle_func();
void passive_motion_cb(int x, int y); // movimiento sin drag
void motion_cb(int x, int y); // drag
void mouse_cb(int button, int state, int x, int y); // down o up de cualquier bonton (incluido wheel)
void mouse_dcb(int x, int y); // doble-click del izquierdo
void keyboard_cb(unsigned char key/*, int x, int y*/);
void keyboard_esp_cb(int key/*, int x, int y*/);

void display_cb();

void fix_mouse_coords(int &x, int &y);

class Entity;
extern Entity *to_set_mouse;
	
#endif

