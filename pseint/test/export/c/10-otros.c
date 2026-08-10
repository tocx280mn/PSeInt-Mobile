#include<stdio.h>
#include<math.h>
#include<time.h>
void esperar(double t);
int main() {
	printf("\n");
	getchar();
	esperar(100);
	esperar(1*1000);
	printf("%f\n", M_PI);
	return 0;
}
void esperar(double t) {
	clock_t t0 = clock();
	double e = 0;
	do {
		e = 1000*double(clock()-t0)/CLOCKS_PER_SEC;
	} while (e<t);
}
