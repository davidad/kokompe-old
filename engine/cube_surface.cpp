#include "cube_surface.h"
#include "square_bitmap.h"
#include "vector.h"
#include "vertex.h"
using namespace std;


cube_surface_t::cube_surface_t(int data, int x, int y, int z, int size, float stepsize) {
		// constuct a new cube surface 
	for (int i=0; i<6; i++) {
	       surfaces[i] = new square_bitmap_t(data, size);
	 }
this->x = x;
this->y = y; 
this->z = z;
this->stepsize = stepsize;

}


cube_surface_t::~cube_surface_t() {
	for (int i=0; i<6; i++) {
		delete surfaces[i];
	}
}



cube_surface_t::cube_surface_t(cube_surface_t **cubes) {
		// merge eight cubes together into one, keeping only outer surfaces
		// when this is called, the cubes should already have had verticies at their intersections
	    // knitted together, so this can do the merge of verticies any way it wants

		// look at the diagram of how cube faces are numbered to understand this code
	    // (ccw around z axis, with face 0 at y=0, then 4 = zlow and 5 = zhigh
	
		int t;

		t= 5;
		surfaces[t] = new square_bitmap_t(cubes[0]->surfaces[t], cubes[4]->surfaces[t], cubes[1]->surfaces[t], cubes[5]->surfaces[t]);
		t= 4;
		surfaces[t] = new square_bitmap_t(cubes[2]->surfaces[t], cubes[6]->surfaces[t], cubes[3]->surfaces[t], cubes[7]->surfaces[t]);
		t= 3;
		surfaces[t] = new square_bitmap_t(cubes[2]->surfaces[t], cubes[0]->surfaces[t], cubes[6]->surfaces[t], cubes[4]->surfaces[t]);
		t= 1;
		surfaces[t] = new square_bitmap_t(cubes[3]->surfaces[t], cubes[1]->surfaces[t], cubes[7]->surfaces[t], cubes[5]->surfaces[t]);
		t= 0;
		surfaces[t] = new square_bitmap_t(cubes[2]->surfaces[t], cubes[0]->surfaces[t], cubes[3]->surfaces[t], cubes[1]->surfaces[t]);
		t= 2;
		surfaces[t] = new square_bitmap_t(cubes[6]->surfaces[t], cubes[4]->surfaces[t], cubes[7]->surfaces[t], cubes[5]->surfaces[t]);

		// Keep x,y,z of the lower-left cube in the merge -- #2
		x = cubes[2]->x;
		y = cubes[2]->y;
		z = cubes[2]->z;
		stepsize = cubes[2]->stepsize;


}
	

// Converts x,y,z coordinates to cube face, row, column coordinates
void cube_surface_t::cube_address(int x, int y, int z, int maxval, int* face, int *row, int *col) {

	if (y == 0) {
		*face = 0;
		*row = x;
		*col = z;
	}
	else if (y==maxval) {
		*face = 2;
		*row = x;
		*col = z;
	}
	else if (x==0) {
		*face = 3;
		*row = y;
		*col = z;
	}
	else if (x==maxval) {
		*face = 1;
		*row = y;
		*col = z;
	}
	else if (z==0) {
		*face = 4;
		*row = x;
		*col = y;
	}
	else if (z==maxval) {
		*face = 5;
		*row = x;
		*col = y;
	}
}


int cube_surface_t::get_data(int x, int y, int z) {
	// Retreive an element from the surface of a cube
	// meaning at least one of x,y,z has to be equal to 0 or side_length
	// everything is duplicately stored so we can pull from either face for corners and edges
	
	int row=0, col=0, face=0;
	int maxval = surfaces[0]->side_length() - 1;
	cube_address(x,y,z,maxval, &face, &row, &col);
	return(surfaces[face]->get_data(row,col));
}

