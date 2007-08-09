#include "trimesh.h"
#include <iostream>
#include <fstream>
#include <string>
#include <sys/types.h>
#include <cmath>
using namespace std;

#ifdef WIN32
#include <windows.h>
#include <gl/gl.h>
#define M_PI 3.14159f
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
	}
	num_triangles = 0;
	next_vertex_number = 0;
}


// Set up the tables used to populate a trimesh
void trimesh_t::initialize_tables() {
	int i,j;
	for (i=0; i<3; i++) {
		voxel_table[i] = new char[nx*ny];
		verticies_table[i] = new vertex_t*[(nx-1)*(ny-1)];
		voxel_centers_table[i] = new vector_t*[nx*ny];
		for (j=0; j< (nx-1)*(ny-1); j++) {
			verticies_table[i][j] = NULL;
		}
		for (j=0; j< nx*ny; j++) {
			voxel_centers_table[i][j] = NULL;
		}	
	}		
}

void trimesh_t::fill_voxels(interval_t X, interval_t Y, int index) {
	// Z interval increments on each call
	interval_t Z;
	Z.set_real_interval(zhorizon, zhorizon + zstep);
	zhorizon += zstep;
	// Setup slice evaluation region
	space_interval_t slice(X,Y,Z);
	// Evaluate single slice of voxel grid
	octree->eval_on_grid(slice, nx, ny, 1, voxel_table[index]);
}


// Populate a trimesh using an evaluated octree on a given space interval,
// with a rectangular lattice using an underlying voxel grid with
// nx, ny, and nz elements in each direction
void trimesh_t::populate(octree_t* octree, space_interval_t* region, int nx, int ny, int nz) {
	int ix, iy, iz, i, index;
	
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

				// For each voxel:

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
			delete voxel_table[0];
			voxel_table[0] = voxel_table[1];
			voxel_table[1] = voxel_table[2];
			voxel_table[2] = new char[nx*ny];
			fill_voxels(X,Y,2);

			delete verticies_table[0];
			verticies_table[0] = verticies_table[1];
			verticies_table[1] = verticies_table[2];
			verticies_table[2] = new vertex_t*[(nx-1)*(ny-1)];
			
			for (i=0; i< (nx-1)*(ny-1); i++) {
				verticies_table[2][i] = NULL;
			}
		
			delete voxel_centers_table[0];
			voxel_centers_table[0] = voxel_centers_table[1];
			voxel_centers_table[1] = voxel_centers_table[2];
			voxel_centers_table[2] = new vector_t*[nx*ny];
			for (i=0; i< nx*ny; i++) {
				voxel_centers_table[2][i] = NULL;
			}	
			//cout << "DID ROLLOVER\n";
		}
		else {
			// All done - Free dynamic memory used for pointer tables		
			for(i=0; i<3; i++) {
				delete verticies_table[i];
				delete voxel_centers_table[i];
			}
		}
	
	}
}



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


	// Add voxel centers to shared tables
	if (voxel_centers_table[in_slice][in_index] == NULL) {
		x = xstart + ((float)in_ix + 0.5f)*xstep;
		y = ystart + ((float)in_iy + 0.5f)*ystep;
		z = zstart + ((float)in_iz + 0.5f)*zstep;
		voxel_centers_table[in_slice][in_index] = new vector_t(x, y, z);
	}
	if (voxel_centers_table[out_slice][out_index] == NULL) {
		x = xstart + ((float)out_ix+0.5f)*xstep;
		y = ystart + ((float)out_iy+0.5f)*ystep;
		z = zstart + ((float)out_iz+0.5f)*zstep;
		voxel_centers_table[out_slice][out_index] = new vector_t(x, y, z);
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
				(verticies_table[zv][index])->si.set(x-xstep*0.5f, x+xstep*0.5f, y-ystep*0.5f, y+ystep*0.5f,z-zstep*0.5f, z+zstep*0.5f);
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
				(verticies_table[zv][index])->si.set(x-xstep*0.5f, x+xstep*0.5f, y-ystep*0.5f, y+ystep*0.5f,z-zstep*0.5f, z+zstep*0.5f);

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
				(verticies_table[edge_slice][index])->si.set(x-xstep*0.5f, x+xstep*0.5f, y-ystep*0.5f, y+ystep*0.5f,z-zstep*0.5f, z+zstep*0.5f);
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
		node->interior_point = voxel_centers_table[in_slice][in_index];
		node->exterior_point = voxel_centers_table[out_slice][out_index];

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

	float l;
	float lattice = 2.0f/30.0f;

	vertex_inside_point_iterator = vertex_inside_point.begin();
	vertex_outside_point_iterator = vertex_outside_point.begin();


	for (vertex_iterator = verticies.begin(); vertex_iterator != verticies.end(); vertex_iterator++) {
		start_point = **vertex_iterator;
		if (octree->eval_at_point(start_point.x, start_point.y, start_point.z)) {
			// Point is inside the object, so line search from vertex to outside point
			end_point = **vertex_outside_point_iterator;
			
	

			
			start_value = 1;
			//cout << "FOUND POINT INSIDE OBJECT.\n";
		}
		else {
			// Point is outside the object, so line search from vertex to inside point
			end_point = **vertex_inside_point_iterator;
			start_value =0;
		}
		//cout << "start_value: " << start_value << "\n";
					l = sqrtf(dot(sub(end_point, start_point), sub(end_point, start_point)));
			if (l > (lattice * sqrtf(3.0)/2.0)) {
				cout << "Error!  Moving a point " << l << "which is greater than maximum move of " << lattice*sqrtf(3.0f)/2.0f << "\n";
			}
		
		pt_value = octree->eval_at_point(end_point.x, end_point.y, end_point.z);
	//	cout << "endpoint value: " << pt_value << "start point value: " << start_value << "\n";

		// Line search along point for edge by binary search
		//cout << "New Pt";
		//cout << start_point << end_point << "\n";

		for(i=0; i<7; i++) {	 // depth at 8 for the moment
			//cout << "Iter " << i << "\n";

			mid_point = midpoint(start_point, end_point);

			//cout << "Mid point: " << mid_point << "\n";
			pt_value = octree->eval_at_point(mid_point.x, mid_point.y, mid_point.z);
			//cout << "pt_value: " << pt_value << "\n";
			if (pt_value == start_value) {
				start_point = mid_point;
			}
			else {
				
				end_point = mid_point;
				//cout << "MOVED ENDPOINT.\n";
			}
			//cout << "start: " << start_point << "end: " << end_point << "\n";

		}
		mid_point = midpoint(start_point, end_point);

		//cout << "using " << mid_point << "\n";

		(**vertex_iterator).set_vector(mid_point);

		//cout << "wrote " << **vertex_iterator << "\n";


		vertex_inside_point_iterator++;
		vertex_outside_point_iterator++;
	}
	
	this->recalculate_normals();
}

