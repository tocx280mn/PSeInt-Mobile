#include <iostream>
#include <cstdlib>
#include "Entity.h"
#include "Global.h"
#include "Text.h"
#include "Renderer.h"

static int bk_x0,bk_x1,bk_y0,bk_y1;

//static void DrawBackground() {
//	int bk_xm=(bk_x0+bk_x1)/2, bk_w=(bk_x1-bk_x0)/2;
//	DrawSolidRectangle(color_shape[ET_COUNT],bk_xm,bk_y1,bk_w,bk_w,bk_y1-bk_y0);
//	bk_x0=bk_x1=g_code.start->d_x;
//	bk_y0=bk_y1=g_code.start->d_y;
//}

void Entity::DrawNassiShne(bool force) {
	if (bwl>2000) std::cerr << bwl << std::endl;
	int icolor=g_config.shape_colors?type:ET_COUNT;
	if (icolor==ET_OPCION) icolor=ET_SEGUN;
//	if (this==g_code.start) DrawBackground();
//	if (mouse==this) DrawSolidRectangle(color_shape[ET_COUNT],d_fx,d_fy,d_bwl,d_bwr,d_bh);
	if (this==g_state.mouse)
		rndr.setColor(g_colors.ghost)
		    .drawSolidRectangle({g_view.d_dx+fx, g_view.d_dy+fy-margin}, bwl-margin, bwr-margin,bh-2*margin);
	if (type!=ET_AUX_PARA&&type!=ET_OPCION) {
		if ((not nolink) or g_state.mouse==this) {
			rndr.setColor(g_colors.shape[icolor]).drawSolidRectangle({d_fx,d_fy}, d_bwl, d_bwr,d_bh);
			rndr.setColor(g_colors.border[icolor]).drawRectangle({d_fx,d_fy}, d_bwl, d_bwr,d_bh);
		}
	}
	// guardar bb para el gran retangulo blanco de fondo
	if (d_fy>bk_y1) bk_y1=d_fy;
	if (d_fy-d_h<bk_y0) bk_y0=d_fy-d_h;
	if (d_fx-d_bwl<bk_x0) bk_x0=d_fx-d_bwl;
	if (d_fx+d_bwr>bk_x1) bk_x1=d_fx+d_bwr;
	if (!nolink) {
		if (type==ET_OPCION) {
			rndr.setColor(g_colors.border[icolor]);
//			if (edit_on) { glVertex2i(d_x-d_bwl+flecha_w,d_y); glVertex2i(d_x-d_bwl+flecha_w,d_y-h); }
			int px=GetParent()->child_dx[GetChildId()]-bwl;
			int px0=-GetParent()->bwl, px1=GetParent()->child_dx[GetParent()->GetChildCount()-1]-GetParent()->GetChild(GetParent()->GetChildCount()-1)->bwl;
			int py0=GetParent()->d_y, ph = GetParent()->h;
			int ax=px-px0; //if (ax<0) ax=-ax;
			if (px1==px0) px1++;
			int y=py0 - (ax*4*h/2)/(px1-px0); // el coef 4*h/2 debe coincidir con el de segun
			rndr.drawLine({d_x-d_bwl,d_y-h}, {d_x-d_bwl,y});
			if (!GetChild(0)) { rndr.drawLine({d_x-d_bwl,d_y-h}, {d_x+d_bwr,d_y-h}); }
		} else if (type==ET_SEGUN) {
			rndr.setColor(g_colors.border[icolor]);
			int px = d_x + child_dx[GetChildCount()-1]-GetChild(GetChildCount()-1)->d_bwl;
			int py = d_y-4*h/2; // el coef 4*h/2 debe coincidir con el de opcion
			rndr.drawLine({d_x-d_bwl,d_y}, {px,py});
			rndr.drawLine({px,py}, {d_x+d_bwr,d_y});
			for(int i=0;i<GetChildCount()-1;i++) {
				if (GetChild(i)==g_state.mouse) continue;
				rndr.drawLine({d_x+child_dx[i]+GetChild(i)->bwr,GetChild(i)->d_y-GetChild(i)->d_h},
				              {d_x+child_dx[i]+GetChild(i)->bwr,d_y-d_bh});
			}
			
		} else 
		if (type==ET_SI) {
			rndr.setColor(g_colors.arrow);
			rndr.drawText({d_x-d_bwl+10,d_y-2*h}, "Si");
			rndr.drawText({d_x+d_bwr-35,d_y-2*h}, "No");
			rndr.setColor(g_colors.border[icolor]);
			int px=d_x;
			if (GetChild(1)) px+=child_dx[1]+GetChild(1)->d_bwr;
			else if (GetChild(0)) px+=child_dx[0]-GetChild(0)->d_bwl;
			rndr.drawLine({d_x-d_bwl,d_y}, {px,d_y-2*h-margin});
			rndr.drawLine({px,d_y-2*h-margin}, {d_x+d_bwr,d_y});
			rndr.drawLine({d_x-d_bwl,d_y-2*h-margin}, {d_x+d_bwr,d_y-2*h-margin});
		}
	}

	if (type==ET_OPCION) { // + para agregar opciones
		if (g_state.edit_on and g_state.mouse!=this) {
			rndr.setColor(g_colors.label_high[4]);
			rndr.drawLine({d_x-d_bwl+3*flecha_w/4,d_y-d_h/2}, {d_x-d_bwl+1*flecha_w/4,d_y-d_h/2});
			rndr.drawLine({d_x-d_bwl+flecha_w/2,d_y-1*d_h/3}, {d_x-d_bwl+flecha_w/2,d_y-2*d_h/3});
		}
	}
	
	if (type==ET_SELECTION) { // + para agregar opciones
		int w = margin, x = d_fx+d_w/2-margin-margin/2, y = d_fy-margin-margin/2;
		rndr.setWidth(g_constants.line_width_bordes);
		w /= 2;
		rndr.setColor(g_colors.arrow);
		rndr.drawLine({x-w,y-w}, {x+w,y+w});
		rndr.drawLine({x+w,y-w}, {x-w,y+w});
		rndr.setWidth(g_constants.line_width_flechas);
	}
	// texto;
	rndr.flush();
	DrawText();
}

