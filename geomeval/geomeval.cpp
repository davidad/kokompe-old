
#include <string>
#include <iostream>
using namespace std;

#include "octree.h"
#include "expression.h"
#include "space_interval.h"
#include "interval.h"
#include "vector.h"
#include "trimesh.h"




#ifdef WIN32
#include <windows.h>
#include "gui.h"
#include "guicon.h"
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, 
				   LPSTR lpCmdLine, int iCmdShow) {

#else
int main(int argc, char* argv[]) {

#endif


  interval_t interval1, result;
  string infix, postfix;
  float x, y, z;   
  
  RedirectIOToConsole();

  // Define the evaluation interval for the expression  
  interval1.set_real_interval(-1.25, 1.25);
  space_interval_t si(interval1, interval1, interval1);
 
  // Convert an infix string (for a circle) to a postfix string 
  infix = "X < 1";
  postfix = convert_infix_to_postfix(infix);
  
  // Display infix and postfix string 
  cout << "Infix: '" << infix << "'\n";
  cout << "Postfix: '" << postfix << "'\n";
  
  // Construct an expression object from the postfix strin
  // This is stored as an abstract syntax tree
  expression_t ex(postfix);
   
  // Construct an octree evaluation context object from the 
  // expression and evaluation interval  
  octree_t octree(ex, si); 

  // Evaluate and prune the expression on the orctree to
  // a specified recursion depth    
  cout << "Generating Tree...\n";
  octree.eval(4);

  // Pull out the stored expression value in the octree region
  // containing a specified point (True, False, or Mixed)
  x = 0.1f; y = 0.0f; z = 0.0f;
  result = octree.get_value_at_point(x, y, z);
  cout << "Result of get_value at (" << x << "," << y << "," << z << ") is " <<  result << "\n";

  // Force evaluation of the expression at a specific point,
  // using the pruned subexpression stored inside that point's
  // octree region
  int val = octree.eval_at_point(0.1f, 0.0f, 0.0f);
  cout << "Result of eval_at_point at (" << x << "," << y << "," << z << ") is " <<  val << "\n";


  // Evaluate the expression on a rectangular grid.  
  // (This is more efficient than the single point evaluator
  // for large grids.)
  char * results = new char[1600];
  octree.eval_on_grid(si, 40, 40, 1, results);

  // Show the results of the grid evaluation
  int index = 0, i, j;
  for (j=0; j<40; j++) {
    for (i=0; i<40;i++) {
      cout << (int)results[index] << " ";
      index++;
    }
    cout << "\n";
  }

  delete results;
 
  vector_t foo;
  vector_t bar(1,2,3);
  foo.set(3,4,5);
  cout << foo << " " << bar << "\n";

  trimesh_t trimesh;
  trimesh.populate(&octree, &si, 50, 50, 50);
  trimesh.write_stl("stlfile.stl");

 // cout << trimesh;

//	cout << "Hello, world!\n";
#ifdef WIN32
  show_graphics_window(hInstance);
#endif



  return(0);

}
