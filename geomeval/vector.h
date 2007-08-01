#ifndef VECTOR_T
#define VECTOR_T

#include <string>
#include <iostream>
using namespace std;


class vector_t {
private:
	float x;
	float y;
	float z;
public:
	vector_t();	
	vector_t(float x, float y, float z);
	void set(float x, float y, float z);
    friend ostream& operator<<(ostream &s, const vector_t &v);
};

ostream& operator<<(ostream &s, const vector_t &v);

#endif

