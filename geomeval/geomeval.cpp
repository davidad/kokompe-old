
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
 
#ifdef WIN32  
  RedirectIOToConsole();
#endif
  
  // Define the evaluation interval for the expression  
  interval1.set_real_interval(-1, 1);
  space_interval_t si(interval1, interval1, interval1);
 
  // Convert an infix string (for a circle) to a postfix string 
 //infix = "X**2 + Y**2 + Z**2 < 0.5";

  infix = "(((((((0 + (X-0)/(0.75 + 0.5*cos(-1.57079632679 + (1.57079632679--1.57079632679)*(Z--0.5)/(0.5--0.5))))-0)**2 + ((0 + (Y-0)/(0.75 + 0.5*cos(-1.57079632679 + (1.57079632679--1.57079632679)*(Z--0.5)/(0.5--0.5))))-0)**2 <= 0.5**2) & (Z >= -0.5) & (Z <= 0.5)) | (((0.35 - sqrt(((Z-0)-0)**2 + ((0 + ((X--0.6)-0)/0.75)-0)**2))**2 + ((0 + ((Y-0)-0)/3)-0)**2) <= 0.05**2) | ((((0.55 - sqrt((X-1.05)**2 + (Z--0.1)**2))**2 + (Y-0)**2) <= 0.1**2)) & (((X >= -1.5) & (X <= 0.9) & (Y >= -1.5) & (Y <= 1.5) & (Z >= 0) & (Z <= 1.5)))) & ~(((((0 + (X-0)/(0.75 + 0.5*cos(-1.57079632679 + (1.57079632679--1.57079632679)*(Z--0.5)/(0.5--0.5))))-0)**2 + ((0 + (Y-0)/(0.75 + 0.5*cos(-1.57079632679 + (1.57079632679--1.57079632679)*(Z--0.5)/(0.5--0.5))))-0)**2 <= 0.4**2) & (Z >= -0.4) & (Z <= 0.6)))) & ~(((((0.55 - sqrt((X-1.05)**2 + (Z--0.1)**2))**2 + (Y-0)**2) <= 0.075**2)) & (((X >= -1.5) & (X <= 0.9) & (Y >= -1.5) & (Y <= 1.5) & (Z >= 0) & (Z <= 1.5))))) & ~(((X >= 0) & (X <= 1.5) & (Y >= -1.5) & (Y <= 0) & (Z >= -1.5) & (Z <= 1.5)))";



  cout << "Converting Infix to Postfix...\n";
  postfix = convert_infix_to_postfix(infix);
  
  // Display infix and postfix string 
  //cout << "Infix: '" << infix << "'\n";
  //cout << "Postfix: '" << postfix << "'\n";
  
  // Construct an expression object from the postfix strin
  // This is stored as an abstract syntax tree
  cout << "Creating Abstract Syntax Tree...\n";
  expression_t ex(postfix);
   
  // Construct an octree evaluation context object from the 
  // expression and evaluation interval  
  octree_t octree(ex, si); 

  // Evaluate and prune the expression on the orctree to
  // a specified recursion depth    
  cout << "Evaluating Expression on Octree...\n";
  octree.eval(7);

  /*
  // Pull out the stored expression value in the octree region
  // containing a specified point (True, False, or Mixed)
  //x = 0.1f; y = 0.0f; z = 0.0f;
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
  //char * results = new char[1600];
  //octree.eval_on_grid(si, 40, 40, 1, results);

  // Show the results of the grid evaluation
  //int index = 0, i, j;
  //for (j=0; j<40; j++) {
  //  for (i=0; i<40;i++) {
  //    cout << (int)results[index] << " ";
   //   index++;
   // }
   // cout << "\n";
  //}

  
  // Run eval at point over grid to test
	float xp, yp;
  for(xp = -1;xp < 1; xp+=0.05) {
	  for (yp = -1;yp<1; yp+=0.05) {
			cout << octree.eval_at_point(xp, yp, 0) << " ";
	  }
	  cout << "\n";
  }
*/





  //delete results;
 
  //vector_t foo;
  //vector_t bar(1,2,3);
  //foo.set(3,4,5);
  cout << "Triangulating Object...\n";
  trimesh_t trimesh; 
  trimesh.populate(&octree, &si, 200, 200, 200);
 cout << "Refining Triangulation...\n";
  trimesh.refine();
	
 // trimesh.remove_splinters();
  //

  cout << "Marking triangles needing further refinement...\n";
  trimesh.mark_triangles_needing_division();

	cout << "computing centroid to object distance for these triangle...\n";
	trimesh.add_centroid_to_object_distance();

//  for(int i=0; i<4; i++) {
	cout << "moving verticies toward corners and edges...\n";
	trimesh.move_verticies_toward_corners();

//	trimesh.refine();
 // }
	cout << "recalculating normals...\n";
trimesh.recalculate_normals();

	cout << "writing STL file...\n";
 trimesh.write_stl("stlfile2.stl");
  //cout << trimesh;

#ifdef WIN32
	cout << "showing graphics...\n";
  show_graphics_window(hInstance, trimesh);
#endif

  return(0);

}
