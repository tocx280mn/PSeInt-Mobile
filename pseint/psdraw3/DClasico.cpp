#include <iostream>
#include <cstdlib>
#include <cmath>
#include "Entity.h"
#include "Entity.h"
#include "Global.h"
#include "Renderer.h"

static const int vf_size=5; // tamaño de los letras V/F en las estructuras de control?

inline void DrawLineaHorizontalTo(int x0, int y0, int x1) { rndr.drawLine({x0,y0}, {x1,y0}); }
inline void DrawLineaHorizontalW(int x, int y, int w) { rndr.drawLine({x,y}, {x+w,y}); }
inline void DrawLineaVerticalTo(int x0, int y0, int y1) { rndr.drawLine({x0,y0}, {x0,y1}); }
inline void DrawLineaVerticalH(int x, int y, int h) { rndr.drawLine({x,y}, {x,y+h}); }
inline void DrawLinea(int x0, int y0, int x1, int y1) { rndr.drawLine({x0,y0}, {x1,y1}); }

void Entity::DrawShapeSolid(const float *color,int x, int y, int w, int h) {
	rndr.setColor(color);
	if (type==ET_PARA) {
		rndr.drawSolidElipse({x,y-h/2}, w/2, h/2);
	} else if (type==ET_PROCESO) {
		float waux = w/2-h;
		rndr.drawSolidRectangle({x,y}, waux, waux, h);
		rndr.drawSolidCircle({x-waux,y-h/2}, h/2);
		rndr.drawSolidCircle({x+waux,y-h/2}, h/2);
	} else if (type==ET_REPETIR||type==ET_MIENTRAS||type==ET_SI) {
		rndr.drawSolidQuad({x,y}, {x+w/2,y-h/2}, {x,y-h}, {x-w/2,y-h/2});
	} else if (type==ET_ESCRIBIR||type==ET_LEER) {
		if (g_config.alternative_io) {
			if (type==ET_LEER) {
				rndr.drawSolidQuad({x-w/2,y-margin}, {x+w/2,y+margin}, {x+w/2,y-h}, {x-w/2,y-h});
			} else {
				float waux = w/2-h;
				rndr.drawSolidRectangle({x,y}, waux+margin, waux, h);
				float r=3*h/4;
				rndr.drawSolidElipse({x+waux,y-h/2}, r, h/2);
				rndr.drawSolidTriangle({x-waux-r,y-h/2}, {x-waux-margin,y}, {x-waux-margin,y-h});
			}
		} else {
			rndr.drawSolidQuad({x+w/2+margin,y}, {x-w/2+margin,y}, {x-w/2-margin,y-h}, {x+w/2-margin,y-h});
		}
	} else if (type==ET_SEGUN) {
		rndr.drawSolidTriangle({x,y}, {x+w/2,y-h}, {x-w/2,y-h});
	} else {
		rndr.drawSolidRectangle({x,y}, w/2, w/2, h);
	}
	rndr.flush();
}