int ratio_test(vertex_t verticies[3], int* bad_vertex) {
	vector_t cross_product;
	float min_ratio = 0.2f;  /// woah!!!
	float sidea, sideb, sidec, fourAsquared, ratiosquared;

	 cross_product = cross(sub(verticies[1], verticies[0]), sub(verticies[2], verticies[0]));
	 fourAsquared = dot(cross_product, cross_product);
	 sidea = dot(sub(verticies[1], verticies[0]),sub(verticies[1],verticies[0]));
	 sideb = dot(sub(verticies[2], verticies[1]),sub(verticies[2], verticies[1]));
	 sidec = dot(sub(verticies[0], verticies[2]),sub(verticies[0], verticies[2]));
     	
	 if ((sidea > sideb) && (sidea > sidec)) {
		 *bad_vertex = 2;
		 ratiosquared = fourAsquared/(sidea*sidea);
	 }
	 if ((sideb > sidea) && (sideb > sidec)) {
		 *bad_vertex = 0;
		 ratiosquared = fourAsquared/(sideb*sideb);
	 }
	 if ((sidec > sidea) && (sidec > sideb)) {
		 *bad_vertex = 1;
		 ratiosquared = fourAsquared/(sidec*sidec);
	 }
	if (ratiosquared > (min_ratio*min_ratio))
		return(1);
	else
		return(0);
}

