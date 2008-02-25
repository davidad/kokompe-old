#ifndef PLANE_T
#define PLANT_T

#include <string>
#include <iostream>
using namespace std;

#include "vector.h"

class line_t;

// An (infinte) plane in 3-dimensional space

class plane_t {
public:
	vector_t n;			// Normal vector to the plane
	float d;			// Plane defined by dot(n,p) = d, for point p

	plane_t();
	void set(vector_t point, vector_t normal);

	static vector_t three_plane_intersection(plane_t &a, plane_t &b, plane_t &c);
	static line_t two_plane_intersection(plane_t &a, plane_t &b);

};



// An (infinite) line in 3-dimensional space

class line_t {
public:
	vector_t v;			// Vector pointing along line		
	vector_t p;			// Point on the line						

	line_t();
	void set(vector_t point, vector_t line_vector); 

	static int line_box_intersection(line_t &line, vector_t box_lower, vector_t box_upper, vector_t *inside_point);
	static int nearest_point_on_line(line_t &line, vector_t point, vector_t *near_point);

};




#endif
