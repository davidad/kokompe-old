


#include "trimesh.h"
#include "plane.h"
#include <iostream>
#include <fstream>
#include <string>
#include <sys/types.h>
#include <time.h>
#include <cmath>
using namespace std;


#ifdef WIN32
#include <windows.h>
#define M_PI 3.14159f
#ifndef NOGL
#include <gl/gl.h>
#endif
//#include <glut.h>
#else
#ifndef NOGL
#include <GL/glut.h>
#endif
#endif


using namespace std;

/*vector_t& operator=(vector_t& a, const vertex_t& b) {
	a.x = b.x;
	a.y = b.y;
	a.z = b.z;
	return(a);
}*/



// Constructor
trimesh_t::trimesh_t() {
	int i;
	for (i=0; i<3; i++) {
		voxel_table[i] = NULL;
		verticies_table[i] = NULL;
		voxel_centers_table[i] = NULL;
		voxel_centers_from_evaluator[i] = NULL;
	}
	num_triangles = 0;
	next_vertex_number = 0;
}


// Destructor
trimesh_t::~trimesh_t() {
  this->depopulate();
}

/*

// Return the standardized number of an edge given the number
// of its verticies
inline int get_edge_number(int start_vertex, int end_vertex) {
	int lower_vertex, upper_vertex;

	if (start_vertex < end_vertex) {
		lower_vertex = start_vertex;
		upper_vertex = end_vertex;
	}
	else {
		lower_vertex = end_vertex;
		upper_vertex = start_vertex;
	}

	// 0-1 is edge 0
	// 1-2 is edge 1
	// 2-0 is edge 2
	if (lower_vertex == 1) {
		return(1);
	}
	else if (upper_vertex == 1) {
		return(0);
	}
	else {
		return(2);
	}
}

inline int opposite_vertex_from_edge (int edge) {
	return((edge + 2)%3);
}

inline int next_vertex(int vertex) {
	return((vertex + 1)%3);
}
*/


void trimesh_t::add_vertex(vertex_t *vertex, vector_t *inside_point, vector_t *outside_point) {
	verticies.push_back(vertex);
	vertex_inside_point.push_back(inside_point);
	vertex_outside_point.push_back(outside_point);
}

void trimesh_t::add_triangle(vertex_t *v1, vertex_t *v2, vertex_t *v3, vector_t normal) {

//	int your_start_vertex_copy;

	// Used to locate edge neighbors
//	vertex_t *start_vertex, *end_vertex;
//	list<trimesh_node_t*>::iterator triangle_iterator;
//	int my_edge, your_edge, your_start_vertex, your_end_vertex;
		
	trimesh_node_t *node = new trimesh_node_t();

	/*if ((node == (trimesh_node_t*)0x014bda10) ||  (node == (trimesh_node_t*)0x014bfb50) || (node == (trimesh_node_t*)0x014bd408)) {
		cout << "creating suspect vertex.\n";
		cout << node << ":\n";
		cout << v1 << *v1 << "\n";
		cout << v2 << *v2 << "\n";
		cout << v3 << *v3 << "\n";
		

	}*/


		// Populate verticies
		node->verticies[0] = v1;
		node->verticies[1] = v2;
		node->verticies[2] = v3;
		// Add normal
		node->normal = normal;

		// Add references to verticies used-lists
		for (int i=0; i<3; i++) {
			node->verticies[i]->triangle_list.push_back(node);  // add reference to vertex's users list
		}
			

		// Populate edge neighbors
		// For each vertex that starts an edge, look at the list of triangles that
		// use this vertex and see if a different triangle shares the other vertex on 
		// that edge.  If so they are neighbors --- mark as such in both
	/*	for (my_edge=0; my_edge<3; my_edge++) {
			start_vertex = node->verticies[my_edge];
			end_vertex = node->verticies[(my_edge+1)%3];

			// For each vertex neighbor triangle
			for(triangle_iterator = start_vertex->triangle_list.begin(); triangle_iterator != start_vertex->triangle_list.end(); triangle_iterator++) {
				// I am not my own neighbor --- do not consider self
				if ((*triangle_iterator) != node) {
					//This is a different triangle that shares the start_vertex.
					// Check to see if it shares the end_vertex of this edge.
					for (your_end_vertex = 0; your_end_vertex<3; your_end_vertex++) {
						if ((*triangle_iterator)->verticies[your_end_vertex] == end_vertex) {
							// Found a neighbor! 
							
							// Mark the neighbor as an edge
							node->neighbors[my_edge] = (*triangle_iterator);
							
							// Mark myself as an edge in their list
							// First find their number for the start vertex
							for (your_start_vertex=0; your_start_vertex<3; your_start_vertex++) {
								if ((*triangle_iterator)->verticies[your_start_vertex] == start_vertex) {
									your_start_vertex_copy = your_start_vertex;
									break;
								}
							}
							// CAN WE ASSUME THAT YOUR_START_VERTEX STAYS VALID OUTSIDE THE FOR????????????????????????? TODO
							// Convert the pair of vertex numbers to an edge number
							your_edge = get_edge_number(your_start_vertex_copy, your_end_vertex);
							// Write into their data structure
							(*triangle_iterator)->neighbors[your_edge] = node;
						}
					}
				}
			}	
		}*/

		// for each vertex of t1, v1..v3
		//   for each vertex neighbor on v_n
		//     check to see whether it shares another vertex (not v_n) with v1
		//     if so, they are neighbors
		//     mark
		//     carry all the way through, and halt if a second neighbor is found


								// mark on neighbor
						//		your_edge = get_edge_number(


					//	}

		/*int parity;

		for (int v1=0; v1<3; v1++) {
			start_vertex = node->verticies[v1];
			for(triangle_iterator = start_vertex->triangle_list.begin(); triangle_iterator != start_vertex->triangle_list.end(); triangle_iterator++) {

				if ((*triangle_iterator) != node) {
				// I am not my own neighbor!

				for (int v2=0;v2<3; v2++) {

					for (int v3=0; v3<3; v3++) {

						if (((*triangle_iterator)->verticies[v2] == node->verticies[v3]) &&
							((*triangle_iterator)->verticies[v2] != start_vertex)) {

							// Hooray!  We've found a triangle sharing two verticies --- an edge neighbor
								my_edge = get_edge_number(v1,v3);
								parity = 0;
								// mark on node
								if (node->neighbors[my_edge] == NULL) {
									node->neighbors[my_edge] = (*triangle_iterator);
									parity = 1;
								}
								else if (node->neighbors[my_edge] != (*triangle_iterator)) {
									cout << "about to overwrite neighbor array!  trouble!";
								}

								// Find original vertex on this triangle
								for (int v4=0; v4<3; v4++) {
									if ((*triangle_iterator)->verticies[v4] == start_vertex) {

										// found the other vertex
										your_edge = get_edge_number(v2,v4);

										if ((*triangle_iterator)->neighbors[your_edge] == NULL) {
											(*triangle_iterator)->neighbors[your_edge] = node;
											parity = 0;
										}
										else {
											if ((*triangle_iterator)->neighbors[your_edge] != node) {
												cout << "about to overwrite neighbor array on remote.  trouble!";
											}
										}
									}
								}
								if (parity == 1) {
									cout << "wrote neighbor without corresponding neighbor.\n";
								}
						}
					}
				}
			}
		}
		}*/
		

		// Assign the normal 
		//node->normal = normal;
		
		// Add to list
		//cout << "added.\n";
		triangles.push_back(node);
		


		num_triangles++;
	}