// Takes two edge-neighbor triangles, joins them into a quadrilateral,
// and then splits them the opposite way.  This is used to repair degenerate
// triangles and to remove very small sliver triangles.
int trimesh_node_t::repair_triangles(trimesh_node_t* triangle_a, trimesh_node_t* triangle_b) {
	int i,j, edge_a = -1, edge_b = -1, dummy;
	vertex_t* quad_verticies[4];
	trimesh_node_t* quad_neighbors[4];
	trimesh_node_t* old_quad_neighbor[4];
	trimesh_node_t* new_quad_neighbor[4];
	vertex_t verticies_a[3];
	vertex_t verticies_b[3];
	vector_t normal_a, normal_b;
	int vertex_a[3];
	int vertex_b[3];


	// Get the common edge
	for (i=0; i<3; i++) {
		if ((triangle_a->neighbors[i]) == triangle_b) 
			edge_a = i;
		if ((triangle_b->neighbors[i]) == triangle_a)
			edge_b = i;
	}

	// If these triangles are not edge neighbors, return without doing anything
	if ((edge_a == -1) || (edge_b == -1))
		return(1);

	// The verticies for quadrilateral, in order, are:
	// opposite vertex of a from edge
	// next vertex of a
	// opposite vertex of b from edge
	// next vetex of a

	vertex_a[0] = opposite_vertex_from_edge(edge_a);
	vertex_a[1] = next_vertex(vertex_a[0]);
	vertex_a[2] = next_vertex(vertex_a[1]);
	vertex_b[0] = opposite_vertex_from_edge(edge_b);
	vertex_b[1] = next_vertex(vertex_b[0]);
	vertex_b[2] = next_vertex(vertex_b[1]);

	quad_verticies[0] = triangle_a->verticies[vertex_a[0]];
	quad_verticies[1] = triangle_a->verticies[vertex_a[1]];
	quad_verticies[2] = triangle_b->verticies[vertex_b[0]];
	quad_verticies[3] = triangle_b->verticies[vertex_b[1]];

	// The edge neighbors for the quadrilateral come from calling
	// get edge on the vertex number list...
	
	quad_neighbors[0] = triangle_a->neighbors[get_edge_number(vertex_a[0], vertex_a[1])];
	quad_neighbors[1] = triangle_b->neighbors[get_edge_number(vertex_b[2], vertex_b[0])];
	quad_neighbors[2] = triangle_b->neighbors[get_edge_number(vertex_b[0], vertex_b[1])];
	quad_neighbors[3] = triangle_a->neighbors[get_edge_number(vertex_a[2], vertex_a[0])];

	// Test resultant triangles to make sure they both pass the ratio test, and their
	// normal angle is not too different.  If not, do not do this. (This catches the case of
	// non-convex quadrilaterals) 

	verticies_a[0] = *quad_verticies[0];
	verticies_a[1] = *quad_verticies[1];
	verticies_a[2] = *quad_verticies[2];
	verticies_b[0] = *quad_verticies[2];
	verticies_b[1] = *quad_verticies[3];
	verticies_b[2] = *quad_verticies[0];

	if (ratio_test(verticies_a, &dummy) && ratio_test(verticies_b, &dummy)) {

		normal_a = normalize(cross(sub(verticies_a[1],verticies_a[0]), sub(verticies_a[2], verticies_a[0])));
		normal_b = normalize(cross(sub(verticies_b[1],verticies_b[0]), sub(verticies_b[2], verticies_b[0])));

		// If triangles have the same normal to within 90 degrees...
		if (dot(normal_a, normal_b) > 0) {
		// Write them out.

			old_quad_neighbor[0] = triangle_a;
			old_quad_neighbor[1] = triangle_b;
			old_quad_neighbor[2] = triangle_b;
			old_quad_neighbor[3] = triangle_a;


			// Now split the quadrilateral into two triangles, overwriting the old triangles

			// Update Verticies
			triangle_a->verticies[0] = quad_verticies[0];
			triangle_a->verticies[1] = quad_verticies[1];
			triangle_a->verticies[2] = quad_verticies[2];

			triangle_b->verticies[0] = quad_verticies[2];
			triangle_b->verticies[1] = quad_verticies[3];
			triangle_b->verticies[2] = quad_verticies[0];

			// Update Neighbors
			triangle_a->neighbors[0] = quad_neighbors[0];
			triangle_a->neighbors[1] = quad_neighbors[1];
			triangle_a->neighbors[2] = triangle_b;

			triangle_b->neighbors[0] = quad_neighbors[2];
			triangle_b->neighbors[1] = quad_neighbors[3];
			triangle_b->neighbors[2] = triangle_a;
	
			// Update our neigbors as to their new neighbors
			new_quad_neighbor[0] = triangle_a;
			new_quad_neighbor[1] = triangle_a;
			new_quad_neighbor[2] = triangle_b;
			new_quad_neighbor[3] = triangle_b;

			for (j=0; j<4; j++) {
				for(i=0; i<3; i++) {
					if (quad_neighbors[j]->neighbors[i] == old_quad_neighbor[j]) {
						quad_neighbors[j]->neighbors[i] = new_quad_neighbor[j];
						break;
					}
				}
			}
			return(0);
		}
		else {
			return(1);
		}
	}
	else {
		return(1);
	}
}








