#include<stdio.h>
int main() {
	float c;
	c = 0;
	while (c<=10) {
		c = c+1;
		printf("%f\n", c);
	}
	c = 0;
	do {
		c = c+1;
		printf("%f\n", c);
	} while (c!=10);
	c = 0;
	do {
		c = c+1;
		printf("%f\n", c);
	} while (c<10);
	if (c==10) {
		printf("Si\n");
	}
	if (c==10) {
		printf("Si\n");
	} else {
		printf("No\n");
	}
	return 0;
}
