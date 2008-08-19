
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
