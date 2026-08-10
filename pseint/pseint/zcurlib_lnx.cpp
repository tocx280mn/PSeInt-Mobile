#include "zcurlib.h"
#include <iostream>
using namespace std;
static char colors[][12]={
	"\033[0m\033[30m", // COLOR_BLACK
	"\033[0m\033[34m", // COLOR_BLUE
	"\033[0m\033[32m", // COLOR_GREEN
	"\033[0m\033[36m", // COLOR_CYAN
	"\033[0m\033[31m", // COLOR_RED
	"\033[0m\033[35m", // COLOR_MAGENTA
	"\033[0m\033[33m", // COLOR_BROWN
	"\033[0m\033[37m", // COLOR_WHITE
	"\033[1m\033[30m", // COLOR_GRAY
	"\033[1m\033[34m", // COLOR_LBLUE
	"\033[1m\033[32m", // COLOR_LGREEN
	"\033[1m\033[36m", // COLOR_LCYAN
	"\033[1m\033[31m", // COLOR_LRED
	"\033[1m\033[35m", // COLOR_LMAGENTA
	"\033[1m\033[33m", // COLOR_YELLOW
	"\033[1m\033[37m" // COLOR_BWHITE	
};

#define LNX_KEY_UP    27+(91<<8)+(65<<16)
#define LNX_KEY_DOWN  27+(91<<8)+(66<<16)
#define LNX_KEY_LEFT  27+(91<<8)+(68<<16)
#define LNX_KEY_RIGHT 27+(91<<8)+(67<<16)
#define LNX_KEY_DELETE 27+(91<<8)+(51<<16)
#define LNX_KEY_INSERT 27+(91<<8)+(50<<16)

#ifndef __WIN32__
#include <termios.h>
#include <unistd.h>
#endif
// secuencias de escape ANSI

void lnx_setBackColor(int color) {	
	static char aux[10]="\033[43m";
	aux[3]=colors[color][7];
	cout<<aux;
}

void lnx_setForeColor(int color) {
	cout<<colors[color];
}

void lnx_clrscr() {
	cout<<"\033[2J";
}

void lnx_gotoXY(int x, int y) {
	cout<<"\033["<<y<<';'<<x<<'H'<<flush;
}

#ifndef __WIN32__

int lnx_getKey(void) {
	static char buf[256];
	struct termios oldt,
		newt;
	int ch;
	tcgetattr( STDIN_FILENO, &oldt );
	newt = oldt;
	newt.c_lflag &= ~( ICANON | ECHO );
	tcsetattr( STDIN_FILENO, TCSANOW, &newt );
	ch = read(STDIN_FILENO,buf,255); // leer teclas
	if (ch==1)
		ch=buf[0];
	else {
		if (ch==3)
			ch = buf[0]+(buf[1]<<8)+(buf[2]<<16);
		else if (ch==5)
			ch = buf[1]+(buf[2]<<8)+(buf[2]<<16)+(buf[2]<<24); // este no tiene sentido
		else if (ch==4)
			ch = buf[0]+(buf[1]<<8)+(buf[2]<<16)+(buf[2]<<24);
		else
			ch = buf[0]+(buf[1]<<8);
		if (ch==LNX_KEY_UP) ch=KEY_UP;
		else if (ch==LNX_KEY_DOWN) ch=KEY_DOWN;
		else if (ch==LNX_KEY_RIGHT) ch=KEY_RIGHT;
		else if (ch==LNX_KEY_LEFT) ch=KEY_LEFT;
		else if (ch==LNX_KEY_INSERT) ch=KEY_INSERT;
		else if (ch==LNX_KEY_DELETE) ch=KEY_DELETE;
	}
	tcsetattr( STDIN_FILENO, TCSANOW, &oldt );
	return ch;
}
#endif

void lnx_hideCursor() {
	cout<<"\033[?25l"<<flush;
}

void lnx_showCursor() {
	cout<<"\033[?25h"<<flush;
}

void lnx_setTitle(const char* title) {
	const static char esc_start[] = { 0x1b, ']', '0', ';', 0 };
	const static char esc_end[] = { 0x07, 0 };
	cout << esc_start << title << esc_end;
}
