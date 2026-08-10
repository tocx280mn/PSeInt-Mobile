#include <iostream>
#include <cstdlib>
#include "Application.h"
#include "mxMainWindow.h"
#include "version.h"
#include "mxCreatorWindow.h"
#include "../wxPSeInt/osdep.h"

IMPLEMENT_APP(mxApplication)
	
Package pack;

bool mxApplication::OnInit() {
	
	srand(time(0));
	_handle_version_query("psEval",false);
	
	OSDep::AppInit();
	
	if (argc<4) {
		std::cerr<<"Use "<<argv[0]<<" <archivo_de_ejercicio> <clave> <comando pseint...>"<<std::endl;
		return false;
	}
	bool creator_mode = false;
	wxString args; 
	for(int i=3;i<argc;i++) { 
		if (wxString(argv[i])=="--create_new_test_package=1") creator_mode=true;
		if (wxString(argv[i]).Contains(' ')) {
			args+="\""; args+=argv[i]; args+="\" "; 
		} else {
			args+=argv[i]; args+=" "; 
		}
	}
	args+="--foreval";
	if (creator_mode) { new mxCreatorWindow(args); return true; }
	else return (new mxMainWindow())->Start(argv[1],argv[2],args);
}


