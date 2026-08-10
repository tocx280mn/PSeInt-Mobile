// programa c++ utilizado para generar automáticamente las entradas de los casos de prueba

#include <iostream>
#include <fstream>
using namespace std;

int main(int argc, char *argv[]) {
	for(int i=0;i<20;i++) {
		int x = 2+rand()%9;
		ofstream o(string("input-")+to_string(i)+".txt");
		o<<x<<endl;
		for(int j=0;j<x;j++)
			o<<15+(rand()%60)*5<<endl;
	}
	return 0;
}