// When we discover vertex doubling, this function is used to replace the reference to the
// old vertex with a reference to the new vertex
// IS THIS SUFFICIENT?
void trimesh_t::replace_vertex(vertex_t *oldv, vertex_t *newv) {
	list<trimesh_node_t*>::iterator triangle_iterator;
	list<trimesh_node_t*>::iterator t1;
	list<trimesh_node_t*>::iterator t2;

			// Replace references to the old vertex with the new vertex
			for(triangle_iterator = oldv->triangle_list.begin(); triangle_iterator != oldv->triangle_list.end(); triangle_iterator++) {
				for (int i=0; i<3; i++) {
					if ((*triangle_iterator)->verticies[i] == oldv) {
						(*triangle_iterator)->verticies[i] = newv;
					}
				}
				newv->triangle_list.push_back(*triangle_iterator);
			}

			// Zero out this vertex's triangle list so we know that we can discard it later
			oldv->triangle_list.clear();

/*
			// Now check to see if any of the triangles on this vertex's triangle list are in fact
			// (new) edge neighbors of each other, and if so, mark them as such

			int vcom, v1, v2;

			// Consider triangles in pairs
			for(t1 = newv->triangle_list.begin(); t1 != newv->triangle_list.end(); t1++) {


				vcom = -1;
				// Figure out which vertex of t1 is the one we are merging
				for(int vc=0; vc<3; vc++) {
					if ((*t1)->verticies[vc] == newv) {
						vcom = vc;
						break;
					}
				}

				// DEBUG TRIPWIRE
				if (vcom == -1) {
					cout << "oops!";
				}

				for(t2 = newv->triangle_list.begin(); t2 != newv->triangle_list.end(); t2++) {

					if (*t1 != *t2) {

						// Consider verticies in pairs
						for (v1=0; v1<3; v1++) {
							for (v2=0; v2<3; v2++) {

								// If the verticies are the same
								if ((*t1)->verticies[v1] == (*t2)->verticies[v2]) {
									// and the vertex is not the one we joined them at
									if ((*t1)->verticies[v1] != newv) {
										// Then we have found a possibly new edge in common.  Mark
										int edgeno = get_edge_number(vcom, v1);
										if ((*t1)->neighbors[edgeno] == NULL)
											(*t1)->neighbors[edgeno] = *t2;
										else if ((*t1)->neighbors[edgeno] != (*t2)) {
											cout << "Inconsistent edge numbers found!";
											cout << (*t1) << "\n";
											cout << *(*t1)->verticies[0] << "\n";
											cout << *(*t1)->verticies[1] << "\n";
											cout << *(*t1)->verticies[2] << "\n";

											cout << (*t2) << "\n";
											cout << *(*t2)->verticies[0] << "\n";
											cout << *(*t2)->verticies[1] << "\n";
											cout << *(*t2)->verticies[2] << "\n";




										}


										//(*t1)->neighbors[] = *t2;
									}
								}
							}
						}
					}
				}
			}*/

}

/*
void trimesh_t::check_neighbors() {
	list<trimesh_node_t*>::iterator triangle_iterator;
		list<vertex_t*>::iterator vertex_iterator;

	for(triangle_iterator = triangles.begin(); triangle_iterator != triangles.end(); triangle_iterator++) {

			(*triangle_iterator)->dirty = 0;
		for (int n=0; n<3; n++) {
			if ((*triangle_iterator)->neighbors[n] == NULL) {
				cout << "Null neighbor!\n";
			(*triangle_iterator)->dirty = 1;
			}


		}
	}

	for (vertex_iterator = verticies.begin(); vertex_iterator != verticies.end(); vertex_iterator++) {
		int sz =  (*vertex_iterator)->triangle_list.size();
		if ((sz >0) && (sz <3)) {
			cout << "slightly used vertex!";
	}
	}








}
*/


void trimesh_t::add_voxel_center(vector_t *v) {
	voxel_centers.push_back(v);
}


// merge trimesh a into this trimesh  --- after this it is O.K. to destruct trimesh a w/o problems,
// because all its STL lists are emptied
// TODO: CHECK THAT IT WORKS!
void trimesh_t::merge(trimesh_t &a) {
	triangles.splice(triangles.end(), a.triangles);
	verticies.splice(verticies.end(), a.verticies);
	vertex_inside_point.splice(vertex_inside_point.end(), a.vertex_inside_point);
	vertex_outside_point.splice(vertex_outside_point.end(), a.vertex_outside_point);
	voxel_centers.splice(voxel_centers.end(), a.voxel_centers);
	num_triangles += a.num_triangles;
	a.num_triangles = 0;
}



// Set up the tables used to populate a trimesh
void trimesh_t::initialize_tables() {
	for (int i=0; i<3; i++) {
		voxel_table[i] = new char[nx*ny];
		verticies_table[i] = new vertex_t*[(nx-1)*(ny-1)];
		voxel_centers_table[i] = new vector_t*[nx*ny];
		voxel_centers_from_evaluator[i] = new vector_t[nx*ny];
		
		memset(verticies_table[i], (unsigned char)NULL, (nx-1)*(ny-1)*sizeof(vector_t*));
		memset(voxel_centers_table[i], (unsigned char)NULL, nx*ny*sizeof(vector_t*));
		//memset(voxel_centers_from_evaluator[i], (unsigned char)NULL, nx*ny*sizeof(vector_t));


		/*  No performance difference, but above is more compact 
		
		for (j=0; j< (nx-1)*(ny-1); j++) {
			verticies_table[i][j] = NULL;
		}
		for (j=0; j< nx*ny; j++) {
			voxel_centers_table[i][j] = NULL;
		}	*/
	}		
}

/*void trimesh_t::fill_voxels(interval_t X, interval_t Y, int index) {
	// Z interval increments on each call
	interval_t Z;
	Z.set_real_interval(zhorizon, zhorizon + zstep);
	zhorizon += zstep;
	// Setup slice evaluation region
	space_interval_t slice(X,Y,Z);
	// Evaluate single slice of voxel grid
	octree->eval_on_grid(slice, nx, ny, 1, voxel_table[index],voxel_centers_from_evaluator[index]);

	// Test code to evaluate using eval_on_grid instead to check
	/*
	x = X.get_lower();
	y = Y.get_lower();
	float xstep = (X.get_upper() - X.get_lower()) / (float)nx;
	float ystep = (Y.get_upper() - Y.get_lower()) / (float)ny;
	
	
	for (int xi=0; xi < nx; xi++) {
		
		y = Y.get_lower();
		for (int yi=0; yi < ny; yi++) {
			voxel_table[index][	

		}
	}*/




//}
/*
// Populate a trimesh using an evaluated octree on a given space interval,
// with a rectangular lattice using an underlying voxel grid with
// nx, ny, and nz elements in each direction
void trimesh_t::populate(octree_t* octree, space_interval_t* region, int nx, int ny, int nz) {
	int ix, iy, iz, i, index;
	char *tmp_voxel_table;
	vertex_t **tmp_verticies_table;
	vector_t **tmp_voxel_centers_table;
	vector_t *tmp_voxel_centers_from_evaluator;


	//cout << "nx: " << nx << "ny: " << ny << " nz: " << nz << "\n";

	// Store pointer to octree to evaluate
	this->octree = octree;

	// Set up class variables used for population
	this->nx = nx;
	this->ny = ny;
	this->nz = nz;
	initialize_tables();

	// Set up X and Y evaluation intervals
	interval_t X(region->get_var_value(0));
	interval_t Y(region->get_var_value(1));
	interval_t Z(region->get_var_value(1));
	
	// Set up voxel grid step size
	xstep = X.get_length()/(float)nx;
	ystep = Y.get_length()/(float)ny;
	zstep = Z.get_length()/(float)nz;
	xstart = X.get_lower();
	ystart = Y.get_lower();
	zstart = Z.get_lower();

	// Initialize the Z horizon for the voxel slice evaluation
	zhorizon = Z.get_lower();

	// Initialize the voxel grid with three slices
	for(i=0; i<3; i++) {
		fill_voxels(X,Y,i);
	}

	// Iterate over every voxel except the outside edge, Z direction first
	z = Z.get_lower() + zstep/2.0f;
	for(iz = 1; iz<(nz-1); iz++) {
		z+= zstep;

		y = Y.get_lower() + ystep/2.0f;
		for(iy=1; iy<(ny-1); iy++) {
			y+= ystep;
			
			x = X.get_lower() + xstep/2.0f; 
			for(ix=1; ix<(nx-1); ix++) {
				x+= xstep;
				
				index = ix + nx*iy;		// Linear array index for this voxel

			
				// GIGO
				// Lets see if the voxel table is correct

				///*x = voxel_centers_from_evaluator[1][index].x;
				//y = voxel_centers_from_evaluator[1][index].y;
				//z = voxel_centers_from_evaluator[1][index].z;


				//if (voxel_table[1][index] != octree->eval_at_point(x,y,z)) {
				//	cout << "Voxel table in error.";
				//}

				// If true here true here
				if (voxel_table[1][index] == 1) {
					// But false to the ... then build two triangles
					// ...left (-X)
					if (voxel_table[1][index - 1] == 0) 
						triangulate_face(1, index, ix, iy, iz, 1, index-1, ix-1, iy, iz);							
					// ...right (+X)
					if (voxel_table[1][index + 1] == 0) 
						triangulate_face(1, index, ix, iy, iz, 1, index+1, ix+1, iy, iz);
					// ... backward (-Y)
					if (voxel_table[1][index - nx] == 0)
						triangulate_face(1, index, ix, iy, iz, 1, index-nx, ix, iy-1, iz);
					// ... forward (+Y)
					if (voxel_table[1][index + ny] == 0)
						triangulate_face(1, index, ix, iy, iz, 1, index+nx, ix, iy+1, iz);
					// ... down (-Z)
					if (voxel_table[0][index] == 0)
						triangulate_face(1, index, ix, iy, iz, 0, index, ix, iy, iz-1);
					// ... up (+Z)
					if (voxel_table[2][index] == 0)
						triangulate_face(1, index, ix, iy, iz, 2, index, ix, iy, iz+1);
				}
			}
		}
		
		// If the loop will run again
		if ((iz+1) < (nz-1)) {
			// Roll through voxel grid			

			tmp_voxel_centers_from_evaluator = voxel_centers_from_evaluator[0];
			voxel_centers_from_evaluator[0] = voxel_centers_from_evaluator[1];
			voxel_centers_from_evaluator[1] = voxel_centers_from_evaluator[2];
			voxel_centers_from_evaluator[2] = tmp_voxel_centers_from_evaluator;

			tmp_voxel_table = voxel_table[0];
			voxel_table[0] = voxel_table[1];
			voxel_table[1] = voxel_table[2];
			voxel_table[2] = tmp_voxel_table;
			fill_voxels(X,Y,2);

			tmp_verticies_table = verticies_table[0];
			verticies_table[0] = verticies_table[1];
			verticies_table[1] = verticies_table[2];
			verticies_table[2] = tmp_verticies_table;
			memset(verticies_table[2], (unsigned char)NULL, (nx-1)*(ny-1)*sizeof(vertex_t*));
		
			tmp_voxel_centers_table = voxel_centers_table[0];
			voxel_centers_table[0] = voxel_centers_table[1];
			voxel_centers_table[1] = voxel_centers_table[2];
			voxel_centers_table[2] = tmp_voxel_centers_table;
			memset(voxel_centers_table[2],(unsigned char)NULL, nx*ny*sizeof(vector_t*));

			



		}
		else {
			// All done - Free dynamic memory used for pointer tables		
			for(i=0; i<3; i++) {
				delete []voxel_table[i];
				delete []verticies_table[i];
				delete []voxel_centers_table[i];
				delete []voxel_centers_from_evaluator[i];
			}
		}
	}
}

*/

