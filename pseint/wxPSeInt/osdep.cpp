#include "osdep.h"


#if defined(__WIN32__) || defined(__WIN64__)

#	include <windows.h>
#	include <tlhelp32.h>
#	include <stddef.h>
#	include <stdlib.h>
#	include <cstdio>

	typedef enum _MONITOR_DPI_TYPE { 
		MDT_EFFECTIVE_DPI  = 0,
		MDT_ANGULAR_DPI    = 1,
		MDT_RAW_DPI        = 2,
		MDT_DEFAULT        = MDT_EFFECTIVE_DPI
	} MONITOR_DPI_TYPE; // DWORD?


	void OSDep::AppInit() {
		HMODULE user32 = LoadLibrary(TEXT("user32.dll"));
		typedef BOOL (WINAPI *SetProcessDPIAwareFunc)();
		SetProcessDPIAwareFunc setDPIAware = (SetProcessDPIAwareFunc)GetProcAddress(user32,"SetProcessDPIAware");
		if (setDPIAware) setDPIAware();
		FreeLibrary(user32);
	}

	static int GetDPI_impl() {
		HMODULE shcore = LoadLibrary(TEXT("Shcore.dll"));
		if (!shcore) return 0;
		typedef HRESULT (WINAPI *GetDpiForMonitorFunc)(HMONITOR,_MONITOR_DPI_TYPE,UINT*,UINT*);
		GetDpiForMonitorFunc GetDpiForMonitor = (GetDpiForMonitorFunc)GetProcAddress(shcore,"GetDpiForMonitor");
		UINT x=0,y=0;
		if (GetDpiForMonitor)
			GetDpiForMonitor( MonitorFromWindow(GetDesktopWindow(),MONITOR_DEFAULTTOPRIMARY),MDT_EFFECTIVE_DPI, &x, &y);
		FreeLibrary(shcore);
		return x;
	}

	int OSDep::GetDPI() {
		static int x=-1;
		if (x==-1) { x = GetDPI_impl(); if (x<=0) x=96; }
		return x;
	}

#endif







#ifdef __linux__
	
#	include <cstdlib>
#	include <iostream>
#	include <list>
#	include <X11/Xlib.h>
#	include <X11/Xatom.h>

	void OSDep::AppInit() {
		
	}

	static int GetDPI_impl() {
		char *displayname = nullptr;
		Display *dpy = XOpenDisplay (displayname);
		// 25.4 mm = 1 inch
		double xres = ((((double) DisplayWidth(dpy,0)) * 25.4) / ((double) DisplayWidthMM(dpy,0)));
		//	double yres = ((((double) DisplayHeight(dpy,0)) * 25.4) / ((double) DisplayHeightMM(dpy,0)));
		int x = (int) (xres + 0.5) /*, y = (int) (yres + 0.5)*/;
		XCloseDisplay (dpy);
		return x;
	}

	int OSDep::GetDPI() {
		static int x=-1;
		if (x==-1) { x = GetDPI_impl(); if (x<=0) x=96; }
		return x;
	}

#endif








#ifdef __APPLE__

	// esto es para evitar el problema de no poder hacerle foco a la ventana en Mac sin tener que hacer un application bundle (ver OnInit)
#	ifdef __WXMAC__
#		include <ApplicationServices/ApplicationServices.h>
#	endif

	void OSDep::AppInit() {
		// esto es para evitar el problema de no poder hacerle foco a la ventana en Mac sin tener que hacer un application bundle
//		ProcessSerialNumber PSN;
//		GetCurrentProcess(&PSN);
//		TransformProcessType(&PSN,kProcessTransformToForegroundApplication); // este es para que pueda recibir el foco
//		SetFrontProcess( &PSN ); // este es para que no aparezca en segundo plano
	}

	int OSDep::GetDPI() { 
		return 96; 
	}

#endif
