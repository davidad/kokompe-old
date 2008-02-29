

//#include <stdfx.h>
#include <iostream>
#include <cmath>
using namespace std;


#include "interval.h"

#ifndef M_PI
#define M_PI 3.14159f
#endif

// An interval on the real line

  // Constructor
  // Real Interval
interval_t::interval_t() {
    status = unknown;
    lower = 0.0;
    upper = 0.0;
  }

ostream& operator<< (ostream &s, const interval_t &i) {
  switch (i.status) {
  case everywhere_false:
    return(s << "False");
    break;
  case everywhere_true:
    return(s << "True");
    break;
  case mixed_boolean:
    return(s << "Mixed");
    break;
  case real_interval:
    return(s << "[" << i.lower << " : " << i.upper << "]");
    break;
  case real_number:
    return(s << i.lower);
    break;
  }
  return(s << "Invalid!");
}

//foo
// Interval algrebra functions
// Returns the interval of possible output values given the input

// See, e.g. http://interval.louisiana.edu/Moores_early_papers/Boche_operational.pdf
// (although our handling of the concept of interval boolean operations is different

// For now, does not allow arithmetic operations on boolean data, 
// although this would be easy to add if it were needed !!!!!

interval_t interval_t::add(const interval_t& a, const interval_t& b) {
  interval_t temp;
  if (a.is_real_number() && b.is_real_number()) {
    temp.set_real_number(a.lower + b.lower);
  }
  else if ((a.is_real_interval() || a.is_real_number()) &&
	  (b.is_real_interval() || b.is_real_number())) {
    temp.set_real_interval(a.lower + b.lower, a.upper + b.upper);        
  }
  return(temp);
}

interval_t interval_t::sub(const interval_t &a, const interval_t& b) {
  interval_t temp;
  if (a.is_real_number() && b.is_real_number()) {
    temp.set_real_number(a.lower - b.lower);
  }
  else if ((a.is_real_interval() || a.is_real_number()) &&
	  (b.is_real_interval() || b.is_real_number())) {
    temp.set_real_interval(a.lower - b.upper, a.upper - b.lower);        
  }
  return(temp);
}

interval_t interval_t::mul(const interval_t &a, const interval_t& b) {
  interval_t result;
  float c[4];
  float min = (float)HUGE, max = -(float)HUGE;
  int i;

  // Number multiplication
  if (a.is_real_number() && b.is_real_number()) {
    result.set_real_number(a.lower * b.lower);
    return(result);
  }

  // Interval multiplication
  else if ((a.is_real_interval() || a.is_real_number()) &&
	  (b.is_real_interval() || b.is_real_number())) {
    
    c[0] = a.lower*b.lower;
    c[1] = a.lower*b.upper;
    c[2] = a.upper*b.lower;
    c[3] = a.upper*b.upper;
    
    for (i=0; i < 4; i++) {
      if (c[i] > max)
	max = c[i];
      if (c[i] < min)
	min = c[i];
    }
    result.set_real_interval(min, max);        
    return(result);
  }

  // Boolean times real number/interval: 0 if false, interval if true, interval including zero if mixed
  else if (a.is_boolean() && (b.is_real_number() ||  b.is_real_interval())) {
    if (a.is_true())
      result = b;
    else if (a.is_false())
      result.set_real_number(0.0f);
    else if (a.is_mixed()) {
      result = b;
      result.expand_to_include_number(0.0f);
    }
    return(result);
  }
  else if (b.is_boolean() && (a.is_real_number() ||  a.is_real_interval())) {
    return(mul(b,a));
  }

  // Boolean multiplication is AND
  else if (a.is_boolean() && b.is_boolean()) {
    return(bool_and(a,b));
  }
  else {
    return(result);
  }
}
    

interval_t interval_t::div(const interval_t &a, const interval_t& b) {
  interval_t temp;
  interval_t b2;
  static int warning = 1;

  //  cout << "entering divide.\n";

  if ((a.is_real_number()) && (b.is_real_number())) {
    // Scalar / Scalar
    //cout << "scalar/scalar.\n";
    temp.set_real_number(a.lower / b.lower);
  }
  else if ((a.is_real_interval()) && (b.is_real_number())) {
    // Interval / Scalar --> Very common case
    //cout << "inteval/scalar\n";
    b2.set_real_number(1/b.lower);
    temp = mul(a, b2);
    //cout << "a: " << a << " b: " << b << " b2: " << b2 << " temp: " << temp << "\n";  

  }
  else if ((a.is_real_number() || a.is_real_interval()) && b.is_real_interval()) {
    // Division by Interval
    //cout << "/interval.\n";     

    if ((b.lower > 0) || (b.upper < 0)) {
      // Division by intervals not containing zero
      //cout << "not containing zero\n";
      b2.set_real_interval(1/b.upper, 1/b.lower);
      temp = mul(a, b2);
    }
    
    else {
      // Division by intervals containing zero

      // I think there may be some limit analysis
      // that could be done here, but for now just
      // set interval to +/- Inf
      if (warning) {
	cerr << "Division by interval containing zero, setting interval to (-Inf, +Inf)" << endl;
	warning = 0;
      } 
      temp.set_real_interval(-(float)HUGE, +(float)HUGE);
    }
  }
  // cout << "about to return.\n";

  return(temp);  
}