// Depopulate a trimesh so it can be used again
void trimesh_t::depopulate() {
  list<trimesh_node_t*>::iterator triangle_iterator;
  list<vertex_t*>::iterator vertex_iterator;
  list<vector_t*>::iterator vector_iterator;
  
  for(triangle_iterator = triangles.begin(); triangle_iterator != triangles.end(); triangle_iterator++) 
    delete *triangle_iterator;
  for(vertex_iterator =verticies.begin(); vertex_iterator != verticies.end(); vertex_iterator++) 
    delete *vertex_iterator;
  for(vector_iterator = voxel_centers.begin(); vector_iterator != voxel_centers.end(); vector_iterator++) 
    delete *vector_iterator;

  triangles.clear();
  verticies.clear();
  voxel_centers.clear();
  vertex_inside_point.clear();
  vertex_outside_point.clear();

  num_triangles = 0;
  octree = NULL;

  nx = 0; 
  ny = 0;
  nz = 0;
  
  xstep = 0.0f;
  ystep = 0.0f;
  zstep = 0.0f;
  zhorizon = 0.0f;
  xstart = 0.0f;
  ystart = 0.0f;
  zstart = 0.0f;

  x = 0.0f;
  y = 0.0f;
  z = 0.0f;

  for (int i=0; i<3; i++) {
    voxel_table[i] = NULL;
    verticies_table[i] = NULL;
    voxel_centers_table[i] = NULL;
  }

  next_vertex_number = 0;

}






