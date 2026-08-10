#include<stdio.h>
int main() {
	float a[10];
	float b[3][6];
	float c;
	int i;
	int j;
	for (i=1; i<=10; ++i) {
		a[i-1] = i*10;
	}
	for (typeof(&(a[0])) elemento=&(a[0]); elemento<=&(a[9]); ++elemento) {
		printf("%f\n", (*elemento));
	}
	c = 0;
	for (typeof(&(b[0][0])) x=&(b[0][0]); x<=&(b[2][5]); ++x) {
		c = c+1;
		(*x) = c;
	}
	for (i=3; i>=1; --i) {
		for (j=1; j<=5; j+=2) {
			printf("%f\n", b[i-1][j-1]);
		}
	}
	return 0;
}
