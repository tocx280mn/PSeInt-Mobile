#include<iostream>
using namespace std;
int main() {
	float c;
	c = 0;
	while (c<=10) {
		c = c+1;
		cout << c << endl;
	}
	c = 0;
	do {
		c = c+1;
		cout << c << endl;
	} while (c!=10);
	c = 0;
	do {
		c = c+1;
		cout << c << endl;
	} while (c<10);
	if (c==10) {
		cout << "Si" << endl;
	}
	if (c==10) {
		cout << "Si" << endl;
	} else {
		cout << "No" << endl;
	}
	return 0;
}