// Mesh refinement is (theoretically) guaranteed not to change the topology of the mesh,
// but it does produce triangles with a very large base-to-height ratio, which cause problems
// in normal calculation.  Also (I think because of floating-point round-off errors) there are
// sometimes very, very small height triangles with verticies just inside their neighbor that put folded-over
// kinks in the mesh and cause problems for the STL output.

// This method goes through the mesh and removes triangles with a very large base-to-height ratio, 
// merging them with their base edge neighbor and then dividing the resulting quadrilateral into two
// different triangles.  

// It looks like we may want to run multiple passes of this, since there are some bad triangles that
// do not have a good triangle for a neighbor!

void trimesh_t::remove_splinters() {
	list<trimesh_node_t*>::iterator triangle_iterator;
	int edge, dummy, i, j=0, k = 0;
	vertex_t new_verticies[3];

	cout << "starting to desplinter.\n";

	for (triangle_iterator = triangles.begin(); triangle_iterator != triangles.end(); triangle_iterator++) {
		// Copy verticies into a local list
		for (i=0; i<3; i++)
			new_verticies[i] = *(*triangle_iterator)->verticies[i];
	
		// Do ratio test
		if (!ratio_test(new_verticies, &dummy)) {	
			// If this triangle fails the test, search edge neighbors
			// for one that passes
			j++;
			for(edge = 0; edge < 3; edge++) {						
				if ((*triangle_iterator)->repair_triangles((*triangle_iterator), (*triangle_iterator)->neighbors[edge]) == 0) {
					k++;
					break;
				}
			}
			// Combine and split these triangles into two non-degenerate triangles

		}
	}




	cout << "try to desplinter " << j << "triangles, suceeded on " << k << "triangles \n";
}


// COMPUTE NORMALS FROM VERTEX POSITIONS

// This is tricky, because the triangulation tends to generate a lot
// of small, sliver triangles with a very small aspect ratio, and these
// get assigned a wildly innacuate normal.  (resulting in speckling,
// improper edge detection/refinement, etc.)  To fix this, when we find
// a low aspect ratio triangle, we use the normal from a neighbor instead

void trimesh_t::recalculate_normals() {
	list<trimesh_node_t*>::iterator triangle_iterator;
	int edge, dummy, i;
	vertex_t new_verticies[3];
	int j = 0;

	for (triangle_iterator = triangles.begin(); triangle_iterator != triangles.end(); triangle_iterator++) {
		// Copy verticies into a local list
		for (i=0; i<3; i++)
			new_verticies[i] = *(*triangle_iterator)->verticies[i];
	
		// Do ratio test
		if (!ratio_test(new_verticies, &dummy)) {	
			// If this triangle fails the test, search edge neighbors
			// for one that passes
			j++;
			for(edge = 0; edge < 3; edge++) {
				for (i=0; i<3; i++) 
					new_verticies[i] = *(*triangle_iterator)->neighbors[edge]->verticies[i];				
				if (ratio_test(new_verticies, &dummy)) 
					break;
			}
		} 
		// Compute normal from cross product of edge vectors
		(**triangle_iterator).normal = normalize(cross(sub(new_verticies[1],new_verticies[0]), sub(new_verticies[2], new_verticies[0])));
	}
	cout << "Recalculate normals found " << j << " sliver triangles.\n";
}


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
	// dirty triangles plot a different color so I can see if this is working
}