/*
// Builds two triangles on a rectangular face dividing a voxel inside the
// object from a voxel outside the object
void trimesh_t::triangulate_face(int in_slice, int in_index,
							   int in_ix, int in_iy, int in_iz,
							   int out_slice, int out_index,
							   int out_ix, int out_iy, int out_iz) {
	float x, y, z;
	int xv, yv, zv;
	int index;
	vertex_t *quad[4];
	vector_t normal;
	trimesh_node_t* node;
	int edge_ix, edge_iy, edge_iz, dir, i, edge_slice, t;
	
	// Used to located edge neighbors
	vertex_t *start_vertex, *end_vertex;
	list<trimesh_node_t*>::iterator triangle_iterator;
	int my_edge, your_edge, your_start_vertex, your_end_vertex;
	

	int a[2][4];
	int b[2][4];

	// Order of quadrilateral traversal
	// 0 is a-first, 1 is b-first
	a[0][0] = 0; b[0][0] = 0;
	a[0][1] = 1; b[0][1] = 0;
	a[0][2] = 1; b[0][2] = 1;
	a[0][3] = 0; b[0][3] = 1;

	a[1][0] = 0; b[1][0] = 0;
	a[1][1] = 0; b[1][1] = 1;
	a[1][2] = 1; b[1][2] = 1;
	a[1][3] = 1; b[1][3] = 0;

	float range = 0.501f;

	// Add voxel centers to shared tables
	if (voxel_centers_table[in_slice][in_index] == NULL) {
		//x = xstart + ((float)in_ix + 0.5f)*xstep;
		//y = ystart + ((float)in_iy + 0.5f)*ystep;
		//z = zstart + ((float)in_iz + 0.5f)*zstep;
		voxel_centers_table[in_slice][in_index] = new vector_t(voxel_centers_from_evaluator[in_slice][in_index]);
		voxel_centers.push_back(voxel_centers_table[in_slice][in_index]);
	}
	if (voxel_centers_table[out_slice][out_index] == NULL) {
		//x = xstart + ((float)out_ix+0.5f)*xstep;
		//y = ystart + ((float)out_iy+0.5f)*ystep;
		//z = zstart + ((float)out_iz+0.5f)*zstep;
		voxel_centers_table[out_slice][out_index] = new vector_t(voxel_centers_from_evaluator[out_slice][out_index]);
		voxel_centers.push_back(voxel_centers_table[out_slice][out_index]);
	}

	// Add verticies to shared table, and record pointers to those 
	// verticies for a quadrilateral (rectangle) counterclockwise
	// around the normal vector to the face.


	// If face is normal to the X axis
	if (in_ix != out_ix) {
		// Select min(in_ix,out_ix) --> This is vertex table edge index 
		if (in_ix < out_ix) {
			edge_ix = in_ix;
			dir = 0;  // from 0,0, pos y first points out for in smaller
			normal.set(1.0, 0.0, 0.0);
		}
		else { 
			edge_ix = out_ix;
			dir = 1;
			normal.set(-1.0,0.0,0.0);
		}
		// Add four verticies in a rectangle in the YZ plane
		for(i=0; i<4; i++) {
			yv = a[dir][i];  
			zv = b[dir][i];

			index = edge_ix + (nx-1)*(in_iy + yv-1);			
			if (verticies_table[zv][index] == NULL) {
				x = xstart + (xstep * (edge_ix + 1.0f));// + 10.0f;     
				y = ystart + ystep * (in_iy  + yv);// + 10.0f;   
				z = zstart + zstep * (in_iz  + zv);// + 10.0f;
				verticies_table[zv][index] = new vertex_t(x,y,z, next_vertex_number++);
				(verticies_table[zv][index])->si.set(x-xstep*range, x+xstep*range, y-ystep*range, y+ystep*range,z-zstep*range, z+zstep*range);
				verticies.push_back(verticies_table[zv][index]);
				vertex_inside_point.push_back(voxel_centers_table[in_slice][in_index]);
				vertex_outside_point.push_back(voxel_centers_table[out_slice][out_index]);
			}		
			quad[i] = verticies_table[zv][index];
		}
	}

	// If face is normal to the Y axis
	if (in_iy != out_iy) {
		// Select min(in_iy,ouy_iy) --> This is vertex table edge index 
		if (in_iy < out_iy) {
			edge_iy = in_iy;
			dir = 1;	 
			normal.set(0.0f, 1.0f, 0.0f);
		}
		else { 
			edge_iy = out_iy;
			dir = 0;	// from 0,0, pos X first is in larger
			normal.set(0.0f, -1.0f, 0.0f);
		}
		// Add four verticies in a rectangle in the XZ plane
		for(i=0; i<4; i++) {
			xv = a[dir][i];  
			zv = b[dir][i];

			//cout << "index calc: in_ix:" << in_ix << "xv: " << xv << " nx: " << nx << "edge_iy: " << edge_iy << "\n";
			index = in_ix-1 + xv + (nx-1)*edge_iy;			
			if (verticies_table[zv][index] == NULL) {
				x = xstart + (xstep * (in_ix  + xv));// + 10.0f;     
				y = ystart + ystep * (edge_iy + 1.0f);// + 10.0f;   
				z = zstart + zstep * (in_iz  + zv);// + 10.0f;
				verticies_table[zv][index] = new vertex_t(x,y,z, next_vertex_number++);
				(verticies_table[zv][index])->si.set(x-xstep*range, x+xstep*range, y-ystep*range, y+ystep*range,z-zstep*range, z+zstep*range);

				//cout << "created new entry at " << verticies_table[zv][index] << "\n";
				verticies.push_back(verticies_table[zv][index]);
				vertex_inside_point.push_back(voxel_centers_table[in_slice][in_index]);
				vertex_outside_point.push_back(voxel_centers_table[out_slice][out_index]);
			}				
			quad[i] = verticies_table[zv][index];
			//cout << "quad["<< i << "]=verticies_table["<<zv<<"]["<<index<<"]\n";

		}	
	}

	// If face is normal to the Z axis
	if (in_iz != out_iz) {
		// Select min(in_iz,in_iz) --> This is vertex table edge index 
		if (in_iz < out_iz) {
			edge_iz = in_iz;
			edge_slice = in_slice;
			dir = 0;	// from 0,0, pos x first is in smaller
			normal.set(0.0f, 0.0f, 1.0f);
		}
		else { 
			edge_iz = out_iz;
			edge_slice = out_slice;
			dir = 1;
			normal.set(0.0f, 0.0f, -1.0f);
		}

		// Add four verticies in a rectangle in the XY plane
		for(i=0; i<4; i++) {
			xv = a[dir][i];  
			yv = b[dir][i];

			index = in_ix-1 + xv + (nx-1)*(in_iy + yv-1);			
			if (verticies_table[edge_slice][index] == NULL) {
				x = xstart + xstep * (in_ix  + xv);// + 10.0f;   
				y = zstart + ystep * (in_iy  + yv);// + 10.0f;
				z = zstart + (zstep * (edge_iz + 1.0f));// + 10.0f;      
				verticies_table[edge_slice][index] = new vertex_t(x,y,z, next_vertex_number++);
				(verticies_table[edge_slice][index])->si.set(x-xstep*range, x+xstep*range, y-ystep*range, y+ystep*range,z-zstep*range, z+zstep*range);
				verticies.push_back(verticies_table[edge_slice][index]);
				vertex_inside_point.push_back(voxel_centers_table[in_slice][in_index]);
				vertex_outside_point.push_back(voxel_centers_table[out_slice][out_index]);	
			}				
			quad[i] = verticies_table[edge_slice][index];
		}
	}

	// Write two triangles to tile quadrilateral	
	//cout << "\n";
	//cout << "New quad.\n";
	

	for (t=0; t<4; t+=2) {
		node = new trimesh_node_t();
		// Populate verticies
		for(i=0; i<3; i++) {
			//cout << i << " " << ((i+t) % 4) << " ";
			node->verticies[i] = quad[((i+t) % 4)];
			node->verticies[i]->triangle_list.push_back(node);  // add reference to vertex's users list
			//cout << (node->verticies[i]) << "\n";
		}	
		// Populate edge neighbors
		// For each vertex that starts an edge, look at the list of triangles that
		// use this vertex and see if a different triangle shares the other vertex on 
		// that edge.  If so they are neighbors --- mark as such in both
		for (my_edge=0; my_edge<3; my_edge++) {
			start_vertex = node->verticies[my_edge];
			end_vertex = node->verticies[(my_edge+1)%3];

			// For each vertex neighbor triangle
			for(triangle_iterator = start_vertex->triangle_list.begin(); triangle_iterator != start_vertex->triangle_list.end(); triangle_iterator++) {
				// I am not my own neighbor --- do not consider self
				if ((*triangle_iterator) != node) {
					//This is a different triangle that shares the start_vertex.
					// Check to see if it shares the end_vertex of this edge.
					for (your_end_vertex = 0; your_end_vertex<3; your_end_vertex++) {
						if ((*triangle_iterator)->verticies[your_end_vertex] == end_vertex) {
							// Found a neighbor! 
							
							// Mark the neighbor as an edge
							node->neighbors[my_edge] = (*triangle_iterator);
							
							// Mark myself as an edge in their list
							// First find their number for the start vertex
							for (your_start_vertex=0; your_start_vertex<3; your_start_vertex++) {
								if ((*triangle_iterator)->verticies[your_start_vertex] == start_vertex)
									break;
							}
							// Convert the pair of vertex numbers to an edge number
							your_edge = get_edge_number(your_start_vertex, your_end_vertex);
							// Write into their data structure
							(*triangle_iterator)->neighbors[your_edge] = node;
						}
					}
				}
			}	
		}

		// Assign the normal 
		node->normal = normal;
		
		// Mark the voxel centers table (SLATED FOR REMOVAL --- NOT USED)
		//	node->interior_point = voxel_centers_table[in_slice][in_index];
		//		node->exterior_point = voxel_centers_table[out_slice][out_index];

		//cout << node->normal;
		//cout << *(node->verticies[0]);
		//cout << *(node->verticies[1]);
		//cout << *(node->verticies[2]);


		// Add to list
		//cout << "added.\n";
		triangles.push_back(node);
		


		num_triangles++;
	}
}
*/

// Refines a trimesh so that the verticies lie more exactly on the object and the
// normals are correct
void trimesh_t::refine() {
	list<vertex_t*>::iterator vertex_iterator;
	list<vector_t*>::iterator vertex_inside_point_iterator;
	list<vector_t*>::iterator vertex_outside_point_iterator;

	vector_t start_point;
	vector_t end_point;
	vector_t mid_point;
	int start_value, i;
	int pt_value;

	vertex_inside_point_iterator = vertex_inside_point.begin();
	vertex_outside_point_iterator = vertex_outside_point.begin();

	for (vertex_iterator = verticies.begin(); vertex_iterator != verticies.end(); vertex_iterator++) {
		start_point = **vertex_iterator;
		if (octree->eval_at_point(start_point.x, start_point.y, start_point.z)) {
			// Point is inside the object, so line search from vertex to outside point
			end_point = **vertex_outside_point_iterator;
			start_value = 1;
		}
		else {
			// Point is outside the object, so line search from vertex to inside point
			end_point = **vertex_inside_point_iterator;
			start_value =0;
		}

		// Line search along point for edge by binary search
		for(i=0; i<16; i++) {	 // depth at 8 for the moment
			mid_point = midpoint(start_point, end_point);
			pt_value = octree->eval_at_point(mid_point.x, mid_point.y, mid_point.z);
			if (pt_value == start_value) {
				start_point = mid_point;
			}
			else {
				end_point = mid_point;
			}
		}
		mid_point = midpoint(start_point, end_point);
		(**vertex_iterator).set_vector(mid_point);

		(**vertex_iterator).clause = octree->differential_eval(start_point.x, start_point.y, start_point.z,
															   end_point.x, end_point.y, end_point.z);

		vertex_inside_point_iterator++;
		vertex_outside_point_iterator++;
	}
}
/*
int ratio_test(vertex_t verticies[3], int* bad_vertex) {
	vector_t cross_product;
	float min_ratio = 0.2f;  /// woah!!!
	float sidea, sideb, sidec, fourAsquared, ratiosquared = 0.0;

	 cross_product = cross(sub(verticies[1], verticies[0]), sub(verticies[2], verticies[0]));
	 fourAsquared = dot(cross_product, cross_product);
	 sidea = dot(sub(verticies[1], verticies[0]),sub(verticies[1],verticies[0]));
	 sideb = dot(sub(verticies[2], verticies[1]),sub(verticies[2], verticies[1]));
	 sidec = dot(sub(verticies[0], verticies[2]),sub(verticies[0], verticies[2]));
     	
	 if ((sidea >= sideb) && (sidea >= sidec)) {
		 *bad_vertex = 2;
		 ratiosquared = fourAsquared/(sidea*sidea);
	 }
	 if ((sideb >= sidea) && (sideb >= sidec)) {
		 *bad_vertex = 0;
		 ratiosquared = fourAsquared/(sideb*sideb);
	 }
	 if ((sidec >= sidea) && (sidec >= sideb)) {
		 *bad_vertex = 1;
		 ratiosquared = fourAsquared/(sidec*sidec);
	 }
	if (ratiosquared > (min_ratio*min_ratio))
		return(1);
	else
		return(0);
}
*/














