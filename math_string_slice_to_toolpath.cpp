#include <string>
#include <iostream>
#include <sstream>
#include <cmath>
using namespace std;

#include <stdlib.h>
#include <errno.h>

#include "octree.h"
#include "expression.h"
#include "space_interval.h"
#include "interval.h"
#include "vector.h"
#include "trimesh.h"

#include "tool_path.h"


float max(float a, float b, float c) {
  if ((a >= b) && (a >= c)) 
    return(a);
  else if ((b >= a) && (b >= c))
    return(b);
  else
    return(c);
}

void print_usage() {
  cerr << "Usage: math_string_slice_to_toolpath xmin xmax ymin ymax zlevel min_feature tool_radius\n";
  cerr << "Takes postfix math string on stdin.\n";
  cerr << "Returns ascii generic toolpath on stdout\n";
}


float read_argf(char ** argv, int arg) {
  float temp;

  temp = atof(argv[arg]);
  if (errno) {
    cerr << "Error parsing floating point value '" << argv[arg] << "'\n";
    print_usage();
    exit(2);
  }
  return(temp);
}


   
int main(int argc, char** argv) {
  string math_string;
  string inputline;
  float min_feature;
  float xmin, xmax, ymin, ymax, zmin, zmax;
  float zlevel;
  
  // Read in command line arguments
  if (argc != 8) {
    cerr << "Wrong number of arguments, got " << argc-1 << " expecting 7.\n";
    print_usage();
    exit(1);
  }
  xmin = read_argf(argv, 1);
  xmax = read_argf(argv, 2);
  ymin = read_argf(argv, 3);
  ymax = read_argf(argv, 4);
  zlevel = read_argf(argv, 5);
  min_feature = read_argf(argv, 6);
  float tool_radius = read_argf(argv, 7);


  // Gobble input until EOF is reached
  while (getline(cin, inputline)) {
      math_string += inputline;
  }  

  
  /* Do the work */

  // Compute the required recursion depth and number of points per side
  // Currently the Octree needs to be uniform --- this is a TODO to fix.
  float xlen = xmax-xmin;
  float ylen = ymax-ymin;
  float maxlen = max(xlen, ylen, 0.0f);
  
  if ((xlen <= 0) || (ylen <= 0) || (min_feature <= 0)) {
    cerr << "Must evaluate over a 2-D region with finite precsision.\n";
    exit(3);
  }

  // Compute octree recursion depth and number of points per side
  // We need to make sure each feature is captured by at least two points,
  // and want there to be 2-3 points per direction inside the smallest
  // octree cells
  int recursion_depth = (int)floorf(log2f( maxlen / min_feature ));
  int nx = 3*(int)ceilf(xlen / min_feature);
  int ny = 3*(int)ceilf(ylen / min_feature);

  // HACK!
  // As this is currently written, there is the potential for very slight
  // inaccuracy in the tool path contouring, because the pixels might
  // not be exactly square.  The tool path is more correct as there are
  // more pixels, and this effect is probably smaller than the base inaccuracy
  // due to pixelation anyway, so it isn't worth worrying about too much.
  // Use average of x and y scale factors, which should be almost exactly equal.
  
  float tool_radius_in_lattice_units = tool_radius * (((float)nx / xlen) + ((float)ny / ylen )) * 0.5f;
  

  // Hard-limit the recursion depth 
  if(recursion_depth < 0)
    recursion_depth = 0;
  else if (recursion_depth > 15)
    recursion_depth = 15;

  // Hard-limit the resolution
  if (nx < 10)
    nx = 10;
  else if (nx > 10000)
    nx = 10000;

  if (ny < 10)
    ny = 10;
  else if (ny > 10000)
    ny = 10000;

  //cerr << "nx: " << nx << " ny: " << ny << " recursion_depth: " << recursion_depth << "\n";

  // Add explicit test for on-this-level to math string to 
  // bound octree evaluation to a 2D region
  stringstream str;

  str << "Z " << zlevel << " == " << math_string << " &";
   
  // Set up a finite octree Z interval so that the slice we want is on it
  // but not exactly between two regions (does this work?  HACK!!! )
  zmin = zlevel - 1.0f;
  zmax = zlevel + 2.0f;

  // Create abstract syntax tree
  //cerr << "Creating math string.\n";
  expression_t ex(str.str());

  // Create intervals
  //cerr << "Creating intervals.\n";
  interval_t x,y,z;
  x.set_real_interval(xmin, xmax);
  y.set_real_interval(ymin, ymax);
  z.set_real_interval(zmin, zmax);
  space_interval_t si(x,y,z);

  // Construct and evaluate octree.
  cerr << "Creating octree.\n";
  octree_t octree(ex, si);
  octree.eval(recursion_depth);

  // Evaluate on grid
  cerr << "Evaluating on grid.\n";
  array_2d<char> results(nx,ny);
  si.set(xmin, xmax, ymin, ymax, zlevel, zlevel);
  octree.eval_on_grid(si, results.width, results.height, 1, &results[0]);

  // Compute tool path
  cerr << "Computing tool path..." << endl;
  float max_error_in_lattice_units = 0.75;  // Neil's emperical value, > sqrt(2)/2

  cerr << "Tool radius in lattice units: " << tool_radius_in_lattice_units << "\n";

  tool_path path = compute_tool_path(results, tool_radius_in_lattice_units, max_error_in_lattice_units);
  
  cout << path << endl;
  cerr << "Tool path computed." << endl;

  return(0);  
}
