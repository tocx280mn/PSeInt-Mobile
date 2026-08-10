#include<iostream>
#include<cmath>
#include<ctime>
using namespace std;
void esperar(double t);
int main() {
	cout << endl;
	cin.get();
	esperar(100);
	esperar(1*1000);
	cout << M_PI << endl;
	return 0;
}
void esperar(double t) {
	clock_t t0 = clock();
	double e = 0;
	do {
		e = 1000*double(clock()-t0)/CLOCKS_PER_SEC;
	} while (e<t);
}