// Divide triangle1 into three new triangles, to be stored in triangle1-3.  Place the new vertex in vertex, 
// and make sure the new vertex is on the object in octree
int trimesh_node_t::divide_triangle(trimesh_node_t* triangle1, trimesh_node_t* triangle2,trimesh_node_t * triangle3,
									 vertex_t* vertex,
									 octree_t* octree,
									 float search_distance) {

	vector_t centroid, start_point, end_point, mid_point, normal;
	


	centroid = triangle_centroid(*triangle1->verticies[0], *triangle1->verticies[1], *triangle1->verticies[2]);	
	normal = triangle1->normal;
/*
	int start_val, end_val, i;

	// Find the right normal/antinormal direction to search in
	// See if the centroid is on the object
	if (octree->eval_at_point(centroid.x, centroid.y, centroid.z)) {
		// If so, follow the normal to the boundary
		end_point = add(centroid, mul(normal, search_distance));
		// Is the proposed end point also on the object?



		if (octree->eval_at_point(end_point.x, end_point.y, end_point.z)) {
			// Disaster!  Punt.
			return(1);
		}
		start_val = 1;
		end_val = 0;

	}
	else {
		// If not, follow the antinormal to the boundary
		end_point = add(centroid, mul(normal, -search_distance));
		// Is the proposed end point also not the object?
		if (!(octree->eval_at_point(end_point.x, end_point.y, end_point.z))) {
			// Disaster!  Punt.
			return(1);
		}
		start_val = 0;
		end_val = 1;
	}
	// At this point, we have an end point for our search, and know that the 
	// object boundary lies somewhere in beteeen.  Binary search out the object position
	start_point = centroid;

	for(i=0; i<8; i++) {
		mid_point = midpoint(start_point, end_point);
		if (octree->eval_at_point(mid_point.x, mid_point.y, mid_point.z) == start_val) {
			start_point = mid_point;
		}
		else {
			end_point = mid_point;
		}
	}
	mid_point = midpoint(start_point, end_point);

	// Set the new vertex to the new point on the object
	vertex->set_vector(mid_point);*/

	vertex->set_vector(centroid);

	// Now, build up the new triangle.
	trimesh_node_t t = *triangle1;  // First copy the old triangle

	// You can see how this works by drawing a triangle divided into three around a centroid
	// vertex, and numbering the verticies of the new triangles so that they wind in the
	// same direction as the old triangle and so that (an arbitrary choice) vertex 2 of all
	// the new triangles is at the center point.  (and numbering the triangles so that THEY
	// wind in the same direction as the orig. triangle, again, an arbitrary decision)

	// First do the verticies
	triangle1->verticies[0] = t.verticies[0];
	triangle1->verticies[1] = t.verticies[1];
	triangle1->verticies[2] = vertex;

	triangle2->verticies[0] = t.verticies[1];
	triangle2->verticies[1] = t.verticies[2];
	triangle2->verticies[2] = vertex;

	triangle3->verticies[0] = t.verticies[2];
	triangle3->verticies[1] = t.verticies[0];
	triangle3->verticies[2] = vertex;

	// Then, the edge neighbors
	triangle1->neighbors[0] = t.neighbors[0];
	triangle1->neighbors[1] = triangle2;
	triangle1->neighbors[2] = triangle3;

	triangle2->neighbors[0] = t.neighbors[1];
	triangle2->neighbors[1] = triangle3;
	triangle2->neighbors[2] = triangle1;

	triangle3->neighbors[0] = t.neighbors[2];
	triangle3->neighbors[1] = triangle1;
	triangle3->neighbors[2] = triangle2;

	// Unset dirty bit
	triangle1->dirty = 0;
	triangle2->dirty = 0;
	triangle3->dirty = 0;


	return(0);
}



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
	int dirty;
	float best_cost, current_cost;
	vector_t normal, start, v;
	int xc, yc, zc;
	float xp, yp, zp;
	float xpstep, ypstep, zpstep;
	float xpstart, ypstart, zpstart;
	int index, best_xc, best_yc, best_zc;
	vector_t best_v;
	int set_best, outside_corner;
	float stepmul;
	int points, i;
	float xmin, xmax, ymin, ymax, zmin, zmax;
	int not_optimal;
	float dx, dy, dz;


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
	

