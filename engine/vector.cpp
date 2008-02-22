#include "vector.h"

#include <cmath>
#include <iostream>
using namespace std;

vector_t::vector_t() {
	x = 0;
	y = 0;
	z = 0;
}

vector_t::vector_t(float x, float y, float z) {
	this->set(x,y,z);
}

void vector_t::set(float x, float y, float z) {
	this->x = x;
	this->y = y;
	this->z = z;
}



ostream& operator<<(ostream &s, const vector_t &v) {
	return(s << "(" << v.x << "," << v.y << "," << v.z << ")");  
}


vector_t add(vector_t a, vector_t b) {
	vector_t t(a.x+b.x, a.y+b.y, a.z+b.z);
	return(t);
}

// Multiplication by a scalar
vector_t mul(vector_t a, float b) {
	vector_t t(a.x*b, a.y*b, a.z*b);
	return(t);
}

vector_t sub(vector_t a, vector_t b) {
	vector_t t(a.x-b.x, a.y-b.y, a.z-b.z);
	return(t);
}

vector_t normalize(vector_t a) {
	float inv_magnitude = 1/sqrt(a.x*a.x + a.y*a.y + a.z*a.z);
	vector_t t(a.x*inv_magnitude, a.y*inv_magnitude, a.z*inv_magnitude);
	return(t);
}

float magnitude(vector_t a) {
	return(sqrt(a.x*a.x + a.y*a.y + a.z*a.z));
}


vector_t cross(vector_t a, vector_t b) {
	vector_t t(a.y*b.z - a.z*b.y, a.z*b.x - a.x*b.z, a.x*b.y - a.y*b.x);
	return(t);
}


vector_t midpoint(vector_t a, vector_t b) {
	vector_t t((a.x + b.x)/2, (a.y+b.y)/2, (a.z + b.z)/2);
	return(t);
}

float dot(vector_t a, vector_t b) {
	return(a.x*b.x + a.y*b.y + a.z*b.z);
}


// Distance between two points in 3-space,
// from their vectors from the origin
float dist(vector_t v1, vector_t v2) {
	vector_t v = sub(v1, v2);
	return(sqrt(dot(v,v)));
}




// Find the coordinates of the centroid of a triangle
vector_t triangle_centroid(vector_t v1, vector_t v2, vector_t v3) {
	// This formula is from http://mathworld.wolfram.com/TriangleCentroid.html
	

	vector_t t = mul(add(add(v1, v2), v3), 1.0f/3.0f);

	/*float tx, ty, tz;
	float x,y,z;
	float a, b, c;

	// Compute edge lengths
	a = dist(v1, v2);
	b = dist(v2, v3);
	c = dist(v1, v3);

	// Compute denominator of Mathworld expression
	tx = 1.0f/(a*v1.x + b*v2.x + c*v3.x);
	ty = 1.0f/(a*v1.y + b*v2.y + c*v3.y);
	tz = 1.0f/(a*v1.z + b*v2.z + c*v3.z);

	// Compute centroid from mathworld formula
	x = tx*v1.x + ty*v1.y + tz*v1.z;
	y = tx*v2.x + ty*v2.y + tz*v2.z;
	z = tx*v3.x + ty*v3.y + tz*v3.z;
		
	return(vector_t(x,y,z));*/
	return(t);



}
