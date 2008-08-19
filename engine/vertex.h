#ifndef VERTEX_H
#define VERTEX_H

#include "vector.h"
#include "trimesh_node.h"
#include "space_interval.h"
#include <list>
using namespace std;

class trimesh_node_t;

class vertex_t : public vector_t {
public:
	int number;
       	space_interval_t si;
	int dirty;
	int clause;
	list<trimesh_node_t*> triangle_list;
	vertex_t(float x, float y, float z)
		: vector_t(x,y,z),
	  number(-1), dirty(0),clause(-1) {		
	}
	vertex_t(float ax, float ay, float az, int a_number) {
		x=ax;
		y=ay;
		z=az;
		number=a_number;
		dirty = 0;
		clause = -1;
	}
	vertex_t() {
	}
	vertex_t(const vector_t& base)
		: vector_t(base) {
	}
	
	void set_vector(const vector_t& a) {
		x = a.x;
		y = a.y;
		z = a.z;
	}
};

#endif
