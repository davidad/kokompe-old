/* Square Bitmap

  A bitmap of voxel values, point pointers, and vertex pointers that covers
  a side of a cube.  This type is used in the recursive creation of triangulated
  meshes.  Is is esentially an inner class of cube_surface_t.

*/

#ifndef SQUARE_BITMAP_H
#define SQUARE_BITMAP_H

#include "vector.h"
#include "trimesh.h"

using namespace std;


   // Bitmap over the surface of a square.
   // May convert to a quadtree representation later, but using a
   // simple array for now
class square_bitmap_t {
	   // child order is raster order:  ul, ur, ll, lr
	   

	public:

int *data;
	   vertex_t **verticies;
	   vector_t **points;
	   int s;

	   // Construct a new square bitmap with size identical elements = data
	   square_bitmap_t();
	   square_bitmap_t(int data, int size);
	   square_bitmap_t(square_bitmap_t* a, square_bitmap_t* b, square_bitmap_t* c, square_bitmap_t* d);  // combine four square bitmaps given in raster order
	   ~square_bitmap_t();
	   
	   int get_data(int row, int col);
	   void set_data(int data, int row, int col);
	   vertex_t* get_vertex(int row, int col, int drow, int dcol);
	   vector_t* get_point(int row, int col);
	   void set_vertex(vertex_t *vertex, int row, int col, int drow, int dcol);
	   void set_point(vector_t *point, int row, int col);
	   int side_length();
};

#endif
