#ifndef ZCURLIB_H
#define ZCURLIB_H

#define COLOR_BLACK	0
#define COLOR_BLUE	1
#define COLOR_GREEN	2
#define COLOR_CYAN	3
#define COLOR_RED	4
#define COLOR_MAGENTA	5
#define COLOR_BROWN	6
#define COLOR_WHITE	7
#define COLOR_GRAY	8
#define COLOR_LBLUE	9
#define COLOR_LGREEN	10
#define COLOR_LCYAN	11
#define COLOR_LRED	12
#define COLOR_LMAGENTA	13
#define COLOR_YELLOW	14
#define COLOR_BWHITE	15   

#define KEY_ESC   27
#define KEY_UP    256+72
#define KEY_DOWN  256+80
#define KEY_LEFT  256+75
#define KEY_RIGHT 256+77
#define KEY_DELETE 256+83
#define KEY_INSERT 256+82
#define KEY_ENTER 13
#define KEY_TAB 9
#include <string>

void setBackColor(int color);	
void setForeColor(int color);
void clrscr();
void gotoXY(int x, int y);
int getKey(void);
void hideCursor();
void showCursor();
void setTitle(const char* title);
std::string getLine();

#endif


