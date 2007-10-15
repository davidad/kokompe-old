#include "eval_geometry.h"

#include <string>
#include <iostream>
using namespace std;

#include "octree.h"
#include "expression.h"
#include "space_interval.h"
#include "interval.h"
#include "vector.h"
#include "trimesh.h"

void eval_geometry(const std::string& infix, trimesh_t& trimesh)
{

  interval_t interval1, result;
  string postfix;

  interval1.set_real_interval(-1, 1);
  // Define the evaluation interval for the expression  
  space_interval_t si(interval1, interval1, interval1);
  postfix = convert_infix_to_postfix(infix);
  
  cout << "Creating Abstract Syntax Tree...\n";
  
  expression_t ex(postfix);
  
  // Construct an octree evaluation context object from the 
  // expression and evaluation interval  
  octree_t octree(ex, si); 

  // Evaluate and prune the expression on the orctree to
  // a specified recursion depth    
  cout << "Evaluating Expression on Octree...\n";
  octree.eval(7);

  cout << "Triangulating Object...\n";

  trimesh.depopulate();
  trimesh.populate(&octree, &si, 200, 200, 200);

  /*
  cout << "Refining Triangulation...\n";
  trimesh.refine();

  cout << "Marking triangles needing further refinement...\n";
  trimesh.mark_triangles_needing_division();

  cout << "computing centroid to object distance for these triangle...\n";
  trimesh.add_centroid_to_object_distance();

  cout << "moving verticies toward corners and edges...\n";
  trimesh.move_verticies_toward_corners();

  cout << "recalculating normals...\n";
  trimesh.recalculate_normals();
  */
}
