using namespace std;
#include <iostream>


#include "interval.h"

#ifndef SPACE_INTERVAL_T
#define SPACE_INTERVAL_T

class space_interval_t {
private:
  interval_t X;
  interval_t Y;
  interval_t Z;

 public:
  space_interval_t();
  space_interval_t(interval_t& X_interval, interval_t& Y_interval, interval_t& Z_interval);
  void split(int i);
  int space_interval_t::overlaps(const space_interval_t &b) const;
  void space_interval_t::set_point(float x, float y, float z);
  interval_t get_var_value(int i);
  int get_zone(float x, float y, float z);
  friend ostream& operator<<(ostream &s, const space_interval_t &si);
 
};

ostream& operator<<(ostream &s, const space_interval_t &si);

#endif