void trimesh_t::add_centroid_to_object_distance() {
	list<trimesh_node_t*>::iterator triangle_iterator;
	vector_t centroid, normal, start_point, mid_point, end_point;
	int punt, start_val, i;

	float search_distance = (xstep+ystep+zstep);

	for (triangle_iterator = triangles.begin(); triangle_iterator != triangles.end(); triangle_iterator++) {
		if ((*triangle_iterator)->dirty) {

			// Compute location of centroid


	centroid = triangle_centroid(*(*triangle_iterator)->verticies[0], *(*triangle_iterator)->verticies[1], *(*triangle_iterator)->verticies[2]);	
	normal = (*triangle_iterator)->normal;

	// See if the centroid is on the object
	punt = 0;
	if (octree->eval_at_point(centroid.x, centroid.y, centroid.z)) {
		// If so, follow the normal to the boundary
		end_point = add(centroid, mul(normal, search_distance));
		// Is the proposed end point also on the object?

		if (octree->eval_at_point(end_point.x, end_point.y, end_point.z)) 
			punt = 1;
		
		start_val = 1;
	}
	else {
		// If not, follow the antinormal to the boundary
		end_point = add(centroid, mul(normal, -search_distance));
		// Is the proposed end point also not the object?
		if (!(octree->eval_at_point(end_point.x, end_point.y, end_point.z))) 
			punt = 1;

	
		start_val = 0;
	}
	// At this point, we have an end point for our search, and know that the 
	// object boundary lies somewhere in beteeen.  Binary search out the object position
	
	if (punt == 0) {
	
	start_point = centroid;

	for(i=0; i<8; i++) {
		mid_point = midpoint(start_point, end_point);
		if (octree->eval_at_point(mid_point.x, mid_point.y, mid_point.z) == start_val) {
			start_point = mid_point;
		}
		else {
			end_point = mid_point;
		}
	}
	mid_point = midpoint(start_point, end_point);

	if (start_val)
		(*triangle_iterator)->centroid_to_object = dist(mid_point, centroid);
	else
		(*triangle_iterator)->centroid_to_object = -dist(mid_point, centroid);

	}
				
		}
	}

}


	

void trimesh_t::divide_triangles() {

	// For each triangle marked as needing division (with dirty bit set), 
	// follow the normal or anti-normal from the centroid to the object.  Put a new vertex there,
	// and divide this triangle into three smaller triangles.

	// The normal following is good for two reasons:
	// 1. It keeps the new vertex strictly inside the projection of the old triangle
	//    perpindicular to its plane, which keeps the new mesh valid.
	// 2. It results in something like Newton shooting toward corners and edges,
	//    resulting in better refinement of corners and edges than simply from an
	//    increase in resolution
	list<trimesh_node_t*>::iterator triangle_iterator;
	trimesh_node_t* triangle1;
	trimesh_node_t* triangle2;
	trimesh_node_t* triangle3;
	vertex_t* vertex;
	
	// Select the search distance
	// Giant HACK!!!!  You need to clean up this idea
	// (e.g. maybe by forcing square voxels)
	float search_distance = ((xstep + ystep + zstep ));



	for (triangle_iterator = triangles.begin(); triangle_iterator != triangles.end(); triangle_iterator++)
		if ((*triangle_iterator)->dirty) {
			//cout << "trying to divide a triangle.\n";
			// Insert two new triangles (keeping the triangle list in approx. Z-sorted order)
			triangle1 = *triangle_iterator;
			triangle2 = new trimesh_node_t();
			triangle3 = new trimesh_node_t();
			vertex = new vertex_t();


			// Divide the triangle into two new triangles
			if(triangle1->divide_triangle(triangle1, triangle2, triangle3, vertex, octree, search_distance)) {
				// If triangle division failed, delete the new triangles and vertex
				//cout << "division failed.\n";
				delete triangle2;
				delete triangle3;
				delete vertex;
			}
			else {
				// If we were sucessful, add the new triangles and new vertex to the trimesh
				//cout << "division suceeded.\n";
				triangles.insert(triangle_iterator, triangle2);
				triangles.insert(triangle_iterator, triangle3);
				//triangle_iterator++;
				//triangle_iterator++;
				verticies.push_back(vertex);
				vertex->number = next_vertex_number;
				next_vertex_number++;
			}
		}
}




	




