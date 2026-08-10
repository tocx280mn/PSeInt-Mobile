// comunicacion con wxPseInt
#include <cmath>
#include "Global.h"
#ifndef _FOR_EXPORT
#	include "Trash.h"
#	include "ShapesBar.h"
#	include "ToolBar.h"
#	include "StatusBar.h"
#	include "ProcessSelector.h"
#	include "EntityEditor.h"
#endif

GConfig g_config;
GConstants g_constants;
GColors g_colors;
GState g_state;
GView g_view;
GCode g_code;
GMouseCursor g_mouse_cursor;
#ifndef _FOR_EXPORT
Scene *g_scene = nullptr;
#endif

//LangSettings lang; // no definir aca, ver comentario en el global.cpp de pseint

void GlobalInitPre( ) {
	
	for(int i=0;i<ET_COUNT;i++)
		g_constants.max_label_len[i]=15;
	g_constants.max_label_len[ET_COMENTARIO]=25;
	g_constants.max_label_len[ET_LEER]=25;
	g_constants.max_label_len[ET_PROCESO]=25;
	g_constants.max_label_len[ET_ESCRIBIR]=25;
	g_constants.max_label_len[ET_ASIGNAR]=25;
	g_constants.max_label_len[ET_SI]=15;
	g_constants.max_label_len[ET_SEGUN]=15;
	g_constants.max_label_len[ET_OPCION]=10;
	g_constants.max_label_len[ET_PARA]=15;
	g_constants.max_label_len[ET_MIENTRAS]=15;
	g_constants.max_label_len[ET_REPETIR]=15;
	g_constants.max_label_len[ET_AUX_PARA]=10;
	
	SetColors();
}

void GlobalInitPost() {
	g_view.win_h = g_config.big_icons ? 800 : 600;
	g_view.win_w = g_config.big_icons ? 1200 : 800; // tamaño de la ventana
	g_constants.imgs_path = g_config.big_icons ? "imgs/flow/32/" : "imgs/flow/24/";
}


static void SetColor(float *c, float r, float g, float b) { 
	c[0] = r; c[1] = g; c[2] = b; 
}

static void SetColor(float *c, const char s[]) {
	static char from_hexa[256] = {0};
	if (from_hexa[0]==0) {
		from_hexa[0] = 1;
		for (int i=0; i<9; ++i) from_hexa['0'+i] = 0+i;
		for (int i=0; i<6; ++i) from_hexa['A'+i] = 10+i;
		for (int i=0; i<6; ++i) from_hexa['a'+i] = 10+i;
	}
	c[0] = (from_hexa[s[1]]*16 + from_hexa[s[2]])/255.f;
	c[1] = (from_hexa[s[3]]*16 + from_hexa[s[4]])/255.f;
	c[2] = (from_hexa[s[5]]*16 + from_hexa[s[6]])/255.f;
}

static void SetColor(float *c, const float *s) {
	SetColor(c,s[0],s[1],s[2]);
}