// COMPUTE NORMALS FROM VERTEX POSITIONS

// This is tricky, because the triangulation tends to generate a lot
// of small, sliver triangles with a very small aspect ratio, and these
// get assigned a wildly innacuate normal.  (resulting in speckling,
// improper edge detection/refinement, etc.)  To fix this, when we find
// a low aspect ratio triangle, we use the normal from a neighbor instead

void trimesh_t::recalculate_normals() {
	list<trimesh_node_t*>::iterator triangle_iterator;
	int i;
	vertex_t new_verticies[3];

	for (triangle_iterator = triangles.begin(); triangle_iterator != triangles.end(); triangle_iterator++) {
		// Copy verticies into a local list
		for (i=0; i<3; i++)
			new_verticies[i] = *(*triangle_iterator)->verticies[i];
	
		// Compute normal from cross product of edge vectors
		(**triangle_iterator).normal = normalize(cross(sub(new_verticies[1],new_verticies[0]), sub(new_verticies[2], new_verticies[0])));
	}
}

/*
void trimesh_t::mark_triangles_needing_division() {
	list<vertex_t*>::iterator vertex_iterator;
	list<trimesh_node_t*>::iterator inner_triangle_iterator;
	list<trimesh_node_t*>::iterator outer_triangle_iterator;
	list<trimesh_node_t*> edge_triangles;

	trimesh_node_t* outer_triangle;
	trimesh_node_t* inner_triangle;

	vector_t v;
	int edge;

	float mindot = cos(10*M_PI/180);  // 10 degree max angle between non-edge triangles 

	// For each vertex, check neightboring triangles for a large difference in normal direction
	for (vertex_iterator = verticies.begin(); vertex_iterator != verticies.end(); vertex_iterator++) {
	
		// Check all edge-neighbor triangles for a large difference in angle
			for (outer_triangle_iterator = (*vertex_iterator)->triangle_list.begin(); outer_triangle_iterator != (*vertex_iterator)->triangle_list.end(); outer_triangle_iterator++) {
		
					outer_triangle = *outer_triangle_iterator;
					for (edge = 0; edge < 3; edge++) {
					  if (outer_triangle->neighbors[edge] != NULL) {
						inner_triangle = outer_triangle->neighbors[edge];

						// If both triangles are already marked, no need to check again
						if (!(outer_triangle->dirty && inner_triangle->dirty)) {
						
								if (fabs(dot(outer_triangle->normal, inner_triangle->normal)) < mindot) {
									//cout << "MARKED!" << "\n";

									// If there is a large difference in angle, add to a list and mark dirty
									if (inner_triangle->dirty == 0) {
										inner_triangle->dirty = 1;
										edge_triangles.push_back(inner_triangle);
									}
									if (outer_triangle->dirty == 0) {
										outer_triangle->dirty = 1;
										edge_triangles.push_back(outer_triangle);
									}
									// Also mark the two verticies defining this edge as dirty
									if (edge == 0) {
										outer_triangle->verticies[0]->dirty = 1;
										outer_triangle->verticies[1]->dirty = 1;
									}
									else if (edge == 1) {
										outer_triangle->verticies[1]->dirty = 1;
										outer_triangle->verticies[2]->dirty = 1;
									}
									else {
										outer_triangle->verticies[2]->dirty = 1;
										outer_triangle->verticies[0]->dirty = 1;
									}

								}
							}
					  }
					}
			}
	}
	// dirty triangles plot a different color so I can see if this is working
}
*/

void trimesh_t::mark_triangles_spanning_surfaces() {
	list<vertex_t*>::iterator vertex_iterator;
	list<trimesh_node_t*>::iterator triangle_iterator;

	int clause;
	int dirty_count = 0;
	int unknown_clause_count = 0;

    // Check triangles for verticies resolved by different expression clauses
	for(triangle_iterator = triangles.begin(); triangle_iterator != triangles.end(); triangle_iterator++) {

		clause = (*triangle_iterator)->verticies[0]->clause;

		if  (((*triangle_iterator)->verticies[1]->clause != clause) ||
			 ((*triangle_iterator)->verticies[2]->clause != clause)) {
			(*triangle_iterator)->dirty = 1;
			(*triangle_iterator)->verticies[0]->dirty = 1;
			(*triangle_iterator)->verticies[1]->dirty = 1;
			(*triangle_iterator)->verticies[2]->dirty = 1;
			dirty_count++;
		}

		if (((*triangle_iterator)->verticies[0]->clause == -1) ||
			((*triangle_iterator)->verticies[1]->clause == -1) ||
			((*triangle_iterator)->verticies[2]->clause == -1)) {
				(*triangle_iterator)->has_unknown_clause = 1;
				unknown_clause_count++;
		}
	}
	cerr << "Marked " << dirty_count << " triangles on edges and marked " << unknown_clause_count << " as having unknown clause." << endl;
}

