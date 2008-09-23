/* Path

   Represention of two-dimensional solid object or toolpath by path in
   two-dimensional space.

   Pas has the facility to keep track of the anchor points (pixel centers) 
   originally used to create the elements of the path
*/

#ifndef PATH_H
#define PATH_H

#include "vector.h"
#include "vertex.h"
#include "interval.h"
#include "space_interval.h"
#include "derivative_table.h"
#include <list>
using namespace std;


class path_node_t {
public:


	vertex_t *verticies[2];
	int type;
	double arc_radius;
	vector_t arc_center;

	static const int LINE = 0;
	static const int ARC = 1;

	path_node_t();
};


// A path is a list of subpaths
// A subpath is a list of path edges ("path_node_t") that traverse 
// the outside of the region.  They are listed in cutout order,
// interior-most to exterior-most, and edges are listed CCW if the 
// tool should be to the left of the path, and CW if the tool should
// be to the right of the path

class path_t {
	list<list<path_node_t*> > subpaths;
	list<path_node_t*>all;
	list<vertex_t*>verticies;
	list<vector_t*>vertex_inside_point;
	list<vector_t*>vertex_outside_point;


public:
	void add_vertex(vertex_t *v, vector_t *inside_point, vector_t *outside_point);
	void add_edge(vertex_t *v1, vertex_t *v2);

	void fill_svg(char **buffer, int *length);

	float bb_x1;
	float bb_y1;
	float bb_x2;
	float bb_y2;


};

#endif
