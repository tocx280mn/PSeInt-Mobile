#include<stdio.h>
#include<math.h>
#include<stdlib.h>
#include<ctype.h>
#include<string.h>
char *convertiratexto(float f);
char *mayusculas(const char *s);
char *minusculas(const char *s);
char *subcadena(const char *s, int desde, int cuantos);
char *get_aux_buffer(double t);
#define MAX_STRLEN 256
int main() {
	char s[MAX_STRLEN];
	float x;
	x = 1.5;
	printf("%f\n", sqrtf(x));
	printf("%f\n", abs(x));
	printf("%f\n", log(x));
	printf("%f\n", exp(x));
	printf("%f\n", sin(x));
	printf("%f\n", cos(x));
	printf("%f\n", tan(x));
	printf("%f\n", asin(x));
	printf("%f\n", acos(x));
	printf("%f\n", atan(x));
	printf("%i\n", int(x+.5));
	printf("%i\n", int(x));
	printf("%i\n", (rand()%15));
	s = "Hola";
	printf("%i\n", strlen(s));
	printf("%s\n", minusculas(s));
	printf("%s\n", mayusculas(s));
	printf("%s\n", subcadena(s, 1, 3-1));
	printf("%s\n", strcat(strcpy(get_aux_buffer(), s), " Mundo"));
	printf("%s\n", strcat(strcpy(get_aux_buffer(), "Mundo "), s));
	printf("%f\n", atof("15.5"));
	printf("%s\n", convertiratexto(15.5));
	return 0;
}
#define MAX_BUFFERS 10
char *get_aux_buffer(double t) {
	static char buffers[MAX_BUFFERS][MAX_STRLEN];
	static int count = -1;
	count = count+1;
	if (count==MAX_BUFFERS) count = 0;
	return buffers[count];
}
char *convertiratexto(float f) {
	char *buf = get_aux_buffer();
	sprintf(buf, "%f", f);
	return buf;
}
char *mayusculas(const char *s) {
	char *buf = get_aux_buffer();
	for (unsigned int i=0; i<s.size(); ++i)
		buf[i] = toupper(s[i]);
	return buf;
}
char *minusculas(const char *s) {
	char *buf = get_aux_buffer();
	for (unsigned int i=0; i<s.size(); ++i)
		buf[i] = tolower(s[i]);
	return buf;
}
char *subcadena(const char *s, int desde, int cuantos) {
	char *buf = get_aux_buffer();
	strncpy(buf, s+desde,cuantos);
	buf[cuantos] = '\0';
	return buf;
}