void SetColors( ) {

	SetColor(g_colors.border[ET_COUNT+1], g_config.dark_theme ? "#FAFAFA" : "#000000");
	SetColor(g_colors.comment, g_config.dark_theme ? "#999999" : "#969696");
	
	// coloreado de sintaxis.. el falso por ahora solo se usa al exportar a imagen bw y siempre con fondo claro
	SetColor(g_colors.label_high[HL_IDENTIFIER], g_config.syntax_highlight ? (g_config.dark_theme ? "#FAFAFA" : "#000000") : "#000000");
	SetColor(g_colors.label_high[HL_NUMBER],     g_config.syntax_highlight ? (g_config.dark_theme ? "#FAFA99" : "#A0522D") : "#000000" );
	SetColor(g_colors.label_high[HL_STRING],     g_config.syntax_highlight ? (g_config.dark_theme ? "#A9FAA9" : "#006400") : "#000000" );
	SetColor(g_colors.label_high[HL_KEYWORD],    g_config.syntax_highlight ? (g_config.dark_theme ? "#A9A9FA" : "#000080") : "#000000" );
	SetColor(g_colors.label_high[HL_COMMENT],    g_config.syntax_highlight ? (g_config.dark_theme ? "#22AA22" : "#5050FF") : "#000000" );
	SetColor(g_colors.label_high[HL_OPERATOR],   g_config.syntax_highlight ? (g_config.dark_theme ? "#A9A9FA" : "#000080") : "#000000" );
	SetColor(g_colors.label_high[HL_ERROR],      g_config.syntax_highlight ? (g_config.dark_theme ? "#FA9999" : "#800000") : "#000000" );
	
	SetColor(g_colors.arrow,    g_config.dark_theme ? "#FF8888" : "#CC0000");
	SetColor(g_colors.status,   g_config.dark_theme ? "#88FF88" : "#006600");
	SetColor(g_colors.io_arrow, g_config.dark_theme ? "#A9A9FA" : "#000080");
	SetColor(g_colors.ghost, "#B2B2B2");
	SetColor(g_colors.error,"#B20000");
	SetColor(g_colors.back, g_config.shape_colors ? (g_config.dark_theme ? "#333333" : "#FAFAFA") : "#FFFFFF");
	
	for(int i=0;i<ET_COUNT;i++)
		SetColor(g_colors.shape[i], "#FFF2CC");
	SetColor(g_colors.shape[ET_COUNT], "#F2F2F2");
	SetColor(g_colors.shape[ET_LEER], "#FFE5F2");
	SetColor(g_colors.shape[ET_ESCRIBIR], "#E5FFE5");
	SetColor(g_colors.shape[ET_ASIGNAR], "#FFFFD8");
	
	SetColor(g_colors.shape[ET_SI], "#D8E5FF");
	SetColor(g_colors.shape[ET_SEGUN], g_colors.shape[ET_SI]);
	SetColor(g_colors.shape[ET_OPCION], g_colors.shape[ET_SI]);
	
	SetColor(g_colors.shape[ET_PARA], "#F2CCFF");
	SetColor(g_colors.shape[ET_MIENTRAS], g_colors.shape[ET_PARA]);
	SetColor(g_colors.shape[ET_REPETIR], "#CCB2FF");
	SetColor(g_colors.shape[ET_COMENTARIO], g_config.dark_theme?"#000000":"#FFFFFF");
	
	// colores de borde a partir de los colores de relleno
	if (g_config.dark_theme) {
		for(int i=0;i<ET_COUNT;i++) { 
			for(int j=0;j<3;j++) {
				g_colors.border[i][j] = g_colors.shape[i][j] = pow(g_colors.shape[i][j],4);
				g_colors.shape[i][j] = g_colors.shape[i][j]*.1 + g_colors.back[j]*.9;
			}
		}
	} else {
		for(int i=0;i<ET_COUNT;i++) { 
			for(int j=0;j<3;j++) 
				g_colors.border[i][j] = pow(g_colors.shape[i][j],7)*.75;
		}
	}
}

void SetColors(wxColour toolbar_color, wxColour selection_color) {
	g_colors.menu_back[0] = toolbar_color.Red()/255.f;
	g_colors.menu_back[1] = toolbar_color.Green()/255.f;
	g_colors.menu_back[2] = toolbar_color.Blue()/255.f;
	g_colors.menu_sel_back[0] = selection_color.Red()/255.f;
	g_colors.menu_sel_back[1] = selection_color.Green()/255.f;
	g_colors.menu_sel_back[2] = selection_color.Blue()/255.f;
	for(int k=0;k<3;++k) {
		g_colors.menu_sel_front[k] = 1-g_colors.menu_sel_back[k];
		g_colors.menu_front_bold[k] = 1-g_colors.menu_back[k];
		g_colors.menu_front[k] = (7*g_colors.menu_front_bold[k] + 3*g_colors.menu_back[k])/10;
	}
}

void GlobalInitGUI() {
#ifndef _FOR_EXPORT
	Trash::Initialize();
	ToolBar::Initialize();
	StatusBar::Initialize();
	ShapesBar::Initialize();
	ProcessSelector::Initialize();
	EntityEditor::Initialize();
	g_entity_editor->MakeCurrent();
#endif
}
