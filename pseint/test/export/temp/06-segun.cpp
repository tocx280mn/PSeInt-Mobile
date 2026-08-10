#include<iostream>
using namespace std;
int main() {
	int c;
	cin >> c;
	switch (c) {
	case 1:
		cout << "1" << endl;
		break;
	case 3:
	case 5:
	case 9:
		cout << "3, 5 o 9" << endl;
		break;
	case 7:
		cout << "7" << endl;
		break;
	default:
		cout << "ni 1, ni 3, ni 5, ni 7, ni 9" << endl;
	}
	return 0;
}
