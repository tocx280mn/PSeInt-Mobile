#include<stdio.h>
#define ARREGLO_MAX 100
#define SIN_TIPO float
int main() {
	SIN_TIPO a[10];
	SIN_TIPO b[20][30];
	SIN_TIPO c[40];
	SIN_TIPO d[50][5];
	SIN_TIPO e[ARREGLO_MAX];
	int n;
	scanf("%i", &n);
	scanf("%f", a[0]);
	a[9] = a[0];
	printf("%f\n", b[9][9]);
	return 0;
}
