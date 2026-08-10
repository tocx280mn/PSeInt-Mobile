#include "EntityEditor.h"
#include "Global.h"
#include "Trash.h"
#include "ShapesBar.h"
#include "Events.h"
#include "Canvas.h"

EntityEditor *g_entity_editor = nullptr;

static int mouse_setted_x,mouse_setted_y; // posicion del click que va a setear el mouse en una entidad cuando se mueva, con correccion de y y zoom aplicados

void EntityEditor::Initialize() {
	g_entity_editor = new EntityEditor;
}

void EntityEditor::ProcessIddle ( ) {
	if (g_state.mouse) {
			// la corrección de m_x es para que cuando la estructura se colapse, el punto
			// de agarre no quede fuera de la misma (ej, un segun con muchas opciones)
			g_state.mouse->m_x -= std::max(g_state.mouse->m_x-g_state.mouse->d_bwr,0);
			g_trash->Show();
			g_shapes_bar->Hide();
		} else {
			g_trash->Hide();
			if (g_state.edit_on) g_shapes_bar->Show();
			else                 g_shapes_bar->Hide();
		}
		g_shapes_bar->ProcessIdle();
}

void EntityEditor::ProcessMotion (int x, int y) {
	fix_mouse_coords(x,y);
	constexpr int MOUSE_SETTED_DELTA = 1000;
	if (to_set_mouse && (x-mouse_setted_x)*(x-mouse_setted_x)+(y-mouse_setted_y)*(y-mouse_setted_y)>MOUSE_SETTED_DELTA) { 
		if (to_set_mouse->type==ET_PROCESO) return; // no permitir mover "proceso" ni "finproceso"
		to_set_mouse->SetMouse(); Entity::CalculateAll();
	}
	if (g_state.selecting_zoom or g_state.selecting_entities) {
		g_state.cur_x = x; g_state.cur_y = y;
		return;
	}
	if (g_state.panning) { 
		g_view.d_dx += x-g_state.m_x0; g_state.m_x0 = x;
		g_view.d_dy += y-g_state.m_y0; g_state.m_y0 = y;
	} 
	if (g_state.mouse) { 
		g_state.cur_y = y; g_state.cur_x = g_state.mouse->d_x;
		g_state.mouse->d_x =x-g_state.mouse->m_x;
		g_state.mouse->d_y =y-g_state.mouse->m_y;
		g_state.mouse->d_fx =x-g_state.mouse->m_x;
		g_state.mouse->d_fy =y-g_state.mouse->m_y;
	}
}

void EntityEditor::ProcessPassiveMotion (int x, int y) {
	if ((not g_view.win_h) or (not g_state.edit_on)) return;
	if (g_state.mouse) {
		g_shapes_bar->Hide();
		return;
	}
	if (g_state.edit_on) {
		g_shapes_bar->ProcessMotion(x,y);
	}
	g_state.cur_x = x; g_state.cur_y = g_view.win_h-y;
	g_canvas->Refresh();
}

static Entity *DuplicateEntity(Entity *orig, bool and_next_one_too = false) {
	Entity *nueva=new Entity(orig->type,orig->label);
	nueva->variante=orig->variante;
	nueva->m_x=orig->m_x; nueva->m_y=orig->m_y;
	nueva->x=orig->x; nueva->y=orig->y;
	nueva->fx=orig->fx; nueva->fy=orig->fy;
	nueva->d_x=orig->d_x; nueva->d_y=orig->d_y;
	nueva->d_fx=orig->d_fx; nueva->d_fy=orig->d_fy;
	for(int i=0;i<orig->GetChildCount();i++) {
		if (orig->GetChild(i)) nueva->LinkChild(i,DuplicateEntity(orig->GetChild(i),true));
		else nueva->LinkChild(i,nullptr);
	}
	if (and_next_one_too && orig->GetNext()) {
		nueva->LinkNext(DuplicateEntity(orig->GetNext(),true));
	}
	return nueva;
}

