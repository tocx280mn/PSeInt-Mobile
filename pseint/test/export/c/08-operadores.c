#include<stdio.h>
#include<math.h>
#include<stdbool.h>
#include<string.h>
char *get_aux_buffer(double t);
#define MAX_STRLEN 256
int main() {
	float a;
	float b;
	bool c;
	bool d;
	char e[MAX_STRLEN];
	char f[MAX_STRLEN];
	a = 11;
	b = 2;
	c = true;
	d = false;
	e = "Hola";
	f = "Mundo";
	printf("%f\n", a+b);
	printf("%f\n", a*b);
	printf("%f\n", a/b);
	printf("%f\n", a-b);
	printf("%f\n", pow(a, b));
	printf("%f\n", a%b);
	printf("%i\n", a==b);
	printf("%i\n", a!=b);
	printf("%i\n", a<b);
	printf("%i\n", a>b);
	printf("%i\n", a<=b);
	printf("%i\n", a>=b);
	printf("%i\n", c && d);
	printf("%i\n", c || d);
	printf("%i\n", !(c && d));
	printf("%s\n", strcat(strcpy(get_aux_buffer(), strcat(strcpy(get_aux_buffer(), e), " ")), f));
	printf("%i\n", strcmp(e, f)==0);
	printf("%i\n", strcmp(e, f)!=0);
	printf("%i\n", strcmp(e, f)<0);
	printf("%i\n", strcmp(e, f)>0);
	printf("%i\n", strcmp(e, f)<=0);
	printf("%i\n", strcmp(e, f)>=0);
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
