#ifndef CUBE_SURFACE
#define CUBE_SURFACE

#include "vertex.h"
#include "vector.h"
#include "square_bitmap.h"
using namespace std;

class square_bitmap_t;

// Bitmap over the surface of a cube
class cube_surface_t {

	int x;
	int y; 
	int z; 
	float stepsize;
	void cube_address(int x, int y, int z, int maxval, int* face, int *row, int *col);

public:
	square_bitmap_t* surfaces[6];

	   // set initial cube surface data
	   cube_surface_t(int data, int x, int y, int z, int size, float stepsize);
	   // new cube surface data from 8 old cube surfaces
	   cube_surface_t(cube_surface_t **surfaces);
	   ~cube_surface_t();

	   int side_length();
	   int get_data(int x, int y, int z);
	   void set_data(int data, int x, int y, int z);
	   vector_t* get_point(int x, int y, int z);
	   void set_point(vector_t *point, int x, int y, int z);
	   
	   // access by integer element location in x,y,z + 0/1 vertex coords on voxel cube
	   vertex_t *get_vertex(int x, int y, int z, int dx, int dy, int dz);
	   void set_vertex(vertex_t *vertex, int x, int y, int z, int dx, int dy, int dz);


	   void get_vertex_coordinates(int x, int y, int z, int dx, int dy, int dz, float *fx, float *fy, float *fz);
	   void get_point_coordinates(int x, int y, int z, float *fx, float *fy, float *fz);

};

#endif