static void FinishMultipleSelection(int x0, int y0, int x1, int y1) {
	if (x0>x1) { int aux = x0; x0 = x1; x1 = aux; }
	if (y0<y1) { int aux = y0; y0 = y1; y1 = aux; }
	g_state.selecting_entities = false;
	// encontrar la primera que entra en la selección
	Entity *aux = g_code.start;
	do {
		if (aux->IsInside(x0,y0,x1,y1))
			break;
		aux = Entity::NextEntity(aux);
	} while (aux);
	if (!aux) return;
	// crear la entidad de seleccion y poner la primera como primer hija
	Entity *selection = new Entity(ET_SELECTION,""), 
		   *aux_prev = aux->GetPrev(), *aux_parent = aux->GetParent();
	
	selection->d_fx = (x0+x1)/2; selection->d_fy = y0;
	selection->d_h = (y0-y1); selection->d_w = x1-x0;
	
	int aux_chid = aux->GetChildId(); aux->UnLink(); 
	if (aux_prev) aux_prev->LinkNext(selection);
	else aux_parent->LinkChild(aux_chid,selection);
	selection->LinkChild(0,aux);
	// ver cuantas "next" también entran en la selección
	while (selection->GetNext() && selection->GetNext()->IsInside(x0,y0,x1,y1)) {
		Entity *sel_next = selection->GetNext();
		sel_next->UnLink();
		aux->LinkNext(sel_next);
		aux = sel_next;
	}
}

void EntityEditor::ProcessClick (int button, int state, int x, int y) {
	to_set_mouse = nullptr;
	if ((not g_state.panning) and (not g_state.selecting_zoom) and (not g_state.selecting_entities))
		if (g_shapes_bar->ProcessMouse(button,state,x,y)) return;
	fix_mouse_coords(x,y);
	if (button==ZMB_WHEEL_DOWN||button==ZMB_WHEEL_UP) {
		double k = g_canvas->GetModifiers()==MODIFIER_SHIFT ? 1.01 : 1.10;
		double f=button==ZMB_WHEEL_UP?1.0/k:k;
		g_view.zoom *= f;
		double dx = x/f-x, dy = y/f-y;
		g_view.d_dx += dx; g_view.d_dy += dy;
	} else if (state==ZMB_DOWN) {
		if (button==ZMB_MIDDLE) { // click en el menu
			g_state.cur_x = g_state.m_x0 = x; g_state.cur_y = g_state.m_y0 = y; 
			g_state.selecting_zoom = true;
			return;
		}
		// click en una entidad? izquierdo=mover, derecho=editar label
		if (g_state.mouse) g_state.mouse->UnSetMouse();
		Entity::AllIterator it = Entity::AllBegin();
		while (it!=Entity::AllEnd()) {
			if (it->CheckMouse(x,y)) {
				if (it->type==ET_PROCESO && it!=g_code.start) break; // para no editar el "FinProceso"
				if (button==ZMB_RIGHT) {
					it->SetEdit(true,x);
					return;
				} else {
					if (it->type!=ET_PROCESO and g_canvas->GetModifiers()==MODIFIER_SHIFT) { // no duplicar "Proceso..." y "FinProceso"
						g_state.mouse = DuplicateEntity(it.GetPtr());
						it.SetPtr(g_state.mouse); it->SetEdit(true,x);
					} 
					if (it.GetPtr()!=g_state.edit) {
						if (g_state.edit) g_state.edit->UnsetEdit();
					}
					to_set_mouse = it.GetPtr(); mouse_setted_x=x; mouse_setted_y=y; // aux->SetMouse(); retrasado
					if (it->type==ET_AUX_PARA) to_set_mouse=it->GetParent(); // para que no haga drag del hijo del para, sino de todo el para completo
					return;
				}
				break;
			}
			++it;
		}
		if (button==ZMB_LEFT and g_canvas->GetModifiers()==MODIFIER_SHIFT) {
			g_state.cur_x = g_state.m_x0 = x; g_state.cur_y = g_state.m_y0 = y;
			g_state.selecting_entities=true;
		} else {
			g_state.m_x0 = x; g_state.m_y0=y; g_state.panning = true;
		}
	} else { // UP
		if (button==ZMB_LEFT) {
			if (g_state.mouse and g_trash->IsSelected()) {
				Entity *p = nullptr;
				if (g_state.mouse->type==ET_OPCION) {
					p = g_state.mouse->GetParent();
				}
				g_state.mouse->UnLink();
				if (p) p->Calculate(); else Entity::CalculateAll();
			}
			if (g_state.mouse) g_state.mouse->UnSetMouse();
			if (g_state.selecting_entities) {
				FinishMultipleSelection(g_state.m_x0,g_state.m_y0,g_state.cur_x,g_state.cur_y);
			}
//			// doble click (por alguna extraña razon en mi wx un doble click genera un evento de down y dos de up)
//			Entity *aux=g_code.start;
//			do {
//				if (aux->CheckMouse(x,y)) {
//					static int last_click_time=0;
//					static Entity *last_click_mouse=nullptr;
//					int click_time=glutGet(GLUT_ELAPSED_TIME);
//					if (click_time-last_click_time<500 && (last_click_mouse==aux ||  (aux->type==ET_PARA && aux->parent==last_click_mouse)) ) aux->SetEdit();
//					last_click_mouse=aux; last_click_time=click_time;
//				}
//				aux=aux->all_next;
//			} while (aux!=g_code.start);
		} else if (button==ZMB_MIDDLE) {
			ZoomExtend(g_state.m_x0-g_view.d_dx,g_state.m_y0-g_view.d_dy,x-g_view.d_dx,y-g_view.d_dy);
			g_state.selecting_zoom = false;
			return;
		}
		g_state.panning = false;
		Entity::CalculateAll();
	}
}

