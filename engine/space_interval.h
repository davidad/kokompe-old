using namespace std;
#include <iostream>


#include "interval.h"
#include "vector.h"

#ifndef SPACE_INTERVAL_T
#define SPACE_INTERVAL_T

class space_interval_t {
private:


 public:
  interval_t X;
  interval_t Y;
  interval_t Z;

  space_interval_t();
  space_interval_t(interval_t& X_interval, interval_t& Y_interval, interval_t& Z_interval);
  void split(int i);
  int overlaps(const space_interval_t &b) const;
  void set_point(float x, float y, float z);
  interval_t get_var_value(int i);
  int get_zone(float x, float y, float z);
  int is_on(float x, float y, float z);
  void set(float xmin, float xmax, float ymin, float ymax, float zmin, float zmax);
  void get_corners(vector_t *lower, vector_t *upper);
  void set_to_center(const space_interval_t &a);

  friend ostream& operator<<(ostream &s, const space_interval_t &si);
 
};

ostream& operator<<(ostream &s, const space_interval_t &si);

#endif


