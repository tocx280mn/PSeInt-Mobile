#include<iostream>
using namespace std;
int main() {
	float a[10];
	float b[3][6];
	float c;
	int i;
	int j;
	for (i=1; i<=10; ++i) {
		a[i-1] = i*10;
	}
	for (auto aux_ptr_0=&(a[0]); aux_ptr_0<=&(a[9]); ++aux_ptr_0) {
		auto &elemento = *aux_ptr_0;
		cout << elemento << endl;
	}
	c = 0;
	for (auto aux_ptr_0=&(b[0][0]); aux_ptr_0<=&(b[2][5]); ++aux_ptr_0) {
		auto &x = *aux_ptr_0;
		c = c+1;
		x = c;
	}
	for (i=3; i>=1; --i) {
		for (j=1; j<=5; j+=2) {
			cout << b[i-1][j-1] << endl;
		}
	}
	return 0;
}