void EntityEditor::ProcessDoubleClick (int x, int y) {
	fix_mouse_coords(x,y);
	Entity::AllIterator it = Entity::AllBegin();
	while (it!=Entity::AllEnd()) {
		if (it->CheckMouse(x,y)) {
			if (it->type==ET_PROCESO and it!=g_code.start) break; // para no editar el "FinProceso"
			it->SetEdit(true,x);
			return;
		}
		++it;
	}
}

void EntityEditor::ProcessKey (unsigned char key) {
	if (key=='\t') g_shapes_bar->ToggleFixed();
	
	else if (g_state.edit) g_state.edit->EditLabel(key);
	
	else if (key=='-') for (int i=0; i<5; ++i) ProcessClick(ZMB_WHEEL_UP  ,0, g_view.win_w/2, g_view.win_h/2);
	else if (key=='+') for (int i=0; i<5; ++i) ProcessClick(ZMB_WHEEL_DOWN,0, g_view.win_w/2, g_view.win_h/2);
	else if (key==27) Salir();
}

void EntityEditor::ProcessSpecialKey (int key) {
	if      (key==WXK_F2) ProcessMenu(MO_SAVE);
	else if (key==WXK_F3) ProcessMenu(MO_FUNCTIONS);
	else if (key==WXK_F4) ProcessMenu(MO_CLOSE);
	else if (key==WXK_F5) ProcessMenu(MO_DEBUG);
	else if (key==WXK_F9) ProcessMenu(MO_RUN);
	else if (key==WXK_F1) ProcessMenu(MO_HELP);
	else if (key==WXK_F7) { if (not g_state.debugging) ToggleEditable(); }
	else if (key==WXK_F11) ProcessMenu(MO_TOGGLE_FULLSCREEN);
	else if (key==WXK_F12) ProcessMenu(MO_ZOOM_EXTEND);
	
	else if (g_state.edit) g_state.edit->EditSpecialLabel(key);
	
	else if (key==WXK_UP   ) g_view.d_dy -= 100/g_view.zoom;
	else if (key==WXK_DOWN ) g_view.d_dy += 100/g_view.zoom;
	else if (key==WXK_LEFT ) g_view.d_dx += 100/g_view.zoom;
	else if (key==WXK_RIGHT) g_view.d_dx -= 100/g_view.zoom;
}

