#include<stdio.h>
#define SIN_TIPO float
float funciondoble(float b);
void noretornanada(SIN_TIPO a, SIN_TIPO b);
void porreferencia(float *b);
void recibevector(SIN_TIPO v[]);
void recibematriz(SIN_TIPO m[][]);
int main() {
	SIN_TIPO a[10];
	SIN_TIPO b[3][4];
	float c;
	printf("%f\n", funciondoble(5));
	noretornanada(3, 9);
	c = 0;
	porreferencia(&c);
	printf("%f\n", c);
	recibevector(a);
	recibematriz(b);
	return 0;
}
float funciondoble(float b) {
	float a;
	a = 2*b;
	return a;
}
void noretornanada(SIN_TIPO a, SIN_TIPO b) {
	printf("%f\n", a+b);
}
void porreferencia(float *b) {
	(*b) = 7;
}
void recibevector(SIN_TIPO v[]) {
	scanf("%f", v[0]);
}
void recibematriz(SIN_TIPO m[][]) {
	scanf("%f", m[0][0]);
}
