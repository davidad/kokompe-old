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

#ifdef WIN32
#include <windows.h>
#include <gl/gl.h>
#endif

class trimesh_node_t;

class vertex_t : public vector_t {
public:
	int number;
	space_interval_t si;
	int dirty;
	list<trimesh_node_t*> triangle_list;
	vertex_t(float x, float y, float z)
		: vector_t(x,y,z),
		number(-1) {		
	}
	vertex_t(float ax, float ay, float az, int a_number) {
		x=ax;
		y=ay;
		z=az;
		number=a_number;
		dirty = 0;
	}
	vertex_t() {
	}
	vertex_t(const vector_t& base)
		: vector_t(base) {
	}
	
	void set_vector(const vector_t& a) {
		x = a.x;
		y = a.y;
		z = a.z;
	}
};



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


class trimesh_node_t {
public:
	vertex_t *verticies[3];
	trimesh_node_t *neighbors[3];    // edge neighbors of this triangle
	vector_t normal;
	vector_t* interior_point;
	vector_t* exterior_point;
	int dirty;	
	float centroid_to_object;

	static int repair_triangles(trimesh_node_t* triangle_a, trimesh_node_t* triangle_b);
    static int divide_triangle(trimesh_node_t* triangle1, trimesh_node_t* triangle2,trimesh_node_t * triangle3,
											 vertex_t* vertex,
											 octree_t* octree,
											float search_distance);

	friend ostream& operator<<(ostream &s, const trimesh_node_t &v);

	trimesh_node_t() {
		dirty = 0;
		centroid_to_object = 0;
		for (int i=0; i<3; i++) {
			neighbors[i] = NULL;
		}
	}
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
	list<vertex_t*> verticies;				

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
	vertex_t **verticies_table[3];
	vector_t **voxel_centers_table[3];
	int next_vertex_number;

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
	void drawgl();
	void remove_splinters();
	void refine();
	void edge_refine();
	void recalculate_normals();
	void mark_triangles_needing_division();
	void trimesh_t::divide_triangles();
	void trimesh_t::move_verticies_toward_corners();
	void trimesh_t::add_centroid_to_object_distance(); 

	friend ostream& operator<<(ostream &s, trimesh_t &v);
};

ostream& operator<<(ostream &s, const trimesh_t &v);

#endif


