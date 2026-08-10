#ifndef TEXT_H
#define TEXT_H
#include <string>

void begin_texto(); 
void dibujar_caracter(const char chr); 
void end_texto(); 
double get_char_width();

void GetTextSize(const std::string &label, int &w, int &h);
void DrawTextRaster(const float *color, int x, int y, const char *t);

#endif

