#include "vector.h"


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