void trimesh_t::move_veticies_onto_edges_and_corners_using_normals() {
	list<vertex_t*>::iterator vertex_iterator;
	list<trimesh_node_t*>::iterator triangle_iterator;
	int clause[3];
	vector_t normals[3];
	vector_t points[3];
	plane_t planes[3];
	vector_t corner;
	vector_t tmp;
	int i;
	float s = 0.001f;
	vector_t edge_point, box_lower, box_upper;
	int corner_count = 0;
	int edge_count = 0;

	//cout << "Building derivative table." << endl;
	// Build the derivative table for the trimesh
	dt.create(*(octree->expression));

	// First do corners
       
	for(triangle_iterator = triangles.begin(); triangle_iterator != triangles.end(); triangle_iterator++) {
		if ((*triangle_iterator)->dirty) {
			for (i=0; i<3; i++) 
				clause[i] = (*triangle_iterator)->verticies[i]->clause;
			if ((clause[0] != clause[1]) && 
				(clause[1] != clause[2]) &&
				(clause[2] != clause[0]) &&
				(clause[0] != -1) &&
				(clause[1] != -1) &&
				(clause[2] != -1)) {
					// We found a corner-spanning triangle!
					//cout << "Found a corner triangle." << endl;

					// Compute the normals to each vertex from the math string derivative.
					// and construct planes through the verticies, tangent to object
					for(i=0; i<3; i++) { 
						normals[i] = dt.evaluate_normal(*(*triangle_iterator)->verticies[i], clause[i]);
						planes[i].set(*(*triangle_iterator)->verticies[i], normals[i]);
					}
					// Find intersection point of planes
					corner = plane_t::three_plane_intersection(planes[0], planes[1], planes[2]);
					// Determine if this corner point overlaps with one of the verticies voronoi regions
					for (i=0; i<3; i++) {
						if ((*triangle_iterator)->verticies[i]->si.is_on(corner.x, corner.y, corner.z)) {
							// yes!  a match!
							// MOVE THE VERTEX TO THE CORNER
							(*triangle_iterator)->verticies[i]->set_vector(corner);
							(*triangle_iterator)->verticies[i]->dirty = 0;
							//cout << "Moved vertex." << endl;
							corner_count++;
							break;
						}
					}
			}
		}
	}


	//cout << "Starting edge pass";
		for(triangle_iterator = triangles.begin(); triangle_iterator != triangles.end(); triangle_iterator++) {
	  if ((*triangle_iterator)->dirty) {
	    
	    
	    // This is written inefficiently --- it computes the vertex normals many times, both for each triangle
	    // and from triangle to triangle.  It might be better to pointer to the vertex normal in the vertex data type.
	    // But lets get this working first...
	    
	    // NOW DO EDGES
	    // For every vertex pair
	    //   If the verticies are on seperate faces
	    //   Compute normals
	    //	 if the normals are not identical, then 
	    //      compute the line of plane intersection
	    //      for every vertex that is dirty
	    //		   check to see the line of plane intersection falls inside the box
	    //		   if, move the vertex onto the line, mark the vertex clean, and move to the next vertex pair
	    //  This algorithim is correct for objects made of planes, and approximately correct for locally planar objects
	    
	    // Compute normal to each vertex on its face
	    for (i=0; i<3; i++) {
	      clause[i] = (*triangle_iterator)->verticies[i]->clause;
	      points[i] = *(*triangle_iterator)->verticies[i];
	      if (clause[i] != -1) {
		normals[i] = dt.evaluate_normal(points[i], clause[i]);
		planes[i].set(points[i], normals[i]);
	      }
	    }					


	    /*	    // New Plan
	    
	    if ((clause[0] != -1) && (clause[1] != -1) && (clause[2] != -1)) {
	     
	      // For each triangle, identify the "odd" vertex on the different face
	      // from the other two.

	      // If all verticies are dirty, move the odd one.
	      // If one of the common pair of verticies is dirty, but not the odd one, move
	      // the dirty vertex on the common pair

	      // Other cases:
	      // If 

	      int odd, pair1, pair2;

	      if ((clause[0] == clause[1]) && (clause[0] != clause[2])) {
		odd = 2;
		pair1 = 0;
		pair2 = 1;
	      }
	      else if ((clause[0] == clause[2]) && (clause[0] != clause[1])) {
		odd = 1;
		pair1 = 0;
		pair2 = 2;
	      }
	      else if ((clause[1] == clause[2]) && (clause[0] != clause[1])) {
		odd = 0;
		pair1 = 1;
		pair2 = 2;
	      }
	      else {
		break;
	      }

	      line_t line = plane_t::two_plane_intersection(planes[odd], planes[pair1]);

	      // If ALL verticies are dirty, move the odd one
	      if (((*triangle_iterator)->verticies[0]->dirty) && 
		  ((*triangle_iterator)->verticies[1]->dirty) && 
		  ((*triangle_iterator)->verticies[2]->dirty)) {
		// Move odd vertex to point nearest line
		line_t::nearest_point_on_line(line, *(*triangle_iterator)->verticies[odd], &edge_point);
		(*triangle_iterator)->verticies[odd]->set_vector(edge_point);
		(*triangle_iterator)->verticies[odd]->dirty = 0;
	      }
	      else if (((*triangle_iterator)->verticies[odd]->dirty) &&
		       ((*triangle_iterator)->verticies[pair1]->dirty) &&
		       (*triangle_iterator)->verticies[pair2]->dirty == 0) {
		// If just pair2 is clean, move pair1 to match
		line_t::nearest_point_on_line(line, *(*triangle_iterator)->verticies[pair1], &edge_point);
		(*triangle_iterator)->verticies[pair1]->set_vector(edge_point);
		(*triangle_iterator)->verticies[pair1]->dirty = 0;
	      }
	      else if (((*triangle_iterator)->verticies[odd]->dirty) &&
		       ((*triangle_iterator)->verticies[pair2]->dirty) &&
		       (*triangle_iterator)->verticies[pair1]->dirty == 0) {
		// If just pair1 is clean, move pair2 to match
		line_t::nearest_point_on_line(line, *(*triangle_iterator)->verticies[pair2], &edge_point);
		(*triangle_iterator)->verticies[pair2]->set_vector(edge_point);
		(*triangle_iterator)->verticies[pair2]->dirty = 0;
	      }
	      // other cases:
	      // pair is clean, odd is in any state --- that's fine
	      // odd is clean, pair is in any state --- that fine
	    }
	  }
	}
	}*/  
	      
							 
	      


	    
	    // For each vertex pair 
	    for (int v1=0; v1<3; v1++) {
	      for (int v2=(v1+1); v2 < 3; v2++) {
		// Check if verticies are on seperate faces and have known faces
		if ((clause[v1] != clause[v2]) &&
		    (clause[v1] != -1) &&
		    (clause[v2] != -1)) {
		  
		  // Test planes for non-parallelness
		  tmp = cross(normals[v1], normals[v2]);
		  if (magnitude(tmp) > s) {
		    // non-parallel planes on differnt known surfaces!
		    line_t line = plane_t::two_plane_intersection(planes[v1], planes[v2]);
		    
		    // Test for intersection of line with the voronoi region of dirty verticies
		    for (int v3=0; v3<3; v3++) {
		      if ((*triangle_iterator)->verticies[v3]->dirty) {
			(*triangle_iterator)->verticies[v3]->si.get_corners(&box_lower, &box_upper);
			if (line_t::line_box_intersection(line, box_lower, box_upper, &edge_point)) {
			  //cout << "MOVING A VERTEX! " << "endl";
			  (*triangle_iterator)->verticies[v3]->set_vector(edge_point);
			  (*triangle_iterator)->verticies[v3]->dirty = 0;
			  edge_count++;
			  //(*triangle_iterator)->dirty = 0;  // once A vertex has been moved, triangle no longer dirty?
			}
		      }
		      
		    }
		  }
		}
	      }
	    }
	    
	  }
	}
	
	cerr << "Moved " << corner_count << " corner verticies and " << edge_count << " edge verticies." << endl;
}





	


#ifdef OLDCODE



// Moves verticies of a trimesh toward the corners and edges of an object.
// Does so by solving the following mathematical program:
//
// max  dot( vertex normal , vector_from_old_position)
//  s.t.   new point is still on object
//		   new point is still inside the voroni tessalation of the vertex's original position
//  AND
//  min  dot( vertex normal , vector_from_old_position)
//  s.t.   new point is still on object
//		   new point is still inside the voroni tessalation of the vertex's original position
// 
// and choose the solution with the fewest "point still on voronoi tessallation" constraints tight