/* Edge Refinement Algorithim:

  TODO:
  What we have works well, except that the edges are all messed up.

  Edge refinement:

  1. When making the trimesh, keep track of edge-neighbors.  Because of the way the
  triangles are made, this should be reasonably straightforward.

  2. Identify triangles with a large angle between their normal and an edge-neighbor's normal.
  
  3. For each of these triangles, compute the center point.  See if is is in the object.  If so, go 
     out in the normal direction.  If not, go in the negative normal direction.  Go in one or two voxel grid
	 units --- there should be object there.  If not, abort and don't worry about this triangle.  If there is,
	 binary line search to find the edge of the object along that vector.  Put a new point there, and convert
	 the existing triangle into three new triangles.  Compute the new normal of each.
  
  3. Repeat the above procedure N times.  This will produce a beautiful multi-resolution refinement of the
     object's edges and points of high curvature.

  ALSO TODO:

  Profile this code.  It looks like you can't do that in MSVC, so do it with gnuprof.
  You may discover craziness.  For example, the stl lists or continuous memory allocation 
  or block filling may be a bad idea.

  Edge detection:

    After the edge refinement procedure, edge-neighbor triangles that still have a large angle between
	them mark a bona-fine edge, or spot of extreme curvature.  Mark it with a black line.

  Vertex detection:

    Verticies shared by two edges are a vertex.  You could mark with a dot. */





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
//		cout << "writing STL file.\n";
		stl_file.write(header, 80);
		stl_file.write((char*)&num_facets, 4);

//		stl_facet.abc0 = 0;
//		stl_facet.abc1 = 0;

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
//			cout << "wrote facet size " << sizeof(stl_facet_t) + sizeof(unsigned short) << "\n";
			
		}
		
		stl_file.close();
//		cout << "closed file.\n";
		
	}

}
void trimesh_t::drawgl() {


#ifdef WIN32

	vector_t v1, v2, v3,n;

	// Draws a trimesh to an already-set-up OpenGL window
	float mcolor[] = { 0.8f, 0.0f, 0.0f, 1.0f };
	float specReflection[] = { 0.8f, 0.8f, 0.8f, 1.0f };

    list<trimesh_node_t*>::iterator triangle_iterator;
	
	// Set up material color and light reflection properties
   GLfloat mat_ambient[] = { 0.2, 0.2, 0.2, 1.0 };
   GLfloat mat_specular[] = { 0.1, 0.1, 0.1, 1.0 };
   GLfloat mat_diffuse[] = { 0, 0, 0.7, 1.0 };
   glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient);
   glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
   glMaterialf(GL_FRONT, GL_SHININESS, 50.0);
   glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);

	for (triangle_iterator = triangles.begin(); triangle_iterator != triangles.end(); triangle_iterator++) {
		n = (*triangle_iterator)->normal;
		v1 = *((*triangle_iterator)->verticies[0]);
		v2 = *((*triangle_iterator)->verticies[1]);
		v3 = *((*triangle_iterator)->verticies[2]);
		
	//	if ((*triangle_iterator)->dirty) {
	//		GLfloat mat_diffuse[] = { 0.7, 0, 0, 1.0 };
	//		glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
	//	}
	//	else {
		GLfloat mat_diffuse[] = { 0, 0, 0.7, 1.0 };
			glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
	//	}
	
		glBegin(GL_TRIANGLES);  
			glNormal3f(n.x, n.y, n.z);
			glVertex3f(v1.x, v1.y, v1.z);                                 
			glVertex3f(v2.x, v2.y,v2.z);                                
			glVertex3f(v3.x, v3.y, v3.z); 
		glEnd();		
	}

#else

	cout << "GL not currently supported in UNIX version on geomeval.\n";
#endif

}


// procedure:  walk octree, generating triangulated mesh.  need to mark
// triangle neightbors so that tree is walkable for corner finding

// generate triangles from voxels on sections of the evaluated voxel
// grid.  This makes it easy to mark most triangles neigbors...just look over.
// Finding boundary triangle neighbors could be tough w/o storage.

// Need to search?  search for triangles without full complement of neighbors?
// Or, do the object one layer at a time, always keeping two full layers, deleting
// a layer just as the third is done.  This means neighboring triangles are
// automatically recorded.

// Identifying non-convex connected regions presents the standard connected
// components problem: initiallly seperate regions may become the joined on a
// new layer.  you can join connected regions by starting them out seperate and
// then merging them...you can do all the triangle connections because the ragged
// edge is fully documented by the layers stored in memory

// writing STL or rendering OpenGL is easy: you iterate through the list

// when writing xy slide, could put a code "2" in ints that are guaranteed not to be edges,
// by being in interior of all 1's and all 0's regions, so the program does not even
// have to check


