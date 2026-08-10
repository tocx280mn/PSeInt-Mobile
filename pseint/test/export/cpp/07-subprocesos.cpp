#include<iostream>
using namespace std;
#define SIN_TIPO string
float funciondoble(float b);
void noretornanada(SIN_TIPO a, SIN_TIPO b);
void porreferencia(float &b);
void recibevector(SIN_TIPO v[]);
void recibematriz(SIN_TIPO m[][]);
int main() {
	SIN_TIPO a[10];
	SIN_TIPO b[3][4];
	float c;
	cout << funciondoble(5) << endl;
	noretornanada(3, 9);
	c = 0;
	porreferencia(c);
	cout << c << endl;
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
	cout << a+b << endl;
}
void porreferencia(float &b) {
	b = 7;
}
void recibevector(SIN_TIPO v[]) {
	cin >> v[0];
}
void recibematriz(SIN_TIPO m[][]) {
	cin >> m[0][0];
}