void trimesh_t::move_verticies_toward_corners() {
	list<vertex_t*>::iterator vertex_iterator;
	//list<vector_t*>::iterator vertex_inside_point_iterator;
	//list<vector_t*>::iterator vertex_outside_point_iterator;
	list<trimesh_node_t*>::iterator triangle_iterator;
	float best_cost, current_cost;
	vector_t normal, start, v;
	int xc, yc, zc;
	float xp, yp, zp;
	float xpstep, ypstep, zpstep;
	float xpstart, ypstart, zpstart;
	int best_xc = 0, best_yc = 0 , best_zc = 0;
	vector_t best_v;
	int set_best, outside_corner;
	float stepmul;
	int points, i;
	


	//vertex_inside_point_iterator = vertex_inside_point.begin();
	//vertex_outside_point_iterator = vertex_outside_point.begin();

	for (vertex_iterator = verticies.begin(); vertex_iterator != verticies.end(); vertex_iterator++) {
		//cout << "at top of loop for vertex " << (*vertex_iterator)->number << "\n";
		// Check all triangles at this vertex, to see if any need refinement.	
		//dirty = 0;
		//for (triangle_iterator = (*vertex_iterator)->triangle_list.begin(); triangle_iterator != (*vertex_iterator)->triangle_list.end(); triangle_iterator++) {
	//		if ((*triangle_iterator)->dirty) {
	//			dirty = 1;
	//		}
//		}



		// If vertex might need refinement,
		if ((*vertex_iterator)->dirty) {
			
			// Compute the vertex normal, the average normal vector for all triangles sharing this vertex
			normal.set(0.0f,0.0f,0.0f);
			set_best = 0;
			float centroid_to_object;
			centroid_to_object = 0.0f;
			for (triangle_iterator = (*vertex_iterator)->triangle_list.begin(); triangle_iterator != (*vertex_iterator)->triangle_list.end(); triangle_iterator++) {
				if ((*triangle_iterator)->dirty) 
					normal = add(normal, (*triangle_iterator)->normal);	

				if ((*triangle_iterator)->dirty) 
					centroid_to_object += (*triangle_iterator)->centroid_to_object;


			}

			normal = normalize(normal);
			start = **vertex_iterator;
			
			if (centroid_to_object > 0.0f) 
				outside_corner = 1;
			else 
				outside_corner = 0;
			

			if (outside_corner)
				best_cost = -(float)HUGE;
			else
				best_cost = (float)HUGE;


			/*
			if (outside_corner) {
				xp = (*vertex_inside_point_iterator)->x;
				yp = (*vertex_inside_point_iterator)->y;
				zp = (*vertex_inside_point_iterator)->z;
				best_cost = -(float)HUGE;
			}
			else {
				xp = (*vertex_outside_point_iterator)->x;
				yp = (*vertex_outside_point_iterator)->y;
				zp = (*vertex_outside_point_iterator)->z;
				best_cost = (float)HUGE;
			}


			xmin = ((*vertex_iterator)->si.get_var_value(0)).get_lower();
			ymin = ((*vertex_iterator)->si.get_var_value(1)).get_lower();
			zmin = ((*vertex_iterator)->si.get_var_value(2)).get_lower();
			xmax = ((*vertex_iterator)->si.get_var_value(0)).get_upper();
			ymax = ((*vertex_iterator)->si.get_var_value(1)).get_upper();
			zmax = ((*vertex_iterator)->si.get_var_value(2)).get_upper();

			dx = xstep / 32.0f;
			dy = ystep / 32.0f;
			dz = zstep / 32.0f;


			not_optimal = 1;
			
			if (outside_corner) {
			
				while(not_optimal) {

					// Metropolis algorithim
					// Consider steps in each cardinal direction
					// We are not picky --- take the first one you find

					v.set(xp+dx,yp,zp);
					current_cost = dot(v,normal);
					if ((current_cost > best_cost) &&
						(xp+dx < xmax))	{
						if (octree->eval_at_point(xp+dx,yp,zp)) {
							xp += dx;
							best_cost = current_cost;
							set_best = 1;
							continue;
						}
					}
					v.set(xp-dx,yp,zp);
					current_cost = dot(v,normal);
					if ((current_cost > best_cost) &&
						(xp-dx > xmin))	{
						if (octree->eval_at_point(xp-dx,yp,zp)) {
							xp -= dx;
							best_cost = current_cost;
							set_best = 1;
							continue;
						}
					}
					v.set(xp,yp+dy,zp);
					current_cost = dot(v,normal);
					if ((current_cost > best_cost) &&
						(yp+dy < ymax))	{
						if (octree->eval_at_point(xp,yp+dy,zp)) {
							yp += dy;
							best_cost = current_cost;
							set_best = 1;
							continue;
						}
					}
					v.set(xp,yp-dy,zp);
					current_cost = dot(v,normal);
					if ((current_cost > best_cost) &&
						(yp-dy > ymin))	{
						if (octree->eval_at_point(xp,yp-dy,zp)) {
							yp -= dy;
							best_cost = current_cost;
							set_best = 1;
							continue;
						}
					}
					v.set(xp,yp,zp+dz);
					current_cost = dot(v,normal);
					if ((current_cost > best_cost) &&
						(zp+dz < zmax))	{
						if (octree->eval_at_point(xp,yp,zp+dz)) {
							zp += dz;
							best_cost = current_cost;
							set_best = 1;
							continue;
						}
					}
					v.set(xp,yp,zp-dz);
					current_cost = dot(v,normal);
					if ((current_cost > best_cost) &&
						(zp-dz > zmin))	{
						if (octree->eval_at_point(xp,yp,zp-dz)) {
							zp -= dz;
							best_cost = current_cost;
							set_best = 1;
							continue;
						}
					}

					// Success!
					not_optimal = 0;
					if (set_best) {

						if (((xp + dx > xmax) || (xp - dx < xmin)) && 
							((yp + dy > ymax) || (yp - dy < ymin)) &&
							((zp + dz > zmax) || (zp - dz < zmin))) {
						
							// All constraints tight!  probably not on object boundary, unless degeneracy

						}
						else {
							v.set(xp, yp, zp);
							(*vertex_iterator)->set_vector(v); 
						}
					}
				
				}
			}

			else {

			while(not_optimal) {

					// Metropolis algorithim
					// Consider steps in each cardinal direction
					// We are not picky --- take the first one you find

					v.set(xp+dx,yp,zp);
					current_cost = dot(v,normal);
					if ((current_cost < best_cost) &&
						(xp+dx < xmax))	{
						if (octree->eval_at_point(xp+dx,yp,zp) == 0) {
							xp += dx;
							best_cost = current_cost;
							continue;
						}
					}
					v.set(xp-dx,yp,zp);
					current_cost = dot(v,normal);
					if ((current_cost < best_cost) &&
						(xp-dx > xmin))	{
						if (octree->eval_at_point(xp-dx,yp,zp) == 0) {
							xp -= dx;
							best_cost = current_cost;
							continue;
						}
					}
					v.set(xp,yp+dy,zp);
					current_cost = dot(v,normal);
					if ((current_cost < best_cost) &&
						(yp+dy < ymax))	{
						if (octree->eval_at_point(xp,yp+dy,zp) == 0) {
							yp += dy;
							best_cost = current_cost;
							continue;
						}
					}
					v.set(xp,yp-dy,zp);
					current_cost = dot(v,normal);
					if ((current_cost < best_cost) &&
						(yp-dy > ymin))	{
						if (octree->eval_at_point(xp,yp-dy,zp) == 0) {
							yp -= dy;
							best_cost = current_cost;
							continue;
						}
					}
					v.set(xp,yp,zp+dz);
					current_cost = dot(v,normal);
					if ((current_cost < best_cost) &&
						(zp+dz < zmax))	{
						if (octree->eval_at_point(xp,yp,zp+dz) == 0) {
							zp += dz;
							best_cost = current_cost;
							continue;
						}
					}
					v.set(xp,yp,zp-dz);
					current_cost = dot(v,normal);
					if ((current_cost < best_cost) &&
						(zp-dz > zmin))	{
						if (octree->eval_at_point(xp,yp,zp-dz) == 0) {
							zp -= dz;
							best_cost = current_cost;
							continue;
						}
					}

					// Success!
					not_optimal = 0;
					if (set_best) {

						if (((xp + dx > xmax) || (xp - dx < xmin)) && 
							((yp + dy > ymax) || (yp - dy < ymin)) &&
							((zp + dz > zmax) || (zp - dz < zmin))) {
						
							// All constraints tight!  probably not on object boundary, unless degeneracy

						}
						else {
							v.set(xp, yp, zp);
							(*vertex_iterator)->set_vector(v); 
						}
					}
					
					

			}





			}*/




			

			// I solve this optimization problem by brute force on a 10X finer grid.
			// There is probably a better way to do this (Karmaker interior point alg?)
			
			// Do progressively finer passes over the evaluation region, so this is
			// a breadth-first search rather than a depth-first.  The eval operation
			// dwarfs everything else, so keeping it so we don't (in the average case)
			// have to do it too much is more important than a small number of dot products

			stepmul = 1.0f;
			points = 1;

			for(i=0; i<3; i++) {
			  stepmul *= 0.5f;
			  points *= 2;

			xpstep = xstep * stepmul;
			ypstep = ystep * stepmul;
			zpstep = zstep * stepmul;

			xpstart = ((*vertex_iterator)->si.get_var_value(0)).get_lower() + 0.5f*xpstep;
			ypstart = ((*vertex_iterator)->si.get_var_value(1)).get_lower() + 0.5f*ypstep;
			zpstart = ((*vertex_iterator)->si.get_var_value(2)).get_lower() + 0.5f*zpstep;
			
			
			zp = zpstart;
			for(zc=0; zc<points; zc++) {
				yp = ypstart;
				for (yc=0;yc<points; yc++) {
					xp = xpstart;
					for (xc=0; xc<points; xc++) {
						
			
						v.set(xp,yp,zp);
						//current_cost = dot(sub(v,start), normal);
						current_cost = dot(v,normal);

						if (outside_corner) {
							if (current_cost > best_cost) {
								// see if this point is feasible
								if (octree->eval_at_point(xp,yp,zp)) {
									best_cost = current_cost;
									best_v = v;
									best_xc = xc;
									best_yc = yc;
									best_zc = zc;
									set_best = 1;
								}
							}
						}
						else {
							if (current_cost < best_cost) {
								// see if this point is feasible
								if (octree->eval_at_point(xp,yp,zp) == 0) {
									best_cost = current_cost;
									best_v = v;
									best_xc = xc;
									best_yc = yc;
									best_zc = zc;
									set_best = 1;
								}
							}
						}
						xp+= xpstep;
					}
					yp += ypstep;
				}
				zp += zpstep;
			}

			}
			
			


			if (set_best &&
					(((best_xc != 0) && (best_xc != (points-1))) ||
					 ((best_yc != 0) && (best_yc != (points-1))) ||
					 ((best_zc != 0) && (best_zc != (points-1))))) {
					(*vertex_iterator)->set_vector(best_v); 
			//		cout << "moved vertex from " << (*vertex_iterator)->number << start << " to " << max_v << "\n";
			}


			// otherwise, either the vertex and corner are coincident with the voxel grid
			// (e.g. degenerate constraints) or there is no corner here, or the normal direction was
			// set wrong
			
			// I think coincident corners are placed correctly anyway (!) so I'm going
			// to just leave the point alone in this case and see what happens

			// oops...this also fails for exactly parallel surfaces to the voxel grid
			// which could happen a lot..lets fix that later

				}
			//	vertex_inside_point_iterator++;
			//	vertex_outside_point_iterator++;

			}

		
	}
	