vertex_t* cube_surface_t::get_vertex(int x, int y, int z, int dx, int dy, int dz) {
	int xp = x+dx;
	int yp = y+dy;
	int zp = z+dz;

	int row=0, col=0, face=0;
	int maxval = surfaces[0]->side_length();
	cube_address(xp,yp,zp,maxval, &face, &row, &col);
	return(surfaces[face]->get_vertex(row, col, 0, 0));
}

void cube_surface_t::set_vertex(vertex_t *vertex, int x, int y, int z, int dx, int dy, int dz) {
	int xp = x+dx;
	int yp = y+dy;
	int zp = z+dz;
    int maxval = surfaces[0]->side_length() ;


	// GET vertex can get from anywhere
	// SET vertex sets it everywhere
	if (yp == 0) {
		surfaces[0]->set_vertex(vertex,xp,zp,0,0);
	}
	if (yp==maxval) {
		surfaces[2]->set_vertex(vertex,xp,zp,0,0);
	}
	if (xp==0) {
		surfaces[3]->set_vertex(vertex,yp,zp,0,0);
	}
	if (xp==maxval) {
		surfaces[1]->set_vertex(vertex,yp,zp,0,0);
	}
	if (zp==0) {
		surfaces[4]->set_vertex(vertex,xp,yp,0,0);
	}
	if (zp==maxval) {
		surfaces[5]->set_vertex(vertex,xp,yp,0,0);
	}
}

vector_t* cube_surface_t::get_point(int x, int y, int z) {
	int row=0, col=0, face=0;
	int maxval = surfaces[0]->side_length() - 1;
	cube_address(x,y,z,maxval, &face, &row, &col);
	return(surfaces[face]->get_point(row, col));
}

void cube_surface_t::set_point(vector_t *point, int x, int y, int z) {
	int xp = x;
	int yp = y;
	int zp = z;
	int maxval = surfaces[0]->side_length() - 1;

	if (yp == 0) {
		surfaces[0]->set_point(point,xp,zp);
	}
	if (yp==maxval) {
		surfaces[2]->set_point(point,xp,zp);
	}
	if (xp==0) {
		surfaces[3]->set_point(point,yp,zp);
	}
	if (xp==maxval) {
		surfaces[1]->set_point(point,yp,zp);
	}
	if (zp==0) {
		surfaces[4]->set_point(point,xp,yp);
	}
	if (zp==maxval) {
		surfaces[5]->set_point(point,xp,yp);
	}
}

void cube_surface_t::set_data(int data, int x, int y, int z) {
	int xp = x;
	int yp = y;
	int zp = z;
	int maxval = surfaces[0]->side_length() - 1;

	if (yp == 0) {
		surfaces[0]->set_data(data,xp,zp);
	}
	if (yp==maxval) {
		surfaces[2]->set_data(data,xp,zp);
	}
	if (xp==0) {
		surfaces[3]->set_data(data,yp,zp);
	}
	if (xp==maxval) {
		surfaces[1]->set_data(data,yp,zp);
	}
	if (zp==0) {
		surfaces[4]->set_data(data,xp,yp);
	}
	if (zp==maxval) {
		surfaces[5]->set_data(data,xp,yp);
	}
}




void cube_surface_t::get_point_coordinates(int x, int y, int z, float *fx, float *fy, float *fz) {
	*fx = ((float)(this->x + x) + 0.5f)*stepsize;
	*fy = ((float)(this->y + y) + 0.5f)*stepsize;
	*fz = ((float)(this->z + z) + 0.5f)*stepsize;
}

void cube_surface_t::get_vertex_coordinates(int x, int y, int z, int dx, int dy, int dz, float *fx, float *fy, float *fz) {
	get_point_coordinates(x,y,z,fx,fy,fz);
	*fx -= 0.5f*stepsize;
	*fy -= 0.5f*stepsize;
	*fz -= 0.5f*stepsize;
	if (dx)
		*fx += stepsize;
	if (dy)
		*fy += stepsize;
	if (dz)
		*fz += stepsize;
}

int cube_surface_t::side_length() {
	return(surfaces[0]->side_length());
}