void Entity::DrawShapeBorder(const float *color,int x, int y, int w, int h) {
	rndr.setWidth(g_constants.line_width_bordes);
	rndr.setColor(color);
	if (type==ET_PARA) {
		rndr.drawElipse({x,y-h/2}, w/2, h/2);
	} else if (type==ET_PROCESO) {
		float waux = w/2-h;
		rndr.drawLine({x-waux,y}, {x+waux,y});
		rndr.drawLine({x-waux,y-h}, {x+waux,y-h});
		rndr.drawHalfElipse({x-waux,y-h/2}, +h/2, h/2);
		rndr.drawHalfElipse({x+waux,y-h/2}, -h/2, h/2);
	} else if (type==ET_REPETIR||type==ET_MIENTRAS||type==ET_SI) {
		rndr.drawQuad({x,y}, {x+w/2,y-h/2}, {x,y-h}, {x-w/2,y-h/2});
	} else if (type==ET_ESCRIBIR||type==ET_LEER) {
		if (g_config.alternative_io) {
			if (type==ET_LEER) {
				rndr.drawQuad({x-w/2,y-margin}, {x+w/2,y+margin}, {x+w/2,y-h}, {x-w/2,y-h});
			} else {
				float waux = w/2-h;
				rndr.drawLine({x-waux-margin,y}, {x+waux,y});
				rndr.drawLine({x-waux-margin,y-h}, {x+waux,y-h});
				float r=3*h/4;
				rndr.drawHalfElipse({x+waux,y-h/2}, -r, h/2);
				rndr.drawLine({x-waux-r,y-h/2}, {x-waux-margin,y});
				rndr.drawLine({x-waux-r,y-h/2}, {x-waux-margin,y-h});
			}
		} else {
			rndr.drawQuad({x+w/2+margin,y}, {x-w/2+margin,y}, {x-w/2-margin,y-h}, {x+w/2-margin,y-h});
		}
	} else if (type==ET_SEGUN) {
		rndr.drawTriangle({x,y}, {x+w/2,y-h}, {x-w/2,y-h});
	} else if (type==ET_COMENTARIO) {
		if (variante) {
			rndr.drawLine({x-w/2,y-h},{x+w/2,y-h});
			rndr.drawLine({x-w/2,y-h},{x-w/2,y-h+2*margin});
			rndr.drawLine({x+w/2,y-h},{x+w/2,y-h+2*margin});
		} else {
			rndr.drawLine({x-w/2,y},{x-w/2+3*margin,y});
			rndr.drawLine({x-w/2,y},{x-w/2,y-h});
			rndr.drawLine({x-w/2,y-h},{x-w/2+3*margin,y-h});
		}
	} else {
		rndr.drawRectangle({x,y}, w/2, w/2, h);
		if (g_state.edit_on and type==ET_OPCION and g_state.mouse!=this) {
			rndr.drawLine({x-w/2+flecha_w,y}, {x-w/2+flecha_w,y-h});
		} else if (type==ET_ASIGNAR and variante) {
			rndr.drawLine({x-w/2+margin,y}, {x-w/2+margin,y-h});
			rndr.drawLine({x+w/2-margin,y-h}, {x+w/2-margin,y});
		}
	}
	rndr.flush();
	rndr.setWidth(g_constants.line_width_flechas);
}

inline void DrawTrue(int x, int y) { // V
	rndr.drawLine({x,y}, {x-margin/2,y+2*vf_size});
	rndr.drawLine({x,y}, {x+margin/2,y+2*vf_size});
}

inline void DrawFalse(int x, int y) { // F
	rndr.drawLine({x-vf_size/2,y}, {x-vf_size/2,y+2*vf_size});
	rndr.drawLine({x-vf_size/2,y+2*vf_size}, {x+vf_size/2,y+2*vf_size});
	rndr.drawLine({x-vf_size/2,y+vf_size}, {x+vf_size/2,y+vf_size});
}

inline void DrawFlechaDownHead(int x, int y2) {
	rndr.drawSolidTriangle({x,y2}, {x-flecha_d,y2+flecha_d}, {x+flecha_d,y2+flecha_d});
}

inline void DrawFlechaDown(int x, int y1, int y2) {
	rndr.drawLine({x,y1}, {x,y2});
	DrawFlechaDownHead(x,y2);
}

inline void DrawFlechaUp(int x, int y1, int y2) {
	rndr.drawLine({x,y1}, {x,y2});
	rndr.drawSolidTriangle({x,y2}, {x-flecha_d,y2-flecha_d}, {x+flecha_d,y2-flecha_d});
}

inline void DrawFlechaR(int x1, int x2, int y) {
	rndr.drawLine({x1,y}, {x2,y});
	rndr.drawSolidTriangle({x2-flecha_d,y-flecha_d}, {x2-flecha_d,y+flecha_d}, {x2,y});
}

inline void DrawFlechaL(int x1, int x2, int y) {
	rndr.drawLine({x1,y}, {x2,y});
	rndr.drawSolidTriangle({x2+flecha_d,y-flecha_d}, {x2+flecha_d,y+flecha_d}, {x2,y});
}

