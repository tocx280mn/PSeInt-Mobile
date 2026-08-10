#include<iostream>
#include<cmath>
#include<cstdlib>
#include<cctype>
#include<sstream>
using namespace std;
string convertiratexto(float f);
string mayusculas(string s);
string minusculas(string s);
int main() {
	string s;
	float x;
	x = 1.5;
	cout << sqrtf(x) << endl;
	cout << abs(x) << endl;
	cout << log(x) << endl;
	cout << exp(x) << endl;
	cout << sin(x) << endl;
	cout << cos(x) << endl;
	cout << tan(x) << endl;
	cout << asin(x) << endl;
	cout << acos(x) << endl;
	cout << atan(x) << endl;
	cout << int(x+.5) << endl;
	cout << int(x) << endl;
	cout << (rand()%15) << endl;
	s = "Hola";
	cout << s.size() << endl;
	cout << minusculas(s) << endl;
	cout << mayusculas(s) << endl;
	cout << s.substr(1, 3-1) << endl;
	cout << (s+" Mundo") << endl;
	cout << (string("Mundo ")+s) << endl;
	cout << atof("15.5") << endl;
	cout << convertiratexto(15.5) << endl;
	return 0;
}
string convertiratexto(float f) {
	stringstream ss;
	ss << f;
	return ss.str();
}
string mayusculas(string s) {
	for (unsigned int i=0; i<s.size(); ++i)
		s[i] = toupper(s[i]);
	return s;
}
string minusculas(string s) {
	for (unsigned int i=0; i<s.size(); ++i)
		s[i] = tolower(s[i]);
	return s;
}