void Entity::CalculateNassiShne() { // calcula lo propio y manda a calcular al siguiente y a sus hijos, y acumula en gw,gh el tamaño de este item (para armar el tamaño del bloque)
	
	if (type==ET_SELECTION) {
		if (!GetChild(0)) return;
		GetChild(0)->x = x; GetChild(0)->y = y - 3*margin;
		bwl = bwr = bh = 0;
		GetChild(0)->Calculate(bwl,bwr,bh);
		bwl+=margin; bwr+=margin; bh+=margin*5;
		h = bh; w = bwl+bwr; t_dy=t_dx=0; fx=x+(bwr-bwl)/2; fy=y;
		child_dx[0] = 0; child_bh[0] = bh;
		return;
	}
	
	// calcular tamaños de la forma segun el texto
	if (type==ET_COMENTARIO and (not g_config.show_comments)) { 
		t_w=bwl=bwr=0; bh=0;
	} else {
		if (!t_w) w=margin*6; else { w=t_w; w+=2*margin; } h=t_h+2*margin; 
		t_dy=t_dx=0; fx=x; fy=y; bh=h; bwr=bwl=w/2; // esto es si fuera solo la forma
	}
	// si son estructuras de control...
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
				else GetChild(0)->ResizeW(bwl+bwr,false);
				child_dx[0]-=(cwr-cwl)/2;
				GetChild(0)->MoveX(child_dx[0]);
			}
			// el ancho se lo define el segun padre
			w=bwl+bwr;
		} else
		if (type==ET_SEGUN) {
			bh+=h;
			bwr=bwl=(w=t_w*2)/2;
			int sw=0, sh=0;
			for (int i=0;i<GetChildCount();i++) {
				int cwl=0,cwr=0,ch=0;
				GetChild(i)->x=0; GetChild(i)->y=y-2*h;
				GetChild(i)->Calculate(cwl,cwr,ch);
				child_bh[i]=ch; child_dx[i]=sw+cwl;
				sw+=cwl+cwr;
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
		} else 
		if (type==ET_SI) {
			bh*=2;
			bh+=margin;
			// calcular tamaño hijo por verdadero
			int c1l=0,c1r=0,c1h=0;
			if (GetChild(0)) {
				GetChild(0)->y=y-bh; GetChild(0)->x=x;
				GetChild(0)->Calculate(c1l,c1r,c1h);
			} else {
				c1r=c1l=c1h=20;
			}
			// calcular tamaño hijo por falso
			int c2l=0,c2r=0,c2h=0;
			if (GetChild(1)) {
				GetChild(1)->y=y-bh; GetChild(1)->x=x;
				GetChild(1)->Calculate(c2l,c2r,c2h);
			} else {
				c2r=c2l=c2h=20;
			}
			// ajustar tamaño propio
			if (c1r+c1l+c2r+c2l>w) {
				bwl=bwr=(c1r+c1l+c2r+c2l)/2;
			} else  {
				int dw=(w-(c1r+c1l+c2r+c2l))/2;
				if (GetChild(0)) GetChild(0)->ResizeW(c1r+c1l+dw,false);
				if (GetChild(1)) GetChild(1)->ResizeW(c2r+c2l+dw,false);
				c1l+=dw/2; c1r+=dw/2; c2l+=dw/2; c2r+=dw/2;
			}
			bh += (c1h>c2h?c1h:c2h);
			child_bh[0]=c1h; child_bh[1]=c2h;
			// mover hijos horizontalmente
			child_dx[0] = bwr-c1r;
			child_dx[1] = -bwl+c2l;
			if (GetChild(0)) GetChild(0)->MoveX(child_dx[0]);
			if (GetChild(1)) GetChild(1)->MoveX(child_dx[1]);
		} else
		if (type==ET_MIENTRAS||type==ET_PARA||type==ET_REPETIR) {
			if (type==ET_PARA) {
				GetChild(1)->CalculateNassiShne();
				GetChild(2)->CalculateNassiShne();
				GetChild(3)->CalculateNassiShne();
				t_dx=GetChild(1)->t_w+GetChild(2)->t_w+GetChild(3)->t_w;
				w=w+t_dx; bwr=bwl=w/2; t_dx=-t_dx/2;
			}
			// calcular tamaño hijo por verdadero
			int c1l=0,c1r=0,c1h=0;
			if (GetChild(0)) {
				GetChild(0)->y=y-(type==ET_REPETIR?0:bh); GetChild(0)->x=x;
				GetChild(0)->Calculate(c1l,c1r,c1h);
			} else {
				c1r=c1l=c1h=20;
			}
			// ajustar tamaño propio
			if (c1r+c1l+40>w) 
				bwl=bwr=(c1r+c1l)/2+20;
			else
				if (GetChild(0)) GetChild(0)->ResizeW(w-40,false);
			bh+=c1h;
			child_bh[0]=0;
			if (GetChild(0)) GetChild(0)->MoveX(child_dx[0]=20);
			if (type==ET_REPETIR) t_dy=-c1h;
			else if (type==ET_PARA) { // acomodar las etiquetas "desde .. hasta .. con paso .."
				GetChild(1)->y=GetChild(1)->fy=fy;
				GetChild(1)->x=GetChild(1)->fx=fx+t_w/2+t_dx+GetChild(1)->t_w/2;
				GetChild(3)->y=GetChild(3)->fy=fy;
				GetChild(3)->x=GetChild(3)->fx=GetChild(1)->fx+GetChild(1)->t_w/2+GetChild(1)->t_dx+GetChild(3)->t_w/2;
				GetChild(2)->y=GetChild(2)->fy=fy;
				GetChild(2)->x=GetChild(2)->fx=GetChild(3)->fx+GetChild(3)->t_w/2+GetChild(3)->t_dx+GetChild(2)->t_w/2;
			}
		} 
	}
	if (!GetNext() && GetPrev()) {
		int max=bwl+bwr;
		Entity *et=GetPrev();
		while (et) {
			if (et->bwl+et->bwr>max)
				max=et->bwl+et->bwr;
			et=et->GetPrev();
		}
		ResizeW(max,true);
	}
}