void Entity::DrawClasico(bool force) {
	if (!force && (type==ET_OPCION || type==ET_AUX_PARA)) return;
	if (this==g_state.mouse and (GetPrev() or GetParent())) { // si se esta moviendo con el mouse, dibujar un ghost donde lo agregariamos al soltar
		int gx = g_view.d_dx+x, gy = g_view.d_dy+y, gw = bwr+bwl;
		if (type==ET_COMENTARIO) {
			if (variante) gx -= w/2+7*margin;
			else gw = -10*margin;
		}
		DrawShapeBorder(g_colors.ghost, gx, gy, gw, h);
	}
	rndr.setColor(g_colors.arrow);
	if (!nolink) {
		if (type==ET_OPCION) {
			DrawLineaVerticalH((GetChild(0)?GetChild(0)->d_x:d_x),d_y-d_h,-flecha_h); 
		} else if (type==ET_SEGUN) {
			for(int i=0;i<GetChildCount();i++) {
				if (!GetChild(i)->GetChild(0)) {
					DrawFlechaDownHead(d_x+child_dx[i],d_y-d_h-GetChild(i)->bh);
					DrawFlechaDown(d_x+child_dx[i],d_y-d_h-child_bh[i],d_y-d_bh+flecha_h); 
				}
				else if (GetChild(i)!=g_state.mouse) {
					DrawFlechaDown(GetChild(i)->GetChild(0)->d_x,d_y-d_h-child_bh[i],d_y-d_bh+flecha_h); 
					DrawFlechaDownHead(GetChild(i)->GetChild(0)->d_x,GetChild(i)->GetChild(0)->d_y-GetChild(i)->GetChild(0)->d_bh); 
				}
			}
			// linea horizontal de abajo
			DrawLineaHorizontalW(d_x,d_y-d_bh+flecha_h,child_dx[0]+(GetChild(0)?GetChild(0)->child_dx[0]:0));
			DrawLineaHorizontalW(d_x,d_y-d_bh+flecha_h,child_dx[GetChildCount()-1]+(GetChild(GetChildCount()-1)?GetChild(GetChildCount()-1)->child_dx[0]:0));
		} else if (type==ET_MIENTRAS) {
			DrawTrue(d_fx+2*vf_size,d_fy-d_h-5*vf_size/2);
			DrawFalse(d_fx+d_w/2+2*vf_size,d_fy-d_h/2+vf_size);
			DrawLineaVerticalH(d_x,d_y,-flecha_in); // flecha que entra
			DrawLineaVerticalTo(d_x,d_y-d_bh+3*flecha_h,d_y-d_bh+2*flecha_h); // sale de la ultima instruccion
			DrawFlechaL(d_x,d_x-d_bwl,d_y-d_bh+2*flecha_h); // sigue a la izquierda
			DrawFlechaUp(d_x-d_bwl,d_y-d_bh+2*flecha_h,d_y); // sube
			DrawFlechaR(d_x-d_bwl,d_x,d_y); // entra arriba de la condicion
			DrawLineaHorizontalW(d_x,d_y-flecha_h-d_h/2,d_bwr); // sale de la condicion para la derecha
			DrawFlechaDown(d_x+d_bwr,d_y-flecha_h-d_h/2,d_y-d_bh+flecha_h); // baja
			DrawLineaHorizontalW(d_x,d_y-d_bh+flecha_h,d_bwr); // va al punto de salida
			DrawLineaVerticalH(d_fx,d_fy-d_h,-flecha_h);
		} else if (type==ET_PARA) {
			DrawLineaVerticalH(d_x,d_y,-flecha_in); // flecha que entra del bloque
			DrawLineaVerticalTo(d_x,d_y-child_bh[0]-flecha_h,d_y-d_bh+flecha_h); // flecha que sale del bloque
			DrawLineaVerticalTo(d_fx,d_y,d_fy); // flecha que sale del circulo
			DrawFlechaR(d_fx,(d_fx+d_x)/2,d_y); 
			DrawLineaHorizontalTo((d_fx+d_x)/2,d_y,d_x); 
			DrawLineaVerticalTo(d_fx,d_y,d_fy); // flecha que sale del circulo
			DrawFlechaUp(d_fx,d_y-d_bh+flecha_h,d_fy-d_h); // flecha que entra al circulo
			DrawLineaHorizontalTo(d_fx,d_y-d_bh+flecha_h,d_x);
		} else if (type==ET_REPETIR) {
			DrawFlechaDownHead(d_fx,d_fy);
			if (variante) {
				DrawFalse(d_fx+2*vf_size,d_fy-d_h-5*vf_size/2);
				DrawTrue(d_fx-d_w/2-2*vf_size,d_fy-d_h/2+vf_size);
			} else {
				DrawTrue(d_fx+2*vf_size,d_fy-d_h-5*vf_size/2);
				DrawFalse(d_fx-d_w/2-2*vf_size,d_fy-d_h/2+vf_size);
			}
			DrawLineaVerticalH(d_x,d_y,-flecha_in); // flecha que entra
			DrawFlechaL(d_x,d_x-d_bwl,d_fy-d_h/2); // sigue a la izquierda
			DrawFlechaUp(d_x-d_bwl,d_fy-d_h/2,d_y); // sube
			DrawFlechaR(d_x-d_bwl,d_x,d_y); // entra arriba de la condicion
			DrawLineaVerticalH(d_fx,d_fy,flecha_h); // flecha a la siguiente instruccion
		} else if (type==ET_SI) {
			DrawTrue(d_fx+d_w/2+2*vf_size,d_fy-d_h/2+vf_size);
			DrawFalse(d_fx-d_w/2-2*vf_size,d_fy-d_h/2+vf_size);
			// linea horizontal de arriba
			DrawLineaHorizontalTo(d_x+child_dx[0],d_y-d_h/2,d_x+child_dx[1]);
			// flechas que bajan por el verdadero
			if (GetChild(0)) DrawLineaVerticalTo(d_x+child_dx[0],d_y-d_h/2,d_y-d_h-flecha_h);
			else DrawFlechaDown(d_x+child_dx[0],d_y-d_h/2,d_y-d_h-flecha_h); 
			DrawFlechaDown(d_x+child_dx[0],d_y-d_h-child_bh[0]-flecha_h,d_y-d_bh+flecha_h); 
			// flechas que bajan por el falso
			if (GetChild(1)) DrawLineaVerticalTo(d_x+child_dx[1],d_y-d_h/2,d_y-d_h-flecha_h);
			else DrawFlechaDown(d_x+child_dx[1],d_y-d_h/2,d_y-d_h-flecha_h); 
			DrawFlechaDown(d_x+child_dx[1],d_y-d_h-child_bh[1]-flecha_h,d_y-d_bh+flecha_h); 
			// linea horizontal de abajo
			DrawLineaHorizontalTo(d_x+child_dx[0],d_y-d_bh+flecha_h,d_x+child_dx[1]);
		} else if (type==ET_COMENTARIO) {
			// linea de flecha que va al siguiente
			if (GetParent()||GetPrev()||GetNext()) {
				Entity *next_nc = GetNextNoComment();
				bool fuera_de_proceso = IsOutOfProcess(next_nc);
				if (!fuera_de_proceso) {
					DrawLineaVerticalH(d_x,d_y,-d_bh); // continuacion del flujo
				}
				if (variante || !fuera_de_proceso) {
					// linea punteada desde el flujo o desde la siguiente entidad hacia el comentario
					rndr.enableStipple(true);
					rndr.setWidth(g_constants.line_width_bordes);
					rndr.setColor(g_colors.comment);
					if (variante) { // apunta al siguiente no comentario
						if (next_nc) DrawLinea(next_nc->d_fx-5*margin,next_nc->d_fy,d_x-d_bwl/2,d_y-d_h);
					} else {
						DrawLineaHorizontalW(d_x+margin,d_y-d_h/2,4*margin); 
					}
					rndr.setColor(g_colors.arrow);
					rndr.enableStipple(false);
					rndr.setWidth(g_constants.line_width_flechas);
				}
			}
		}
		if (type!=ET_OPCION && type!=ET_COMENTARIO && type!=ET_SELECTION) {
			// punta de flecha que viene del anterior
			if (!(type==ET_PROCESO&&!variante)) DrawFlechaDownHead(d_x,d_y-flecha_in); // no en inicio
			// linea de flecha que va al siguiente
			if (!(type==ET_PROCESO&&variante)) DrawLineaVerticalH(d_x,d_y-d_bh,flecha_h);
		}
	} else if (g_state.mouse==this and (GetNext() or GetParent())) {
		// flecha que va al siguiente item cuando este esta flotando
		DrawLineaVerticalH(g_view.d_dx+x,g_view.d_dy+y-bh,flecha_h);
		if (type!=ET_OPCION) DrawFlechaDownHead(g_view.d_dx+x,g_view.d_dy+y); // no en inicio
	}
	if (type==ET_COMENTARIO or type==ET_SELECTION) {
		rndr.enableStipple(true);
		// borde de la forma
		DrawShapeBorder(/*mouse==this?color_selection:*/g_colors.comment,d_fx,d_fy,d_w,d_h);
		rndr.enableStipple(false);
	} else {
		// relleno de la forma
		int icolor = g_config.shape_colors?type:ET_COUNT;
		DrawShapeSolid(g_colors.shape[icolor],d_fx,d_fy,d_w,d_h);
		// borde de la forma
		DrawShapeBorder(/*mouse==this?color_selection:*/(g_colors.border[icolor]),d_fx,d_fy,d_w,d_h);
	}
	if (type==ET_OPCION) { // + para agregar opciones
		if (g_state.edit_on and g_state.mouse!=this) {
			rndr.setColor(g_colors.label_high[3]);
			DrawLineaHorizontalTo(d_x-d_bwl+3*flecha_w/4,d_y-d_h/2,d_x-d_bwl+1*flecha_w/4);
			DrawLineaVerticalTo(d_x-d_bwl+flecha_w/2,d_y-1*d_h/3,d_y-2*d_h/3);
		}
	} else 
	if (!nolink) {
		if (type==ET_SELECTION && !nolink) {
			rndr.setColor(g_colors.back);
			int w = margin, x = d_fx+d_w/2, y = d_fy/*-d_h*/;
			rndr.drawSolidQuad({x-w,y-w}, {x-w,y+w}, {x+w,y+w}, {x+w,y-w});
			rndr.setWidth(g_constants.line_width_bordes);
			rndr.setColor(g_colors.menu_front);
			rndr.drawQuad({x-w,y-w}, {x-w,y+w}, {x+w,y+w}, {x+w,y-w});
			w /= 2;
			rndr.setColor(g_colors.arrow);
			DrawLinea(x-w,y-w,x+w,y+w);
			DrawLinea(x+w,y-w,x-w,y+w);
			rndr.setWidth(g_constants.line_width_flechas);
		} else
		if (type==ET_ESCRIBIR or type==ET_LEER) { // flecha en la esquina
			rndr.setColor(g_colors.io_arrow);
			int axl = d_x+d_w/2-margin, axe = d_x+d_w/2+margin;
			if (g_config.alternative_io and type==ET_ESCRIBIR) { axl-=h; axe-=h; }
			DrawLinea(axl,d_y-margin,axe,d_y+margin);
			if (type==ET_LEER) {
				DrawLinea(axl,d_y-margin,axl+margin,d_y-margin);
				DrawLinea(axl,d_y-margin,axl,d_y-margin+margin);
			} else {
				DrawLinea(axe,d_y+margin,axe-margin,d_y+margin);
				DrawLinea(axe,d_y+margin,axe,d_y+margin-margin);
			}
		}
	}
	// texto;
	DrawText();
	if (!nolink) {
		if (type==ET_SEGUN) {
			for(int i=0;i<GetChildCount();i++) { 
				GetChild(i)->Draw(true);
			}
		} else if (type==ET_PARA) {
			rndr.setColor(g_colors.border[g_config.shape_colors?ET_PARA:ET_COUNT]);
			DrawLinea(d_x-2*margin,d_y-d_bh+flecha_h-margin,d_x+2*margin,d_y-d_bh+flecha_h+margin);
			DrawLinea(d_x-2*margin,d_y-margin,d_x+2*margin,d_y+margin);
			rndr.setWidth(g_constants.line_width_bordes);
			DrawLineaHorizontalW(d_fx-w/2,d_fy-d_h/2,w); // separadores de las cuatro partes del circulo
			if (!variante) {
				if (g_state.edit_on or GetChild(2)->label.size()) {
					DrawLineaVerticalTo(d_x+child_dx[1],d_fy-d_h/2,d_fy-d_h+margin);
					DrawLineaVerticalTo(d_x+child_dx[2],d_fy-d_h/2,d_fy-d_h+margin);
				} else {
					DrawLineaVerticalTo(d_x+(child_dx[1]+child_dx[2])/2,d_fy-d_h/2,d_fy-d_h+margin);
				}
				GetChild(1)->DrawText();
				GetChild(2)->DrawText();
				GetChild(3)->DrawText();
			} else {
				GetChild(2)->DrawText();
			}
			rndr.setWidth(g_constants.line_width_flechas);
		}
	}
}

