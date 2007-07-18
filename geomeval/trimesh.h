/* Triangulated Mesh

   Represention of solid 3-D object(s) by triangulated mesh(es).
   The points defining the mesh points are stored centrally, so that 
   moving a mesh point in one triangle automatically updates the other
   triangles. 

   Trimesh has facility to keep track of the anchor points (voxel centers) 
   originally used to create the triangle
*/

#ifndef TRIMESH_H
#define TRIMESH_H

#include "vector.h"
#include "interval.h"
#include "space_interval.h"
#include "octree.h"
#include <list>
using namespace std;

class trimesh_node_t {
public:
	vector_t *verticies[3];
	vector_t normal;
	vector_t* interior_point;
	vector_t* exterior_point;
	//trimesh_node_t* neighbors[3];	
	friend ostream& operator<<(ostream &s, const trimesh_node_t &v);
};

ostream& operator<<(ostream &s, const trimesh_node_t &v);

class stl_facet_t {
public:
	vector_t normal;
	vector_t vertex_1;
	vector_t vertex_2;
	vector_t vertex_3;
};


class trimesh_t {
private:
	// Class members defining the trimesh iteself
	list<trimesh_node_t*> triangles;		
	list<vector_t*> verticies;				

	// Class members that provide context about the trimesh
	list<vector_t*> vertex_inside_point;
	list<vector_t*> vertex_outside_point;
	int num_triangles;


	// Class members used to populate the trimesh from voxel grid
	octree_t *octree;
	int nx, ny, nz;
	float xstep, ystep, zstep, zhorizon, xstart, ystart, zstart;
	float x, y, z;
	/*voxel_table is a slice of the voxelated object, at the
	previous (0), current (1), and next (2) Z slice
	voxel_center_table is a table of pointers to vector_t objects
	marking the center of each voxel in the above slice
	vertex_table is a table of pointers to vector_t objects
	marking the corners (used as vertices of triangles) in the
	above vertex table */
	char *voxel_table[3]; 
	vector_t **verticies_table[3];
	vector_t **voxel_centers_table[3];
	
	// Class Methods:
	void initialize_tables();
	void fill_voxels(interval_t X, interval_t Y, int index);
	void triangulate_face(int in_slice, int in_index,
						  int in_ix, int in_iy, int in_iz,
						  int out_slice, int out_index,
						  int out_ix, int out_iy, int out_iz);


public:
	trimesh_t();	// Constructor
	// Create a triangulated mesh from an evaluated octree on a given space interval
	void populate(octree_t* octree, space_interval_t* region, int nx, int ny, int nz);
	// Write an 3D Systems Format Binary STL file from a triangulated mesh
	void write_stl(string filename);

	friend ostream& operator<<(ostream &s, trimesh_t &v);
};

ostream& operator<<(ostream &s, const trimesh_t &v);

#endif