// Less than and greater than:  Real intervals and numbers to boolean
interval_t interval_t::less_than(const interval_t &a, const interval_t &b) {
  interval_t result;

  if (a.is_real_number() && b.is_real_number()) {
    result.set_boolean(a.lower < b.lower);
  }
  else if ((a.is_real_number() || a.is_real_interval()) &&
	  (b.is_real_number() || b.is_real_interval())) {
    if (a.upper < b.lower)
      result.set_boolean(1);
    else if(b.upper < a.lower)
      result.set_boolean(0);
    else
      result.set_mixed();
  }
  return(result);
}
 
interval_t interval_t::greater_than(const interval_t &a, const interval_t &b) {
  interval_t null_interval;

  return(less_than(b,a));
}

interval_t interval_t::greater_than_or_equals(const interval_t &a, const interval_t &b) {
  interval_t null_interval;

  return(bool_or(greater_than(a,b), equals(a,b)));
}

interval_t interval_t::less_than_or_equals(const interval_t &a, const interval_t &b) {
  interval_t null_interval;
  
  return(bool_or(less_than(a,b), equals(a,b)));
}


// Equals: Real numbers to boolean

interval_t interval_t::equals(const interval_t &a, const interval_t &b) {
  interval_t result, temp;

  if (a.is_real_number() && b.is_real_number()) {
    result.set_boolean(a.lower == b.lower);
  }
  else if ((a.is_real_number() || a.is_real_interval()) &&
	  (b.is_real_number() || b.is_real_interval())) {
    
    temp = less_than(a, b);
    if (temp.is_resolved()) 
      result.set_boolean(0);
    else
      result.set_mixed();
  }
  return(result);
}


// Boolean NOT
interval_t interval_t::bool_not(const interval_t &a, const interval_t &b) {
  interval_t result;
  
  if (a.is_boolean()) {
    
    if (a.is_resolved()) {
      result.set_boolean(a.get_boolean() == 0);
    }
    else {
      result.set_mixed();
    }
  }
 
  return(result);

}

// Boolean OR
interval_t interval_t::bool_or(const interval_t &a, const interval_t &b) {
  interval_t result;

  if (a.is_boolean() && b.is_boolean()) {
    
    if (a.is_false())
      return(b);
    if (b.is_false())
      return(a);
    
    if (a.is_true()) {
      result.set_boolean(1);
      return(result);
    }
    if (b.is_true()) {
      result.set_boolean(1);
      return(result);
    }
    result.set_mixed();
    return(result);
  }
  return(result);
}

// Boolean AND
interval_t interval_t::bool_and(const interval_t &a, const interval_t &b) {
  interval_t result;

  if (a.is_boolean() && b.is_boolean()) {
    
    if (a.is_true())
      return(b);
    if (b.is_true())
      return(a);
    
    if (a.is_false()) {
      result.set_boolean(0);
      return(result);
    }
    if (b.is_false()) {
      result.set_boolean(0);
      return(result);
    }
    result.set_mixed();
    return(result);
  }
  return(result);
}

interval_t interval_t::sin(const interval_t &a, const interval_t &b) {
  interval_t result;
  float mod_arg, range, upper, lower;
  float x;
  float y;


  if (a.is_real_number()) {
    result.set_real_number(sinf(a.lower));
  }
  else {
    range = a.upper - a.lower;
    if (range > (2*M_PI)) {
      result.set_real_interval(-1.0f, 1.0f);
    }
    else {
      mod_arg = fmodf(a.lower,2*(float)M_PI);
      
      x = sinf(a.lower);
      y = sinf(a.upper);
      if (x > y) {
	upper = x;
	lower = y;
      }
      else {
	lower = x;
	upper = y;
      }

      if (mod_arg <= (M_PI*0.5f)) {
	if (( (M_PI*0.5f) - mod_arg) < range) {
	  // M_PI/2 is inside range
	  upper = 1.0f;
	}
      }
      if (mod_arg <= (1.5f*M_PI)) {
	if (((1.5f*M_PI) - mod_arg) < range) {
	  // 3*M_PI/2 is inside range
	  lower = -1.0f;
	}
      }
      result.set_real_interval(lower, upper);
    }
  }
  return(result);
}


interval_t interval_t::cos(const interval_t &a, const interval_t &b) {
  interval_t result;
  interval_t offset;
  
  
  if (a.is_real_number()) {
    result.set_real_number(cosf(a.lower));
    return(result);
  }
  else {
    offset.set_real_number((float)M_PI/2);
    return(sin(add(a, offset), offset)); 
  }
}


// Identical to powf(x,y), but checks for some trivial cases 
// and computes them more efficiently
inline float powcheckf(float x, float y) {
  if (y == 2)
    return(x*x);
  else if (y == 1)
    return(x);
  else
    return(powf(x,y));
}

int is_integer(float x) {
  float epsilon = 1e-6;

  if (fabsf( x - roundf(x)) < epsilon)
    return(1);
  else
    return(0);
}

