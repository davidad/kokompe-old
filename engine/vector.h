#ifndef VECTOR_T
#define VECTOR_T

#include <string>
#include <iostream>
using namespace std;


class vector_t {
public:
	float x;
	float y;
	float z;

	vector_t();	
	vector_t(float x, float y, float z);
	void set(float x, float y, float z);
    friend ostream& operator<<(ostream &s, const vector_t &v);
};

ostream& operator<<(ostream &s, const vector_t &v);

vector_t sub(vector_t a, vector_t b);
vector_t normalize(vector_t a);
vector_t cross(vector_t a, vector_t b);
vector_t midpoint(vector_t a, vector_t b);
float dot(vector_t a, vector_t b);

vector_t mul(vector_t a, float b);
vector_t add(vector_t a, vector_t b);
float dist(vector_t v1, vector_t v2);
vector_t triangle_centroid(vector_t v1, vector_t v2, vector_t v3);

float magnitude(vector_t a);



#endif