#endif



// Display a trimesh (text)
ostream& operator<<(ostream &s, trimesh_t &v) {   
   list<trimesh_node_t*>::iterator triangle_iterator;
   list<vertex_t*>::iterator vertex_iterator;

   s << "Triangles:\n";

   for (triangle_iterator = v.triangles.begin(); triangle_iterator != v.triangles.end(); triangle_iterator++) {
     s << **triangle_iterator << "\n";
   }
   
   s << "Verticies:\n";
   for (vertex_iterator = v.verticies.begin(); vertex_iterator != v.verticies.end(); vertex_iterator++) {
	   s << *vertex_iterator << " ";
	   s << **vertex_iterator << "\n";
   }

   return(s);

}


ostream& operator<<(ostream &s, const trimesh_node_t &v) {
	int i;

	s << "Verticies: ";
	for (i=0; i<3; i++) {
		//s << (v.verticies[i]) << " ";
    	s << *(v.verticies[i]) << " ";
	}
	s << "Normal: " << v.normal << " ";
//	s << "Interior Point: " << *(v.interior_point) << " ";
//	s << "Exterior Point: " << *(v.exterior_point) << " ";
	s << "\n";

	return(s);
}





void trimesh_t::fill_stl(char **buffer, int *buffer_size) {
  list<trimesh_node_t*>::iterator triangle_iterator;
  stl_facet_t stl_facet;
  unsigned int num_facets = (unsigned int)num_triangles;  
  unsigned short attr_byte_count = 0;
  char *ptr;

  // Binary STL file format is described in
  // Automated Fabrication, by Marshall Burns, Ch.6
  // and at http://www.ennex.com/~fabbers/StL.asp
  
  // Compute size of stl file in bytes
  *buffer_size = 80 + 4 + num_triangles*(sizeof(stl_facet_t)+sizeof(unsigned short));

  
  // Allocate memory buffer
  *buffer = new char[*buffer_size];
  
  ptr = *buffer;
  

  // Write header
  char header[80] = "Created by Fab Server 0.1";
  
  memcpy(ptr, header, 80);
  
  ptr += 80;
  

  // Write number of triangles to STL data
  memcpy(ptr, (char*)&num_facets, 4);
  
  ptr += 4;
  
  // Write triangles to STL data
  for (triangle_iterator = triangles.begin(); triangle_iterator != triangles.end(); triangle_iterator++) {
  
    stl_facet.normal = (*triangle_iterator)->normal;
    stl_facet.vertex_1 = *((*triangle_iterator)->verticies[0]);
    stl_facet.vertex_2 = *((*triangle_iterator)->verticies[1]);
    stl_facet.vertex_3 = *((*triangle_iterator)->verticies[2]);
    
    stl_facet.vertex_1.x += 10.0f;
    stl_facet.vertex_1.y += 10.0f;
    stl_facet.vertex_1.z += 10.0f;
    
    stl_facet.vertex_2.x += 10.0f;
    stl_facet.vertex_2.y += 10.0f;
    stl_facet.vertex_2.z += 10.0f;
    
    stl_facet.vertex_3.x += 10.0f;
    stl_facet.vertex_3.y += 10.0f;
    stl_facet.vertex_3.z += 10.0f;
        
  
    memcpy(ptr, (char*)&stl_facet, sizeof(stl_facet_t));
  
    ptr += sizeof(stl_facet_t);
  
    memcpy(ptr, (char*)&attr_byte_count, sizeof(unsigned short));
  
    ptr += sizeof(unsigned short);
  
  }
  

}




void trimesh_t::drawgl() {

#ifndef NOGL


	vector_t v1, v2, v3,n;

	// Draws a trimesh to an already-set-up OpenGL window

    list<trimesh_node_t*>::iterator triangle_iterator;
	
	// Set up material color and light reflection properties
   GLfloat mat_ambient[] = { (GLfloat)0.2f, (GLfloat)0.2f, (GLfloat)0.2f, (GLfloat)1.0f };
   GLfloat mat_specular[] = { (GLfloat)0.1f, (GLfloat)0.1f, (GLfloat)0.1f, (GLfloat)1.0f };
   GLfloat mat_diffuse[] = { (GLfloat)0.0f, (GLfloat)0.0f, (GLfloat)0.7f, (GLfloat)1.0f };
   glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient);
   glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
   glMaterialf(GL_FRONT, GL_SHININESS, 50.0);
   glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);

	for (triangle_iterator = triangles.begin(); triangle_iterator != triangles.end(); triangle_iterator++) {
		n = (*triangle_iterator)->normal;
		v1 = *((*triangle_iterator)->verticies[0]);
		v2 = *((*triangle_iterator)->verticies[1]);
		v3 = *((*triangle_iterator)->verticies[2]);
		
		if ((*triangle_iterator)->has_unknown_clause) {
			GLfloat mat_diffuse[] = { 0.0, 0.7,0.0, 1.0 };
			glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
	
		}
		else if ((*triangle_iterator)->dirty) {
			GLfloat mat_diffuse[] = { 0.7, 0, 0, 1.0 };
			glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
		}

	
		else {
			GLfloat mat_diffuse[] = { 0.0f, 0.0f, 0.7f, 1.0f };
			glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
		}
	
		glBegin(GL_TRIANGLES);  
			glNormal3f(n.x, n.y, n.z);
			glVertex3f(v1.x, v1.y, v1.z);                                 
			glVertex3f(v2.x, v2.y,v2.z);                                
			glVertex3f(v3.x, v3.y, v3.z); 
		glEnd();		
	}

#endif
}

void trimesh_t::write_stl(string filename) {
  
  // Binary STL file format is described in
  // Automated Fabrication, by Marshall Burns, Ch.6
  // and at http://www.ennex.com/~fabbers/StL.asp
   
  fstream stl_file (filename.c_str(), ios::binary | ios::out | ios::trunc);
  char header[80] = "Created by geomeval 0.1";
  list<trimesh_node_t*>::iterator triangle_iterator;
  stl_facet_t stl_facet;
  unsigned int num_facets = (unsigned int)num_triangles;  
  unsigned short attr_byte_count = 0;

  if (stl_file.fail()) {
    cout << "Error opening STL output file.\n";
    return;
  }
  else {
    //cout << "writing STL file.\n";
    stl_file.write(header, 80);
    stl_file.write((char*)&num_facets, 4);

    //stl_facet.abc0 = 0;
    //stl_facet.abc1 = 0;

    //cout << "sizeof char " << sizeof(char) << "\n";
    //cout << "sizeof vector_t " << sizeof(vector_t) << "\n";
    //cout << "sizeof float " << sizeof(float) << "\n";


    for (triangle_iterator = triangles.begin(); triangle_iterator != triangles.end(); triangle_iterator++) {
      stl_facet.normal = (*triangle_iterator)->normal;
      stl_facet.vertex_1 = *((*triangle_iterator)->verticies[0]);
      stl_facet.vertex_2 = *((*triangle_iterator)->verticies[1]);
      stl_facet.vertex_3 = *((*triangle_iterator)->verticies[2]);

      stl_facet.vertex_1.x += 10.0f;
      stl_facet.vertex_1.y += 10.0f;
      stl_facet.vertex_1.z += 10.0f;

      stl_facet.vertex_2.x += 10.0f;
      stl_facet.vertex_2.y += 10.0f;
      stl_facet.vertex_2.z += 10.0f;

      stl_facet.vertex_3.x += 10.0f;
      stl_facet.vertex_3.y += 10.0f;
      stl_facet.vertex_3.z += 10.0f;



      stl_file.write((char*)&stl_facet, sizeof(stl_facet_t));
      stl_file.write((char*)&attr_byte_count, sizeof(unsigned short));
      //cout << "wrote facet size " << sizeof(stl_facet_t) + sizeof(unsigned short) << "\n";
      
    }
    
    stl_file.close();
    //cout << "closed file.\n";
    
  }

}
