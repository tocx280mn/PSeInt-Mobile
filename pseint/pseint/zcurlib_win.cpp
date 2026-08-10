#include "zcurlib.h"
#include <iostream>

#if defined(WIN32) or defined(WIN64)

// codigo windows, basado en http://www.gamedev.net/community/forums/topic.asp?topic_id=308393

#include <windows.h>
#include <conio.h>

void win_setBackColor(int c) {	
	static HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	WORD wAttrib = 0;
	CONSOLE_SCREEN_BUFFER_INFO csbi;
	GetConsoleScreenBufferInfo(hConsole,&csbi);
	wAttrib = csbi.wAttributes;
	wAttrib &= ~(BACKGROUND_BLUE |
		BACKGROUND_GREEN | 
		BACKGROUND_RED |
		BACKGROUND_INTENSITY);
	if (c & 1)		wAttrib |= BACKGROUND_BLUE;
	if (c & 2)		wAttrib |= BACKGROUND_GREEN;
	if (c & 4)		wAttrib |= BACKGROUND_RED;
	if (c & 8)		wAttrib |= BACKGROUND_INTENSITY;
	SetConsoleTextAttribute(hConsole, wAttrib);
}

void win_setForeColor(int c) {	
	static HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);	
	WORD wAttrib = 0;
	CONSOLE_SCREEN_BUFFER_INFO csbi;	
	GetConsoleScreenBufferInfo(hConsole,&csbi);
	wAttrib = csbi.wAttributes;	
	wAttrib &= ~(FOREGROUND_BLUE |
		FOREGROUND_GREEN |
		FOREGROUND_RED |
		FOREGROUND_INTENSITY);
	if (c & 1)		wAttrib |= FOREGROUND_BLUE;
	if (c & 2)		wAttrib |= FOREGROUND_GREEN;
	if (c & 4)		wAttrib |= FOREGROUND_RED;
	if (c & 8)		wAttrib |= FOREGROUND_INTENSITY;
	SetConsoleTextAttribute(hConsole, wAttrib);
}

void win_clrscr() {
	std::cout.flush();
	static HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	COORD coord = {0, 0};	
	DWORD dw;	
	CONSOLE_SCREEN_BUFFER_INFO csbi;
	DWORD dwSize;
	GetConsoleScreenBufferInfo(hConsole,&csbi);
	dwSize = csbi.dwSize.X * csbi.dwSize.Y;
	FillConsoleOutputCharacter(hConsole,
		' ',
		dwSize,
		coord,
		&dw);
	FillConsoleOutputAttribute(hConsole,
		csbi.wAttributes,
		dwSize,
		coord,
		&dw);
	SetConsoleCursorPosition(hConsole, coord);
}

void win_gotoXY(int x, int y) {
	HANDLE screen_buffer_handle = GetStdHandle(STD_OUTPUT_HANDLE);
	COORD coord;
	
	coord.X = x-1;
	coord.Y = y-1;
	
	SetConsoleCursorPosition(screen_buffer_handle, coord);
}

int win_getKey( void ) {
	int ch = getch();
	if ( ch == 0 || ch == 224 )
		ch = 256 + getch();
	return ch;
}


void win_hideCursor() {
	CONSOLE_CURSOR_INFO ConCurInf;
	ConCurInf.dwSize = 100;
	ConCurInf.bVisible = FALSE;
	SetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE),&ConCurInf);
}

void win_showCursor() {
	CONSOLE_CURSOR_INFO ConCurInf;
	ConCurInf.dwSize = 100;
	ConCurInf.bVisible = TRUE;
	SetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE),&ConCurInf);
}

void win_setTitle(const char* title) {
	SetConsoleTitle(title);
}

#endif
