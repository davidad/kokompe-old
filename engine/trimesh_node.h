#ifndef TRIMESH_NODE_H
#define TRIMESH_NODE_H

using namespace std;

class vertex_t;
class vector_t;


class trimesh_node_t {
public:
	vertex_t *verticies[3];
//	trimesh_node_t *neighbors[3];    // edge neighbors of this triangle
	vector_t normal;
	//vector_t* interior_point;
	//vector_t* exterior_point;
	int dirty;	
	int has_unknown_clause;
	//	float centroid_to_object;

	friend ostream& operator<<(ostream &s, const trimesh_node_t &v);

	trimesh_node_t() {
		dirty = 0;
		has_unknown_clause = 0;
		//centroid_to_object = 0;
	//	for (int i=0; i<3; i++) {
	//		neighbors[i] = NULL;
	//	}
	}
};

#endif
