#include <cstddef>
#include "ToolBar.h"
#include "Events.h"
#include "Global.h"
#include "StatusBar.h"

ToolBar *g_toolbar = nullptr;

void ToolBar::SetSelection (int i) {
	m_selection = i;
}

void ToolBar::Draw ( ) {
	if (m_selection==MO_NONE) return;
	// the actual drawing of the toolbar is done by wxToolBar
	switch(m_selection) {
		case MO_ZOOM_EXTEND:       g_status_bar->Set("Ajustar el zoom para visualizar todo el diagrama."); break;
		case MO_TOGGLE_FULLSCREEN: g_status_bar->Set("Alternar entre modo ventana y pantalla completa."); break;
		case MO_TOGGLE_COLORS:     g_status_bar->Set("Cambiar tema de colores."); break;
		case MO_CROP_LABELS:       g_status_bar->Set("Acortar textos muy largos en etiquetas."); break;
		case MO_TOGGLE_COMMENTS:   g_status_bar->Set("Mostrar/Ocultar comentarios."); break;
		case MO_CHANGE_STYLE:      g_status_bar->Set("Cambiar el tipo de diagrama (clásico, o Nassi-Shneiderman)."); break;
		case MO_RUN:               g_status_bar->Set("Ejecuta el algoritmo en la terminal de PSeInt."); break;
		case MO_DEBUG:             g_status_bar->Set("Ejecuta el algoritmo paso a paso marcando los pasos sobre el diagrama."); break;
		case MO_EXPORT:            g_status_bar->Set("Permite guardar el diagrama como imagen."); break;
		case MO_CLOSE:             g_status_bar->Set("Cierra el editor, preguntando antes si se deben aplicar los cambios en el pseudocódigo"); break;
		case MO_HELP:              g_status_bar->Set("Muestra una ventana de ayuda que explica cómo utilizar este editor y cuáles son sus atajos de teclado."); break;
		case MO_FUNCTIONS:
			g_status_bar->Set(
					  g_lang[LS_PREFER_FUNCION] ? "Permite elegir cual función editar, crear una nueva o eliminar una existente."
					  : (g_lang[LS_PREFER_ALGORITMO] ? "Permite elegir cual subalgoritmo editar, crear uno nuevo o eliminar uno existente."
						 : "Permite elegir cual subproceso editar, crear uno nuevo o eliminar uno existente.") ); 
			break;
		default:
			;
	}
}

void ToolBar::Initialize ( ) {
	g_toolbar = new ToolBar();
}

