#include <cmath>
#include <iostream>
using namespace std;


int main(int argc, char **argv) {
	float x = -9; 

	cout << fmodf(x, (float)2*M_PI);
	cout << endl;



	return(0);
}



