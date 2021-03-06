#include <string>
#include <iostream>
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

#ifdef WIN32
#include <io.h>
#include <fcntl.h>
#endif


void print_usage() {
  cout << "Usage: math_string_to_stl xmin xmax ymin ymax zmin zmax min_feature renderlevel\n";
  cout << "Takes postfix math string on stdin.\n";
  cout << "Returns binary STL on stdout.\n";
  cout << "min_feature is minimum feature size, floating point.\n";
  cout << "renderlevel = 0 for block-object, 1 for smooth surfaces, 2 for smooth surfaces and sharp edges";
}


float maxf(float a, float b, float c) {
  if ((a >= b) && (a >= c)) 
    return(a);
  else if ((b >= a) && (b >= c))
    return(b);
  else
    return(c);
}


#ifdef WIN32
float log2f(float x) {
	return(logf(x)/logf(2));
}
#endif


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
  int renderlevel = 2;
  int writeSTL = 0;

  // Figure out STL vs KPF file generation from the invoked filename
  string name;
  name.append(argv[0]);
  string extension = name.substr(name.size()-3, name.size()-1);
  if (!extension.compare("stl")) {
    writeSTL = 1;
  }
  else if (!extension.compare("kpf")) {
    writeSTL = 0;
  }
  else {
    cerr << "This program must be invoked with the name math_string_to_stl or math_string_to_kpf." << endl;
    exit(239);
  }

  // Read in command line arguments
  if (argc <= 7) {
    cerr << "Wrong number of arguments, got " << argc-1 << " expecting 7 or 8.\n";
    print_usage();
    exit(1);
  }
  xmin = read_argf(argv, 1);
  xmax = read_argf(argv, 2);
  ymin = read_argf(argv, 3);
  ymax = read_argf(argv, 4);
  zmin = read_argf(argv, 5);
  zmax = read_argf(argv, 6);
  min_feature = read_argf(argv, 7);
  if (argc >= 9) {
    renderlevel = atoi(argv[8]);
  }

  // Gobble input until EOF is reached
  while (getline(cin, inputline)) {
      math_string += inputline;
  }  

  /* Do the work */

  // Compute the required recursion depth and number of points per side
  // Currently the Octree needs to be uniform --- this is a TODO to fix.
  float xlen = xmax-xmin;
  float ylen = ymax-ymin;
  float zlen = zmax-zmin;
  float maxlen = maxf(xlen, ylen, zlen);
  
  if ((xlen <= 0) || (ylen <= 0) || (zlen <= 0) || (min_feature <= 0)) {
    cerr << "Must evaluate over a 3-D region with finite precsision.\n";
    exit(3);
  }

  // Compute octree recursion depth and number of points per side
  // We need to make sure each feature is captured by at least two points,
  // and want there to be 2-3 points per direction inside the smallest
  // octree cells
  int recursion_depth = (int)floorf(log2f( maxlen / min_feature ));
  int nx = 3*(int)ceilf(xlen / min_feature);
  int ny = 3*(int)ceilf(ylen / min_feature);
  int nz = 3*(int)ceilf(zlen / min_feature);

  // Hard-limit the recursion depth  (change limit for 2D...)
  if(recursion_depth < 0)
    recursion_depth = 0;
  else if (recursion_depth > 10)
    recursion_depth = 10;

  // Hard-limit the resolution
  if (nx < 10)
    nx = 10;
  else if (nx > 10000)
    nx = 10000;

  if (ny < 10)
    ny = 10;
  else if (ny > 10000)
    ny = 10000;

  if (nz < 10)
    nz = 10;
  else if (nz > 10000)
    nz = 10000;

  //cerr << "nx: " << nx << " ny: " << ny << " nz: " << nz << " recursion_depth: " << recursion_depth << "\n";


  // Create abstract syntax tree
  cerr << "Creating math string.\n";
  expression_t ex(math_string);

  // Create intervals
  cerr << "Creating intervals.\n";
  interval_t x,y,z;
  x.set_real_interval((float)xmin,(float)xmax);
  y.set_real_interval((float)ymin,(float)ymax);
  z.set_real_interval((float)zmin,(float)zmax);
  space_interval_t si(x,y,z);

  // Prune the expression right off -- this does things like combine unary minus with 
  // numbers into constants
  int dummy = 0;
  cerr << "Simplifying expression.\n";
  ex.prune(si, 1, &dummy, 0, NULL, 1);
  
  cerr << "Converting syntax tree to syntax graph.\n";
  ex.convert_to_graph();

  cerr << "Marking clause numbers.\n";
  ex.mark_clause_numbers();

  // Construct and evaluate octree.
  cerr << "Creating octree.\n";
  octree_t octree(ex, si);
  octree.create_cache(recursion_depth);
  octree.eval(recursion_depth);

  // Create and evaluate a trimesh
  cerr << "Populating trimesh.\n";
  trimesh_t *trimesh;
  octree.trimesh(&trimesh);
  //  trimesh.populate(&octree,&si, nx, ny, nz);  
  if (renderlevel >= 1) {
    cerr << "Refining trimesh.\n";  
    trimesh->refine();
  
    if (renderlevel >= 2) {
      //cerr << "Marking triangles near edges and corners.\n";
      //trimesh.mark_triangles_needing_division();
      //cerr << "Adding centroid to determine inside from outside.\n";
      //trimesh.add_centroid_to_object_distance();
      cerr << "Marking triangles spanning faces.\n";
      trimesh->mark_triangles_spanning_surfaces();
      cerr << "Moving verticies toward corners.\n";
      trimesh->move_veticies_onto_edges_and_corners_using_normals();
    }  

    if (writeSTL) {
      // KPF files do not contain facet normals -- only STL
      cerr << "Recalculating normals...\n";
      trimesh->recalculate_normals();
    }
  }
  
   char *stl;
   int stl_length;
   
   // Fill STL or KPF data
   #ifdef WIN32
	/* Change translation mode for stdout to BINARY */
	_setmode( _fileno( stdout ), O_BINARY );
   #endif
   
   if (writeSTL) {
     cerr << "Writing STL." << endl;
     trimesh->fill_stl(&stl, &stl_length);
   }
   else {   
     cerr << "Writing KPF.\n";
     trimesh->fill_kpf(&stl, &stl_length);
   }


   // Write STL or KPF data to stdout
   cout.write(stl, stl_length);
   delete []stl;

   delete trimesh;
 
   octree.delete_cache();

  return(0);  
}
