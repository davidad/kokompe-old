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
#include "vertex.h"
#include "interval.h"
#include "octree.h"
#include "space_interval.h"
#include "derivative_table.h"
#include <list>
using namespace std;

#ifdef WIN32
#include <windows.h>
#include <gl/gl.h>
#endif

class trimesh_node_t;





/* Triangle convention:

				E2	
		V0			  V2	
          *----------*
           \        /
			\      /		Normal out of the page by right hand rule
		E0	 \    /   E1
			  \  /
			   *
			   V1

   Verticies are stored in a sepeate list, pointed to by each triangle that
   uses them.  Each triangle points to its three edge neighbors, NULL for
   disconnected ends.  */




ostream& operator<<(ostream &s, const trimesh_node_t &v);

class stl_facet_t {
public:
	vector_t normal;
	vector_t vertex_1;
	vector_t vertex_2;
	vector_t vertex_3;
};

class octree_t;

class trimesh_t {
private:
	// Class members defining the trimesh iteself
	list<trimesh_node_t*> triangles;		
	list<vertex_t*> verticies;				

	// Class members that provide context about the trimesh
	list<vector_t*> vertex_inside_point;
	list<vector_t*> vertex_outside_point;
	list<vector_t*> voxel_centers;  // unique list of vertex inside/outside points for depopulate
	int num_triangles;

	// Class members used to compute locations of corners and edges
	derivative_table_t dt;

	// Class members used to populate the trimesh from voxel grid
	//octree_t *octree;
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
	vertex_t **verticies_table[3];
	vector_t **voxel_centers_table[3];
    vector_t *voxel_centers_from_evaluator[3]; 
   
	int next_vertex_number;

	// Class Methods:
	void initialize_tables();
	void fill_voxels(interval_t X, interval_t Y, int index);
	void triangulate_face(int in_slice, int in_index,
						  int in_ix, int in_iy, int in_iz,
						  int out_slice, int out_index,
						  int out_ix, int out_iy, int out_iz);
	

public:
	octree_t *octree;
	trimesh_t();	// Constructor
	~trimesh_t();   // Destructor
	// Create a triangulated mesh from an evaluated octree on a given space interval
	void populate(octree_t* octree, space_interval_t* region, int nx, int ny, int nz);
	void depopulate();
	// Write an 3D Systems Format Binary STL file from a triangulated mesh
	void write_stl(string filename);
	void fill_stl(char **buffer, int *length);
	void fill_kpf(char **buffer, int *length);
	void drawgl();
	void remove_splinters();
	void refine();
	void edge_refine();
	void recalculate_normals();
	void mark_triangles_needing_division();
	void mark_triangles_spanning_surfaces();
	void divide_triangles();
	void move_veticies_onto_edges_and_corners_using_normals();
	void move_verticies_toward_corners();
	void add_centroid_to_object_distance(); 

	void merge(trimesh_t &a);   // Merge another trimesh into this one, possibly duplicating verticies
	void replace_vertex(vertex_t *oldv, vertex_t *newv);
	void add_vertex(vertex_t *vertex, vector_t *inside_point, vector_t *outside_point);
	void add_triangle(vertex_t *v1, vertex_t*v2, vertex_t *v3, vector_t normal);
	void add_voxel_center(vector_t *v);
	void check_neighbors();  // TODO REMOVE
	friend ostream& operator<<(ostream &s, trimesh_t &v);
};

ostream& operator<<(ostream &s, const trimesh_t &v);

#endif


