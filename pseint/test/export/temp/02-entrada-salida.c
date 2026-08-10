#include<stdio.h>
#include<stdbool.h>
#define MAX_STRLEN 256
int main() {
	int a;
	float b;
	char c[MAX_STRLEN];
	bool d;
	scanf("%i", &a);
	scanf("%f", &b);
	scanf("%s", c);
	scanf("%i", &d);
	printf("%i\n", a);
	printf("%f\n", b);
	printf("%s\n", c);
	printf("%i\n", d);
	scanf("%i", &a);
	scanf("%f", &b);
	scanf("%s", c);
	scanf("%i", &d);
	printf("%i%f%s%i\n", a, b, c, d);
	printf("Esta linea no lleva enter al final");
	return 0;
}
