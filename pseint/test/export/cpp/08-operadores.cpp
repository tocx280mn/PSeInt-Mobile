#include<iostream>
#include<cmath>
using namespace std;
int main() {
	float a;
	float b;
	bool c;
	bool d;
	string e;
	string f;
	a = 11;
	b = 2;
	c = true;
	d = false;
	e = "Hola";
	f = "Mundo";
	cout << a+b << endl;
	cout << a*b << endl;
	cout << a/b << endl;
	cout << a-b << endl;
	cout << pow(a, b) << endl;
	cout << a%b << endl;
	cout << a==b << endl;
	cout << a!=b << endl;
	cout << a<b << endl;
	cout << a>b << endl;
	cout << a<=b << endl;
	cout << a>=b << endl;
	cout << c && d << endl;
	cout << c || d << endl;
	cout << !(c && d) << endl;
	cout << e+" "+f << endl;
	cout << e==f << endl;
	cout << e!=f << endl;
	cout << e<f << endl;
	cout << e>f << endl;
	cout << e<=f << endl;
	cout << e>=f << endl;
	return 0;
}
