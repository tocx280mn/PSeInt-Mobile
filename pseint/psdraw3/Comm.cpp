#include <cstdlib>
#include <sstream>
#include <fstream>
#include <iostream>
#include "GLstuff.h"
#include "../pseint/zockets.h"
#include "Comm.h"
#include "Global.h"
#include "Entity.h"
#include "Events.h"
#include "Load.h"
#include "MainWindow.h"

ZOCKET zocket=ZOCKET_ERROR; // para comunicarse con wxPSeInt


bool Connect(int port, int id) {
	zocket = zocket_llamar(port,"127.0.0.1");
	if (zocket==ZOCKET_ERROR) return false;
	std::stringstream ss;
	ss<<"hello-flow "<<id<<"\n";
	std::string s=ss.str();
	zocket_escribir(zocket,s.c_str(),s.size());
	if (zocket==ZOCKET_ERROR) return false;
	return true;
}

void NotifyModification() {
	if (zocket==ZOCKET_ERROR) return;
	zocket_escribir(zocket,"modified\n",9);
}
bool SendUpdate(int action) {
	if (!g_state.modified && action==MO_UPDATE) return true;
	if (!Save()) return false;
	if (zocket==ZOCKET_ERROR) return false;
	if (action==MO_UPDATE)
		zocket_escribir(zocket,"update\n",7);
	else if (action==MO_EXPORT)
		zocket_escribir(zocket,"export\n",7);
	else if (action==MO_RUN)
		zocket_escribir(zocket,"run\n",4);
	else if (action==MO_DEBUG)
		zocket_escribir(zocket,"debug\n",6);
	else
		zocket_escribir(zocket,"reload\n",7);
	if (zocket==ZOCKET_ERROR) return false;
	return true;
}

bool SendHelp() {
	if (zocket==ZOCKET_ERROR) return false;
	zocket_escribir(zocket,"help\n",5);
	if (zocket==ZOCKET_ERROR) return false;
	return true;
}

bool SendConfig(const char *key, bool val) {
	if (zocket==ZOCKET_ERROR) return false;
	std::string msg = "config:"; msg += key; msg += (val?"=1\n":"=0\n");
	zocket_escribir(zocket,msg.c_str(),msg.size());
	if (zocket==ZOCKET_ERROR) return false;
	return true;
}

void CloseComm( ) {
	if (zocket!=ZOCKET_ERROR) {
		zocket_cerrar(zocket);
	}
}

void ReadComm( ) {
	static char *rec=new char[256]; int cant=256;
	if (zocket!=ZOCKET_ERROR && zocket_leer(zocket,rec,cant)) {
		rec[cant]=0; std::string sr_full=rec;
		size_t pn=sr_full.find('\n'),lpn=0;
		while (pn!=std::string::npos) {
			std::string sr=sr_full.substr(lpn,pn-lpn); lpn=pn+1;
			if (sr=="edit") { 
				g_state.edit_on=true; Raise();
			}
			else if (sr=="send update") { 
				SendUpdate(MO_UPDATE);
			} 
			else if (sr=="send debug") { 
				SendUpdate(MO_DEBUG);
			} 
			else if (sr=="send run") { 
				SendUpdate(MO_RUN);
			} 
			else if (sr=="raise") { 
				Raise();
			}
			else if (sr=="quit") {
				Salir();
			}
			else if (sr=="errors reset") {
				Entity::AllIterator it = Entity::AllBegin();
				while (it!=Entity::AllEnd()) {
					it->error.clear();
					++it;
				} 
			}
			else if (sr.substr(0,11)=="errors add ") {
				sr=sr.substr(11);
				size_t p=sr.find(' ');
				auto it = g_code.code2draw.find(sr.substr(0,p));
				if (it!=g_code.code2draw.end()) it->second.entidad->error+=sr.substr(p+1)+"  ";
			}
			else if (sr.substr(0,5)=="step ") {
				sr=sr.substr(5);
				auto it = g_code.code2draw.find(sr);
				if (it!=g_code.code2draw.end()) 
					FocusEntity(&(it->second));
			}
			else if (sr=="debug start") { 
				g_state.debugging=true;
				if (g_state.edit_on) ToggleEditable(); 
				Raise();
			}
			else if (sr.substr(0,4)=="pos ") { 
				sr.erase(0,4); int p=sr.find(' ',0);
				int x=atoi(sr.substr(0,p).c_str());
				int y=atoi(sr.substr(p+1).c_str());
				g_main_window->Move(x,y);
			} else if (sr.substr(0,5)=="size ") {
				sr.erase(0,5); int p=sr.find(' ',0);
				int w=atoi(sr.substr(0,p).c_str());
				int h=atoi(sr.substr(p+1).c_str());
				g_main_window->SetSize(w,h);
			}
			else if (sr=="debug stop") {
				g_state.debugging=false;
				if (!g_state.edit_on) ToggleEditable();
			}
			pn=sr_full.find('\n',lpn);
		}
	}
}