void Entity::CalculateClasico() { // calcula lo propio y manda a calcular al siguiente y a sus hijos, y acumula en gw,gh el tamano de este item (para armar el tamano del bloque)
	
	if (type==ET_SELECTION) {
		if (!GetChild(0)) return;
		GetChild(0)->x = x; GetChild(0)->y = y;
		bwl = bwr = bh = 0;
		GetChild(0)->Calculate(bwl,bwr,bh);
		bwl+=2*margin; bwr+=2*margin;
		h = bh; w = bwl+bwr; t_dy=t_dx=0; fx=x+(bwr-bwl)/2; fy=y+flecha_h/2;
		child_dx[0] = 0; child_bh[0] = bh;
		return;
	}
	
	// calcular tamanos de la forma segun el texto
	h=t_h+2*margin; if (!t_w) w=margin*6; else { w=t_w; if (type!=ET_PROCESO) w+=2*margin; else w+=2*(h-margin); }
	if (type==ET_REPETIR||type==ET_MIENTRAS||type==ET_SI) {
		w*=2; h*=2;
	} else if (type==ET_ESCRIBIR||type==ET_LEER) {
		if (g_config.alternative_io) { if (type==ET_ESCRIBIR) { w+=2*(h-margin); }
		} else { w+=2*margin; }
	} else if (type==ET_ASIGNAR && variante) {
		w+=2*margin;
	} else if (type==ET_PARA) {
		h=2*h+3*margin; w=1.3*w+2*margin;
	} else if (type==ET_SEGUN) {
		h*=2;
	}
	
	t_dy=t_dx=0; fx=x; fy=y; bh=h+flecha_h; bwr=bwl=w/2; // esto es si fuera solo la forma
	if (g_config.alternative_io) {
		if (type==ET_LEER) { bh+=margin; h+=margin; t_dy-=margin/2; }
	}
	if (type==ET_COMENTARIO) { 
		if (not g_config.show_comments) { bwl=bwr=bh=0; return; }
		if (variante || (GetNext() && GetNext()->type==ET_COMENTARIO)) bh-=flecha_h/2;
		if (variante) {
			fx-=w/2+5*margin; bwr=0; bwl=w+5*margin;
		} else {
			fx+=w/2+5*margin; bwl=0; bwr=w+5*margin;
		}
	}
	
	// si son estructuras de control, es un viaje
	if (!nolink && GetChildCount()) {
		if (type==ET_OPCION) {
			bwr=bwl=(w=t_w+2*margin)/2;
			if (g_state.edit_on) 
			{ bwr+=flecha_w; bwl+=flecha_w; } // el + para agregar opciones
			child_dx[0]=0; child_bh[0]=0;
			if (GetChild(0)) {
				GetChild(0)->x=x; GetChild(0)->y=y-bh;
				int cwl=0,cwr=0,ch=0;
				GetChild(0)->Calculate(cwl,cwr,ch);
				bh+=ch; 
				if (cwl+cwr>bwl+bwr) bwl=bwr=(cwl+cwr)/2;
				bwl+=flecha_w/2;
				bwr+=flecha_w/2;
				child_dx[0]-=(cwr-cwl)/2;
				GetChild(0)->MoveX(child_dx[0]);
			}
			// el ancho se lo define el segun padre
			w=bwl+bwr;
		} else if (type==ET_SEGUN) {
			t_dy=-t_h/2-margin;
			bwr=bwl=(w=t_w*2)/2;
//				w=bwr=bwl=0; // todo: ver como corregir esto
			int sw=0, sh=0;
			for (int i=0;i<GetChildCount();i++) {
				int cwl=0,cwr=0,ch=0;
				GetChild(i)->x=0; GetChild(i)->y=y-h;
				GetChild(i)->Calculate(cwl,cwr,ch);
				child_bh[i]=ch; child_dx[i]=sw+cwl;
				sw+=cwl+cwr-2;
				if (ch>sh) sh=ch;
			}
			if (sw>w) w=sw; 
			bwr=bwl=w/2; 
			bh+=sh;
			for (int i=0;i<GetChildCount();i++) {
				child_dx[i]-=w/2;
				GetChild(i)->MoveX(child_dx[i]);
			}
			Entity *dom=GetChild(GetChildCount()-1);
			if (dom->x+dom->bwr<x+bwr) {
				int dif=(x+bwr)-(dom->x+dom->bwr);
				dom->bwl+= dif/2;
				dom->bwr+= dif/2;
				dom->w+= dif;
				dom->MoveX(dif/2);
				child_dx[GetChildCount()-1]+=dif/2;
			}
		} if (type==ET_SI) {
			int c1l=0,c1r=0,c1h=0;
			if (GetChild(0)) {
				GetChild(0)->y=y-bh; GetChild(0)->x=x;
				GetChild(0)->Calculate(c1l,c1r,c1h);
			} 
			int c2l=0,c2r=0,c2h=0;
			if (GetChild(1)) {
				GetChild(1)->y=y-bh; GetChild(1)->x=x;
				GetChild(1)->Calculate(c2l,c2r,c2h);
			} 
			bh += (c1h>c2h?c1h:c2h) + flecha_h;
			child_bh[0]=c1h; child_bh[1]=c2h;
			child_dx[0] = -flecha_w-(c1r>bwl?c1r:bwl);
			child_dx[1] =  flecha_w+(c2l>bwr?c2l:bwr);
			if (GetChild(0)) GetChild(0)->MoveX(child_dx[0]);
			if (GetChild(1)) GetChild(1)->MoveX(child_dx[1]);
			if (c1l-child_dx[0]>bwl) bwl=c1l-child_dx[0];
			if (c2r+child_dx[1]>bwr) bwr=c2r+child_dx[1];
		} else if (type==ET_MIENTRAS||type==ET_REPETIR) {
			int c1l=0,c1r=0,c1h=0;
			if (GetChild(0)) {
				GetChild(0)->y=y-(type==ET_MIENTRAS?bh+flecha_h:flecha_h); GetChild(0)->x=x;
				GetChild(0)->Calculate(c1l,c1r,c1h);
			} 
			child_dx[0]=0; child_bh[0]=c1h;
			if (c1l>bwl) bwl=c1l;
			if (c1r>bwr) bwr=c1r;
			bwr+=flecha_w; bwl+=flecha_w; bh+=c1h;
			fy-= (type==ET_MIENTRAS)?flecha_h:c1h+flecha_h;
			bh+= (type==ET_MIENTRAS)?flecha_h*3:flecha_h;
		} else if (type==ET_PARA) {
			t_dy=(t_h+margin)/2;
			
			// averiguar cuanto miden las tres etiquetas de abajo en el circulo
			GetChild(1)->x=GetChild(2)->x=GetChild(3)->x=x;
			int vl=0,vr=0,vh=0;
			GetChild(1)->Calculate(vl,vr,vh); 
			int v1=vl+vr; vl=vr=0;
			GetChild(2)->Calculate(vl,vr,vh); 
			int v2=vl+vr; vl=vr=0;
			GetChild(3)->Calculate(vl,vr,vh); 
			int v3=vl+vr;
			if (variante) { v1=v3=0; GetChild(1)->w=GetChild(3)->w=0; }
			else if ((not g_state.edit_on) and GetChild(2)->label.empty()) { v2=0; GetChild(2)->w=0; }
			
			// calcular el ancho del circulo, puede estar dominado por las tres etiquetas de abajo o por la propia 
			int v=v1+v2+v3-2*margin;
			w=(v>t_w?v:t_w)*1.3+2*margin;
			v+=2*margin;
			
			// acomodar el circulo
			bwr=0; bwl=w;
			int c1l=0,c1r=0,c1h=0;
			if (GetChild(0)) {
				GetChild(0)->y=y-flecha_h; GetChild(0)->x=x;
				GetChild(0)->Calculate(c1l,c1r,c1h);
			} 
			child_dx[0]=0; child_bh[0]=c1h;
			if (c1r>bwr) bwr=c1r;
			bwl=bwl+c1l+flecha_w;
			if (c1h>bh) bh=c1h;
			bh+=2*flecha_h;
			fx=-c1l-w/2-flecha_w;
			fy=y+(flecha_h-bh+h)/2;
			
			// acomodar las tres etiquetas
			int cy=fy-h/2-margin/2;
			GetChild(1)->fy=cy; GetChild(1)->MoveX(fx-x+(-v+v1)/2); child_dx[1]=GetChild(1)->fx+v1/2;
			GetChild(2)->fy=cy; GetChild(2)->MoveX(fx-x+(-v+v2)/2+v1); child_dx[2]=GetChild(2)->fx+v2/2;
			GetChild(3)->fy=cy; GetChild(3)->MoveX(fx-x+(v-v3)/2); child_dx[3]=GetChild(3)->fx+v3/2;
		}
	}
}