int is_odd(float x) {
  int xint = (int)roundf(x);
  if (xint % 2)
    return(1);
  else
    return(0);
}


interval_t interval_t::power(const interval_t &a, const interval_t &b) {
  interval_t result;
  float x, y;
  int unbounded = 0;
  static int warning1 = 1, warning2 = 1, warning3 = 1, warning4 = 1;
  
  if ( a.is_real_interval()   && (a.lower > a.upper) ) {
    cerr << "entered power with out-of-order interval " << a.lower << " " << a.upper <<" " << a.status << endl;    
  }
  
  if (b.is_real_number()) {
    if (a.is_real_number()) {
      // For purely numerical (non-interval) arguments, just
      // compute the result and be done with it.
      result.set_real_number(powcheckf(a.lower, b.lower));
    }
    else {
      // Interval raised to a power.
      
      float power = fabsf(b.lower);
      
      if (power == 0) {
	// ********************* RAISING TO THE ZEROTH POWER
	
	if (a.lower >= 0) {
	  // any positive number or zero to the zero power is one
	  result.set_real_number(1.0f);
	}
	else if (a.upper < 0) {
	  // negative number to the zero power is infinity
	  result.set_real_number(HUGE);
	}
	else {
	  // interval containing positive and negative numbers
	  // raised to the zero power could be zero OR infinity
	  if (warning1) {
	    cerr << "Interval containing zero raised to the zero power; unbounded. " << endl;
	    warning1 = 0;
	  }
	  unbounded = 1;
	}
      }
      else {
	
	// ******************* FIRST COMPUTE FOR POSITIVE POWER
	
	if (is_integer(power)) {
	  if (is_odd(power)) {
	    // odd integer power -> monotonic
	    result.set_real_interval(powcheckf(a.lower,power), powcheckf(a.upper,power));
	  }     
	  else {
	    // even integer power -> not monotonic, need to consider cases
	    if ((a.lower <= 0) && (a.upper >= 0)) {
	      x = fabsf(a.lower);
	      y = fabsf(a.upper);
	      if (x > y) {
		result.set_real_interval(0, powcheckf(x, power));
	      }
	      else {
		result.set_real_interval(0, powcheckf(y, power));;
	      }
	    }
	    else if (a.upper > 0) {
	      result.set_real_interval(powcheckf(a.lower, power), powcheckf(a.upper, power));
	    }
	    else if (a.lower < 0) {
	      result.set_real_interval(powcheckf(a.upper, power), powcheckf(a.lower, power));
	    }
	  }
	}
	else {
	  // non-integral power
	  if (a.upper > 0) {
	    // if argument is guaranteed positive -> monotonic
	    result.set_real_interval(powf(a.lower, power), powf(a.upper, power));
	  }
	  else {
	    // possibly negative number raised to non-integral power
	    // this could be undefined.  we don't know how to handle this case yet,
	    // so just punt and set to -INF, INF, meaning it could be anything.  This 
	    // will force an actual numerical evaluation.  Also print an error message. 
	    if (warning2) {
	      cerr << "Possibly negative number raised to non-integral power.  Setting interval to (-INF, INF)" << endl;
	      warning2 = 0;
	    }
	    unbounded = 1;	    
	  }
	}
	
	// THEN CHECK TO SEE IF POWER IS NEGATIVE AND RECIPROCAL NEEDED
	
	if (b.lower < 0) {
	  // Negative power -- equiv. to 1/(x^y), so interval divide
	  if ((result.upper < 0) || (result.lower > 0)) {
	    result.set_real_interval(1.0f/result.upper, 1.0f/result.lower);
	  }
	  else {	    
	    if (warning4) {
	      cerr << "Negative power results in division by interval containing zero, setting interval to (-Inf, +Inf)" << endl;
	      warning4 = 0;
	    } 
	    unbounded = 1;
	  }
	}
      }
    }
  }
  else {
    if (warning3) {
      cerr << "Raising a power to a non-constant expression --- not yet supported --- setting interval to (-Inf, Inf)" << endl;
      warning3 = 0;
    }
    unbounded = 1;
  }
  
  
  // IF UNBOUNDED IS SET -- SET INTERVAL TO WHOLE REAL INTERVAL  
  if (unbounded) {
    result.set_real_interval(-(float)HUGE, (float)HUGE);
  }	

  return(result);
    
}    



interval_t interval_t::sqrt(const interval_t &a, const interval_t &b) {
  interval_t result;

  if (a.is_real_number()) {
    result.set_real_number(sqrtf(a.lower));
  }
  else {
    if (a.lower < 0) {
      cout << "Possibly Negative square root " << a.lower << " " << a.upper << "\n";
    }
    else {
      result.set_real_interval(sqrtf(a.lower), sqrtf(a.upper));
    }
  }
  
  return(result);
}

interval_t interval_t::unary_minus(const interval_t &a, const interval_t &b) {
  interval_t result;
  
  if (a.is_real_number()) {
    result.set_real_number(-a.lower);
  }
  else {
    result.set_real_interval(-a.upper, -a.lower);
  }
  return(result);
}


