#ifndef GLOBAL_H
#define GLOBAL_H
#include <string>
#include <vector>
#include <map>
#include <wx/string.h>
#include <wx/colour.h>
#include "Entity.h"
#include "../pseint/LangSettings.h"

/// @TODO: mucho de esto ahora debería ser atributo de EntityEditor

struct GConfig {
	bool alternative_io = false; ///< utilizar simbolos alternativos para las instrucciones Leer y Escribir
	bool nassi_shneiderman = false; ///< usar diagramas de Nassi-Shneiderman en lugar de "clásico"
	bool syntax_highlight = true; ///< colorear texto (el falso por ahora solo se usa al exportar a imagen bw y siempre con fondo claro)
	bool shape_colors = true; ///< mostrar los bloques de diferentes colores
	bool dark_theme = false; ///< usar una combinación de colores claros sobre fondo oscuro
	bool enable_partial_text = true; ///< acortar labels largos
	bool show_comments = true; ///< mostrar entidades de tipo ET_COMENTARIO
	bool big_icons = false;
};

struct GConstants {
	wxString imgs_path;
	const int menu_line_width = 1;
	int max_label_len[ET_COUNT] = {0};
	int line_width_flechas = 2, line_width_bordes = 1; // anchos de las lines y flechas
};

enum { HL_IDENTIFIER, HL_STRING, HL_NUMBER, HL_KEYWORD, HL_COMMENT, HL_OPERATOR, HL_ERROR, HL_COUNT };
struct GColors {
	float border[ET_COUNT+2][3]; // borde de la forma de una entidad
	float comment[3]; // para borde y fuente de las entidades con comentarios
	float label_high[HL_COUNT][3]; // estilos para el coloreado de sintaxis
	float arrow[3]; // flechas que guian el flujo y unen entidades
	float status[3]; // texto o borde de forma seleccionada
	float shape[ET_COUNT+2][3]; // fondo de la forma
	float back[3]; // fondo de la pantalla
	float ghost[3]; // borde fantasma cuando se arrastra una forma
	float menu_back[3]; // fondo del menu
	float menu_front[3]; // texto del menu
	float menu_front_bold[3]; // texto resaltado del menu
	float menu_sel_back[3]; // fondo de la seleccion del menu
	float menu_sel_front[3]; // texto de la seleccion del menu
	float io_arrow[3]; // mensajes de error de sintaxis
	float error[3]; // mensajes de error de sintaxis
};


struct GState {
	bool big_icons = false;
	
	std::string fname; // archivo que recibe como argumento
	bool edit_on = true; // indica si se puede editar el diagrama
	bool debugging = false; // indica si el programa se esta ejecutando paso a paso (en ese caso no se puede modificar edit_on)
	bool loading = false; // indica si se esta cargando un nuevo algoritmo desde un archivo para desactivar el evento de modificacion
	bool modified = false; // para saber si hay que preguntar antes de salir sin guardar
	
	// estado para pasar entre eventos para la edicion
	Entity *edit = nullptr; // entidad seleccionado para editar su texto
	Entity *mouse = nullptr; // entidad seleccionado por el mouse
	int m_x0 = 0, m_y0  = 0; // click del mouse, para referencia en el motion, se setea en el mouse
	int cur_x = 0, cur_y = 0; // ubicacion del raton (en coord del dibujo)
	
	Entity *debug_current = nullptr;  // la entidad que se esta ejecutando actualmente en el paso a paso
	
	bool panning = false; // indica si se esta moviendo el dibujo, para el motion
	bool selecting_entities = false; // para selecciones múltiples (rectangulares, shift+drag derecho)
	bool selecting_zoom = false; // para hacer zoom en un area marcada, con el boton del medio, m_x0 y m_y0 guardan la primer esquina
};

struct GView {
	// zoom y panning
	int win_h = 0, win_w = 0; // tamaño de la ventana
	double d_dx = 0, d_dy = 0; // "paning" del dibujo
	double d_zoom = 0.1; // zoom del dibujo
	double zoom = 0.1; // zoom final
};

// comunicacion con wxPseInt
extern LangSettings g_lang;

// para interpolar en las animaciones, good converge pixel perfect, la otra puede que no
#ifdef _FOR_EXPORT
	#define interpolate(a,b) a=b
	#define interpolate_good(a,b) a=b
#else
	//#define interpolate(a,b) a=(2*a+b)/3
	#define interpolate(a,b) if ((a)+3>(b) && (a)-3<(b)) a=b; else a=(2*a+b)/3
	#define interpolate_good(a,b) if ((a)+3>(b) && (a)-3<(b)) a=b; else a=(2*a+b)/3
#endif


// para asociar las lineas de código al diagrama de flujo
struct LineInfo { 
	Entity *proceso = nullptr, *entidad = nullptr;
	int variante = 0;
};
struct GCode {
	// para almacenar el proceso principal y los subprocesos
	// la forma de gestionar todo esto es tener los diagramas cargados todos a la
	// vez pero independientes, para lo cual trucheo start y Entity::all_any en 
	// Load y SetProc
	std::vector<Entity*> procesos;
	std::map<std::string,LineInfo> code2draw;
	// auxiliares varios
	Entity *start = nullptr; // entidad donde comienza el algoritmo
	Entity *all_any = nullptr;
	
	/// @TODO: use smart_ptrs
	Entity *entity_to_del = nullptr; // para un delayed delete (cuando suelta uno que sale del shapebar y no queda en ningun lado)
};

enum GMouseCursor {	Z_CURSOR_INHERIT, Z_CURSOR_CROSSHAIR, Z_CURSOR_HAND, Z_CURSOR_TEXT, Z_CURSOR_DESTROY, Z_CURSOR_NONE, Z_CURSOR_MOVE, Z_CURSOR_COUNT };
extern GMouseCursor g_mouse_cursor;

extern GConfig g_config;
extern GConstants g_constants;
extern GColors g_colors;
extern GState g_state;
extern GView g_view;
extern GCode g_code;

void SetColors(); // colores independientes
void SetColors(wxColour toolbar_color, wxColour selection_color); // colores que dependen de wx

void GlobalInitPre(); // llamar antes de parsear la conf que se recibe por linea de comandos
void GlobalInitPost(); // llamar luego de parsear la conf que se recibe por linea de comandos
void GlobalInitGUI(); // llamar luego de los otros 2, para crear las instancias de las partes de la gui (g_status_bar, g_trash, g_process_selector, g_entity_editor, ...)

#endif

