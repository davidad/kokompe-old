

//#include <stdfx.h>

#include <iostream>
using namespace std;


#include "space_interval.h"

// An interval of 3D real space


// Empty interval constructor
space_interval_t::space_interval_t() {
  interval_t i;
  i.set_real_number(0.0);
  X = i;
  Y = i;
  Z = i;
}

// Constructor from intervals
space_interval_t::space_interval_t(interval_t& X_interval, interval_t& Y_interval, interval_t& Z_interval) {
  X = X_interval;
  Y = Y_interval;
  Z = Z_interval;
}

void space_interval_t::set(float xmin, float xmax, float ymin, float ymax, float zmin, float zmax) {
	X.set_real_interval(xmin, xmax);
	Y.set_real_interval(ymin, ymax);
	Z.set_real_interval(zmin, zmax);
}



ostream& operator<<(ostream&s, const space_interval_t &si) {
  return( s << "{ X:" << si.X <<" Y:" << si.Y << " Z:" << si.Z <<" }");   
}

// Split a space interval into one of the eight orthants
void space_interval_t::split(int i) {
  X.split(i & 0x1);
  Y.split(i & 0x2);
  Z.split(i & 0x4);
}

// get the zone where a particular point resides
int space_interval_t::get_zone(float x, float y, float z) {
  int result = 0;
  
  if (x > X.get_center())
    result |= 0x1;
  if (y > Y.get_center())
    result |= 0x2;
  if (z > Z.get_center())
    result |= 0x4;
  return(result);
}

void space_interval_t::set_point(float x, float y, float z) {
  X.set_real_number(x);
  Y.set_real_number(y);
  Z.set_real_number(z);
}



// See if two space intervals overlap
int space_interval_t::overlaps(const space_interval_t &b) const {
  return(X.overlaps(b.X) && Y.overlaps(b.Y) && Z.overlaps(b.Z));
}

int space_interval_t::is_on(float x, float y, float z) {
	return(X.is_on(x) && Y.is_on(y) && Z.is_on(z));
}

// Get interval
interval_t space_interval_t::get_var_value(int i) {
  switch (i) {
  case 0:
    return(X);
    break;
  case 1:
    return(Y);
    break;
  case 2:
    return(Z);
    break;
  }
  return(X);    /// Hack to satsify compiler.
}

void space_interval_t::get_corners(vector_t *lower, vector_t *upper) {
	lower->x = X.get_lower();
	lower->y = Y.get_lower();
	lower->z = Z.get_lower();
	upper->x = X.get_upper();
	upper->y = Y.get_upper();
	upper->z = Z.get_upper();
}
