#include <iostream>
using namespace std;

#ifndef INTERVAL_T
#define INTERVAL_T

const int everywhere_false=0;
const int everywhere_true=1;
const int mixed_boolean=2;
const int real_interval=3;
const int real_number=4;
const int unknown = 5;


class interval_t {
private:
  float lower;
  float upper;
  int status;

 public:
  interval_t();
  void set_real_interval(float lower_in, float upper_in);
  void set_real_number(float value_in);
  float get_center() const;
  void set_boolean(int val);
  int get_boolean() const;
  void set_mixed();
  int is_resolved() const;
  void split(int i);
  int is_real_interval() const;
  int is_real_number() const;
  int is_boolean() const;
  int is_true() const;
  int is_false() const;
  int is_mixed() const;
  int interval_t::is_unknown() const;
  float get_upper() const;
  float get_lower() const;
  int overlaps(const interval_t &b) const;
  void expand_to_include_number(float x);
  float get_length();
  int is_on(float number);

  static interval_t add(const interval_t &a, const interval_t &b);
  static interval_t sub(const interval_t &a,const interval_t &b);
  static interval_t mul(const interval_t &a,const interval_t &b);
  static interval_t div(const interval_t &a,const interval_t &b);
  static interval_t less_than(const interval_t &a,const interval_t &b);
  static interval_t greater_than(const interval_t &a,const interval_t &b);
  static interval_t equals(const interval_t &a,const interval_t &b); 
  static interval_t less_than_or_equals(const interval_t &a,const interval_t &b);
  static interval_t greater_than_or_equals(const interval_t &a,const interval_t &b);

  static interval_t bool_not(const interval_t &a,const interval_t &b);
  static interval_t bool_or(const interval_t &a, const interval_t &b);
  static interval_t bool_and(const interval_t &a, const interval_t &b);

  static interval_t sin(const interval_t &a, const interval_t &b);
  static interval_t cos(const interval_t &a, const interval_t &b);
  static interval_t power(const interval_t &a, const interval_t &b); 
  static interval_t sqrt(const interval_t &a, const interval_t &b);
  static interval_t unary_minus(const interval_t &a, const interval_t &b);
  

  friend ostream& operator<<(ostream &s, const interval_t &i);


};

ostream& operator<<(ostream &s, const interval_t &i);


inline float interval_t::get_length() {
	return(upper - lower);
}

inline  void interval_t::set_real_interval(float lower_in, float upper_in) {    
    if (lower_in > upper_in) {
      printf("Out of order interval!!!");
      exit(1);
    }
		  
    
    lower = lower_in;
    upper = upper_in;
    status = real_interval;
  }
inline  void interval_t::set_real_number(float value_in) {
    lower = value_in;
    upper = value_in; 
    status = real_number;
  }

inline  float interval_t::get_center() const {
    return((lower+upper)*0.5f);
  }

inline float interval_t::get_upper() const {
  return(upper);
}

inline float interval_t::get_lower() const {
  return(lower);
}
  
inline int interval_t::overlaps(const interval_t &b) const {
  return( ((b.lower >= lower) && (b.lower <= upper)) || ((lower >= b.lower) && (lower <= b.upper)) );
}


  // careful...
inline  void interval_t::set_boolean(int val) {
    status = val;
  }
inline  int interval_t::get_boolean() const {
    return(status);
  }

inline int interval_t::is_true() const {
  if (status == everywhere_true)
    return(1);
  else
    return(0);
}

inline int interval_t::is_false() const {
  if (status == everywhere_false)
    return(1);
  else
    return(0);
}

inline int interval_t::is_unknown() const {
  if (status == unknown)
    return(1);
  else
    return(0);
}



inline int interval_t::is_mixed() const {
  if (status == mixed_boolean)
    return(1);
  else
    return(0);
}



inline  void interval_t::set_mixed() {
    status = mixed_boolean;
  }
   
  // Check an interval to see if its value is known
  // (Returns true for known true/false, returns
  // false for a real interval or unknown
inline  int interval_t::is_resolved() const {
    if ((status == everywhere_false) ||
	(status == everywhere_true) ||
	(status == real_number))
      return(1);
    else
      return(0);	
  }

  // Divide a real interval in half (specify 0 half or 1 half)
inline  void interval_t::split(int i) {
    float half_length = (upper - lower)*0.5f;
    
    if (status == real_interval) {
      
      if (i == 0) {
	upper = lower + half_length;
      }
      else {
	lower = upper - half_length;
      }
    }
  }

inline  int interval_t::is_real_interval() const {
    if (status == real_interval)
      return(1);
    else
      return(0);
  }
inline  int interval_t::is_real_number() const {
    if (status == real_number)
      return(1);
    else
      return(0);
  } 

inline  int interval_t::is_boolean() const {
    if ((status == everywhere_false) || 
	(status == everywhere_true) ||
	(status == mixed_boolean))
      return(1);
    else
      return(0);
  }
   
  // expand an interval to include 
inline  void interval_t::expand_to_include_number(float x) {
    if (upper < x)
      upper = x;
    else if (lower > x)
      lower = x;    
    status = real_interval;
  }

inline int interval_t::is_on(float number) {
	if ((number >= lower) && (number <= upper))
		return(1);
	else
		return(0);
}



#endif


