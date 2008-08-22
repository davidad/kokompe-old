#include "octree.h"
#include "cube_surface.h"
#include "vector.h"
#include "vertex.h"
#include <cmath>
using namespace std;

  // Constructor
  // User Constructor
  // Sets up a new octree root with a space interval and expression,
  // ready for evaluation
  octree_t::octree_t(expression_t& expression_in, space_interval_t& space_interval_in):space_interval(space_interval_in) {
    expression = new expression_t(expression_in);
    expression_root = 1;
    children = NULL;
    parent = NULL;
	recursion_level = 0;
  }


  // Constructor
  // Used internally for recursively evaluating
  octree_t::octree_t(octree_t *parent_in, expression_t *expression_in, space_interval_t &space_interval_in):space_interval(space_interval_in) {
    parent = parent_in;

    expression = expression_in;
    expression_root = 0;
	recursion_level = 0;

    children = NULL;
  }
  
  // Destructor
  // Deleting an octree recursively deletes all of its children
  octree_t::~octree_t() {
	if (expression_root)
      delete expression;

    if (children != NULL) {
      for(int i=0; i<8; i++) {
        delete children[i];
      }
      delete []children;
    }
  }

  void octree_t::delete_cache() {
	expression->delete_cache();
  }

  // Eval    
  // Recursively evaluate the expression on a space interval,
  // stopping after the value of the expression is known everywhere
  // on the interval, or after a recursion depth of N
  void octree_t::eval(int recursion_depth) {
    interval_t tmp;
    space_interval_t si;
    int i;
    int would_prune;

    //cout << "about to eval expression on interval " << space_interval << "\n";
   

    // value = expression->eval(space_interval);

	// store the recusion level at this node for context
	// mixed leaves are level 0)
	recursion_level = recursion_depth;

    if (recursion_depth > 0) {

      // Evaluate the expression and test to see if it can be 
      // pruned for further evaluation on this interval
      int do_float_opt;
	  if (parent == NULL)  // only do floating-point optimization (e.g. removing adding 0) the first time
	    do_float_opt = 1;
	  else
	    do_float_opt = 0;


      value = expression->prune(space_interval, 0, &would_prune, 0, NULL, do_float_opt);  
    
      if ((would_prune) && (!(value.is_resolved()))) {
        
        if (expression_root == 0) {     
          // Copy the expreesion to a node managed by this element
          expression = new expression_t(*expression);
          expression_root = 1;
        }
        
        // Prune the copied expression
        expression->prune(space_interval, 1, &would_prune, 0, NULL, do_float_opt);
      }
    }
    else {
      // End of the line.  Just evaluate the expression.
      value = expression->eval(space_interval);
    }

    


    //cout << "done evaluating, result is " << value << "\n";

    if (value.is_resolved()) {
      //cout << "resolved!\n";
      return;
    }
    else {
      //cout << "not resolved.\n";

      if (recursion_depth > 0) {
        //cout << "generating 8 children.\n";


        // Generate 8 children and recurse
        
        // Later we will check to see if the expression equals the 
        // previous expression, and not bother re-evaluating if so.
        // For now we will just always delete any existing children.
        
        if (children != NULL) {
          for (i=0; i<8; i++) {
            delete children[i];
          }
          delete []children;
        }
        
        children = new octree_t*[8];
        for(i=0; i < 8; i++) {
          si = space_interval;
          si.split(i);
          //cout << "generating child " << i << " on interval " << si << "\n";
          children[i] = new octree_t(this, expression, si);
          //cout << "about to call with recursion depth " << recursion_depth-1 << "\n";
          children[i]->eval(recursion_depth-1);
          //cout << "called" << "\n";
        }
      }
    }
  }

// Get the interval value of the expression at a particular point
interval_t octree_t::get_value_at_point(float x, float y, float z) {
  if (children == NULL)
    return(value);
  else
    return(children[space_interval.get_zone(x,y,z)]->get_value_at_point(x, y, z));
}

int octree_t::eval_at_point(float x, float y, float z) {
  static octree_t* zone_hint = NULL;
  if (children == NULL) {
    if (value.is_resolved()) {
      return(value.get_boolean());
    }
    else {
      interval_t xi;
      interval_t yi;
      interval_t zi;
      interval_t result;
      
      xi.set_real_number(x);
      yi.set_real_number(y);
      zi.set_real_number(z);
      space_interval_t si(xi, yi, zi);
      result = expression->eval(si);
	  zone_hint = this;
      return(result.get_boolean());
    }
  }
  else {
	  // The zone hint is the last leaf node which terminated a search.  If
	  // we need to drill down, check this leaf node first.  (Because this function
	  // is often repeatedly called inside a line search with almost identical 
	  // values from iteration to iteration.)
// checking to see if zone hint is causing problems --- uncomment   TODO TODO   TODO 
	/*  if (zone_hint != NULL) {
		  if (zone_hint->space_interval.is_on(x,y,z)) {
			return(zone_hint->eval_at_point(x,y,z));
		  }
		  else {
			zone_hint = NULL;
		  }
	  }
	  else {
	  }*/
	  return(children[space_interval.get_zone(x,y,z)]->eval_at_point(x, y, z));
  }
}


int octree_t::cached_eval_at_point(float x, float y, float z, int lx, int ly, int lz, int cache_offset) {
  static octree_t* zone_hint = NULL;
  if (children == NULL) {
    if (value.is_resolved()) {
      return(value.get_boolean());
    }
    else {
      interval_t xi;
      interval_t yi;
      interval_t zi;
      interval_t result;
      
      xi.set_real_number(x);
      yi.set_real_number(y);
      zi.set_real_number(z);
      space_interval_t si(xi, yi, zi);
      result = expression->cached_eval(si, lx,ly,lz,cache_offset);
	  zone_hint = this;
      return(result.get_boolean());
    }
  }
  else {
	  // The zone hint is the last leaf node which terminated a search.  If
	  // we need to drill down, check this leaf node first.  (Because this function
	  // is often repeatedly called inside a line search with almost identical 
	  // values from iteration to iteration.)
// checking to see if zone hint is causing problems --- uncomment   TODO TODO   TODO 
	/*  if (zone_hint != NULL) {
		  if (zone_hint->space_interval.is_on(x,y,z)) {
			return(zone_hint->eval_at_point(x,y,z));
		  }
		  else {
			zone_hint = NULL;
		  }
	  }
	  else {
	  }*/
	  return(children[space_interval.get_zone(x,y,z)]->eval_at_point(x, y, z));
  }
}



// Evaluate an expression at TWO points (assumed to be very close together)
// and find a clause of the expression which is different between the points.
// If all clauses are identical, returns -1.  

// This method does the following:
//  1. Find the smallest octree region that contains both points.
//  2. Rebuild the clause table for the expression there if it does not exist / is dirty
//  3. Evaluate each expression in the clause table at both points and note if there is a difference
//

// This implementation assumes that the points are on the octree zone specified, and also
// that the result of the expression is different at the points

// FEATURE REQUEST:  This function should make use of the ZONE HINT from eval_at_point,
// since usually that zone will be the zone we want! (would improve speed)

int octree_t::differential_eval(float x1, float y1, float z1, float x2, float y2, float z2) {
	int zone1, zone2;

	if (children != NULL) {
		zone1 = space_interval.get_zone(x1,y1,z1);
		zone2 = space_interval.get_zone(x2,y2,z2);
	}

	if ((children == NULL) || (zone1 != zone2)) {
		// We are in the smallest applicable octree region; work here
		int clause = -1;

		// Verify the integrity of the clause table -- rebuild if dirty or not present
		if (expression->clause_table == NULL) {
			expression->create_clause_table();
		}
		else if (expression->clause_table->dirty) {
			expression->create_clause_table();		
		}

		space_interval_t p1;
		space_interval_t p2;
		p1.set_point(x1,y1,z1);
		p2.set_point(x2,y2,z2);
		
		// Clause table is created based on number of clauses in actual expression
		// Clause numbers are universal
		// Indicies to clause table are not equal to clause numbers!

		// Evaluate each clause of the expression
		int num_clauses = expression->clause_table->num_clauses;
		interval_t v1;
		interval_t v2;
		
		int unequal_count = 0;
		for(int i=0; i<num_clauses; i++) {
			v1 = expression->clause_table->clauses[i]->eval(p1);
			v2 = expression->clause_table->clauses[i]->eval(p2);
			if ((v1.get_boolean() != v2.get_boolean())) {
				// we found an unequal clause
				clause = expression->clause_table->clauses[i]->clause_number;
				unequal_count++;
			}
		}
		//if (clause == -1) {
		//	cout << "no unequal clause found!!!" << endl;
		//}
		if (unequal_count > 1) {
	//		cout << "found " << unequal_count << " unequal clauses!" << endl;
	    // This case (rare, but happens) could be addressed with a Sat-Solver
		// type algorithim to check to see which of the unequal clauses actually
		// changes the value of the expression --- given a proper representation
		// of the boolean part of the expression, could be fast.
		
		  // allowing this case to deal with degeneracy in castle

  //clause = -1;
		}
		if (unequal_count == 0) {
			interval_t i1, i2;
			
			i1 = expression->eval(p1);
			i2 = expression->eval(p2);
			// TODO: Investigate when this happens!  It may be an intervalization bug,
			// and it may be leading to some of the "chips" in corners and edges!
			// (Schema is: doesn't matter which interval a points is evaled in --- should be correct
			// regardless because closed intervals are used.  But I suppose a rounding error might
			// mess this up?  TRACE THIS!  (happens on teapot once)
	//		cout << "found 0 unequal clauses!!!\n";
		}
		if ((unequal_count > 1) && (clause == -1)) {
	//		cout << "found unequal clause with unknown number.\n";
		}


		return(clause);
	}
	else {
		return(children[zone1]->differential_eval(x1,y1,z1,x2,y2,z2));
	}
}



void octree_t::eval_on_grid_core(eval_info_t *eval_info) {
  int i;
  //cout << "entering...";

  if (space_interval.overlaps(eval_info->eval_interval)) {    
    //cout << "overlap true.\n";
    if (children == NULL) {
      //cout << "running on " << space_interval << "...";
      this->eval_zone_on_grid(eval_info);
      //cout << "done.\n";
    }
    else {
      // cout << "walking children...";
      for (i=0; i<8; i++) {
        children[i]->eval_on_grid_core(eval_info);      
      }
      //cout << "done walking children.\n";
    }
  }
}



void octree_t::eval_zone_on_grid(eval_info_t *eval_info) {
  int start_index[3];
  int end_index[3];
  float start_point[3];
  int i;
  int index;
  int xi, yi, zi;
  int num_pts[3];
  interval_t eval_interval;
  interval_t zone_interval;
  int xstride, ystride, zstride;
  space_interval_t eval_point;
  float X, Y, Z;

    // Find rectangular block zone of array corresponding to this area
    
    for(i=0; i<3; i++) {
      eval_interval = eval_info->eval_interval.get_var_value(i);
      zone_interval = space_interval.get_var_value(i);
      // cout << "space interval: " << space_interval << "\n";
      // cout << "i = " << i << " zone_interval = " << zone_interval << " eval_interval = " << eval_interval << "\n";

      if (eval_interval.get_lower() >= zone_interval.get_lower()) {
        start_index[i] = 0;
        //cout << "set to zero\n";
      }
      else {
        //cout << zone_interval.get_lower() << " " << eval_interval.get_lower() << " " << eval_info->step[i] << " for i = " << i << "\n";

        start_index[i] = (int)(ceilf((zone_interval.get_lower() - eval_interval.get_lower() - 0.5f*eval_info->step[i])*eval_info->step_recip[i]));
      }
      
      if (eval_interval.get_upper() <= zone_interval.get_upper()) {
        end_index[i] = eval_info->n[i] - 1;
      }
      else {
        end_index[i] = (int)(floorf((zone_interval.get_upper() - eval_interval.get_lower() - 0.5f*eval_info->step[i])*eval_info->step_recip[i]));
      }
      
      num_pts[i] = end_index[i] - start_index[i] + 1;
      start_point[i] = eval_interval.get_lower() + 0.5f*eval_info->step[i] + start_index[i]*eval_info->step[i];
      
    }

    int ystart, zstart;
    // Compute the x, y, and z stride for the array inside the evaluation loop
    xstride = 1;
    ystride = eval_info->n[0];
    zstride = (eval_info->n[1])*(eval_info->n[0]);
    
    // Compute the first linear index into the array
    index = start_index[2]*eval_info->n[0]*eval_info->n[1] + start_index[1]*eval_info->n[0] + start_index[0];
    
	// Write the coordinates to use for evaluation
	// This is done ONCE, HERE, and then the exact floating
	// point values from here are used throughout --- if the values
	// are regenerated later, roundoff errors can cause incorrect voxels
	// to be filled, leading to logic bugs and bad rendering artifacts
    if (eval_info->result_points != NULL) {
    Z = start_point[2];                                           
      zstart = index;
      for (zi=0; zi < num_pts[2]; zi++) { 
        Y = start_point[1];
        ystart = zstart;
        for (yi=0; yi < num_pts[1]; yi++) { 
          X = start_point[0];
          index = ystart;
          for (xi=0; xi < num_pts[0]; xi++) {        
              eval_info->result_points[index].set(X,Y,Z);         
            index += xstride;
            X += eval_info->step[0];
          }
          ystart += ystride;
          Y += eval_info->step[1];
        }
        zstart += zstride;
        Z += eval_info->step[2];
      }
    }

    // Compute the first linear index into the array
    index = start_index[2]*eval_info->n[0]*eval_info->n[1] + start_index[1]*eval_info->n[0] + start_index[0];

    if (value.is_true()) {
      // If this octree zone is guaranteed to be true, just fill the array
      // with ones in a tight loop

      
      zstart = index;
      for (zi=0; zi < num_pts[2]; zi++) { 
        ystart = zstart;
        for (yi=0; yi < num_pts[1]; yi++) { 
          index = ystart;
          for (xi=0; xi < num_pts[0]; xi++) { 
            //cout << "writing index " << index << "\n";
                eval_info->results[index] = 1;
                //cout << "fast filling point\n";
            index += xstride;
          }
          ystart += ystride;
        }
        zstart += zstride;
      }
    }
     
    else if (value.is_mixed()) {
      // If this octree section is mixed, evaluate the expression at each
      // point and store the result
      
      Z = start_point[2];                                           
      zstart = index;
      for (zi=0; zi < num_pts[2]; zi++) { 
        Y = start_point[1];
        ystart = zstart;
        for (yi=0; yi < num_pts[1]; yi++) { 
          X = start_point[0];
          index = ystart;
          for (xi=0; xi < num_pts[0]; xi++) { 
            
            eval_point.set_point(X, Y, Z);
            //cout << "writing index " << index << "\n";
              eval_info->results[index] = (char)((expression->eval(eval_point)).get_boolean());
                       
            index += xstride;
            X += eval_info->step[0];
          }
          ystart += ystride;
          Y += eval_info->step[1];
        }
        zstart += zstride;
        Z += eval_info->step[2];
      }
    }
   // } 
}


void octree_t::eval_on_grid(space_interval_t &eval_interval, int nx, int ny, int nz, char *results, vector_t *result_points ) {
  eval_info_t eval_info;
  int i;
  
  eval_info.eval_interval = eval_interval;
  eval_info.n[0] = nx;
  eval_info.n[1] = ny;
  eval_info.n[2] = nz;
  


  for (i=0; i<3; i++) {
    eval_info.n_recip[i] = 1/((float)(eval_info.n[i]));
    eval_info.step[i] = (eval_interval.get_var_value(i).get_upper() - eval_interval.get_var_value(i).get_lower()) * eval_info.n_recip[i];
    eval_info.step_recip[i] = 1/(eval_info.step[i]);
  }
  eval_info.results = results;
  eval_info.result_points = result_points;
  
  // Initialize array to zeros
  for(i=0; i<(nx*ny*nz); i++) {
    results[i] = 0;
  }

  this->eval_on_grid_core(&eval_info);
}


// Evaluates an octree at its geometric center and returns the 
// result, true or false
int octree_t::eval_at_center() {
	if (value.is_resolved()) {
		return(value.get_boolean());
	}
	else {
      space_interval_t center_si;
	  center_si.set_to_center(space_interval);
      if (expression->eval(center_si).is_true())
		  return(1);
	  else
		  return(0);
	}
}

void octree_t::create_cache(int lvl) {
		int size = 4*(1 << lvl);   // do in 2 x 2 x 2 blocks

	expression->mark_dependence();
	expression->unmark();
	expression->create_cache(size);

}

// Create a populated trimesh for the octree, using the 
// octree traversal method, rather than the old layer-by-layer
// method.   Pass in a pointer to a trimesh object --- a new trimesh
// is returned, which then you are responsible for deleting

void octree_t::trimesh(trimesh_t **trimesh) {
	cube_surface_t *cube_surface;
	
	int size = 4*(1 << recursion_level);   // do in 2 x 2 x 2 blocks
	
	// For now, this function always evaluates over a box -1, 1
	// This would be easy to change
	int x = -size/2;
	int y = -size/2;
	int z = -size/2;
	float stepsize = 2.0f / (float)size;
	int cache_offset = size/2;

	// Create the expression caches
	// Walk the expression and find the highest points
	// that are dependent only on one variable.  For each, create
	// an array of interval_t's and put a pointer to it.
	


	// DO the evaluation
	trimesh_core(trimesh, &cube_surface, x, y, z, size, stepsize, cache_offset);
	// Delete final cube surface data structure
	delete cube_surface;
	(*trimesh)->octree = this;


	// DEBUG TRIPWIRE TODO REMOVE
	// Make sure all triangles have a full set of neighbors
	//(*trimesh)->check_neighbors();
	



}


inline void write_triangles(int lvalue, trimesh_t *trimesh, vertex_t ** v, vector_t normal) {
    if (lvalue == 1) {
      // construct a face, two triangles, pointing out from left to right
      trimesh->add_triangle(v[0], v[1], v[2], normal);
      trimesh->add_triangle(v[2], v[3], v[0], normal);
    }
    else {
      // construct a face, two triangles, pointing in from right to left
      trimesh->add_triangle(v[2], v[1], v[0], mul(normal, -1.0f));
      trimesh->add_triangle(v[0], v[3], v[2], mul(normal, -1.0f));
    }
}


inline void build_face(int l, int r, int lx, int ly, int lz, int rx, int ry, int rz, int* ldx, int* ldy, int* ldz, int* rdx, int* rdy, int* rdz, trimesh_t *trimesh, cube_surface_t **cube_surfaces, vector_t normal, float stepsize) {
  
  
  vertex_t *v[4];
  vertex_t *lvp;
  vector_t *left_point;
  vector_t *right_point;
  
  float fx, fy, fz;
  
  int lvalue = cube_surfaces[l]->get_data(lx, ly, lz);
  int rvalue = cube_surfaces[r]->get_data(rx, ry, rz);
  
  // If we need to build a face
  if (lvalue != rvalue) {
    
    for (int i=0; i<4; i++) {
      lvp = cube_surfaces[l]->get_vertex(lx, ly, lz, ldx[i], ldy[i], ldz[i]);

      if (lvp == NULL) {
	// need to create verticies for upcoming triangle creation
	cube_surfaces[l]->get_vertex_coordinates(lx, ly, lz, ldx[i], ldy[i], ldz[i], &fx, &fy, &fz);
	lvp = new vertex_t(fx, fy, fz);
	
	float range = 0.501f*stepsize;
	lvp->si.set(fx-range, fx+range, fy-range, fy+range, fz-range, fz+range);


	// Get inside and outside points for vertex creation
	left_point = cube_surfaces[l]->get_point(lx,ly,lz);
	if (left_point == NULL) {
	  cube_surfaces[l]->get_point_coordinates(lx, ly, lz, &fx, &fy, &fz);
	  left_point = new vector_t(fx, fy, fz);
	  cube_surfaces[l]->set_point(left_point, lx,ly,lz);
	  trimesh->add_voxel_center(left_point);
	}
	
	// Get inside and outside points for vertex creation
	right_point = cube_surfaces[r]->get_point(rx,ry,rz);
	if (right_point == NULL) {
	  cube_surfaces[r]->get_point_coordinates(rx, ry, rz, &fx, &fy, &fz);
	  right_point = new vector_t(fx, fy, fz);
	  cube_surfaces[r]->set_point(right_point, rx,ry,rz);
	  trimesh->add_voxel_center(right_point);
	}
	
	if (lvalue) {
	  trimesh->add_vertex(lvp, left_point, right_point);
	}
	else {
	  trimesh->add_vertex(lvp, right_point, left_point);
	}
	
	cube_surfaces[l]->set_vertex(lvp, lx, ly, lz, ldx[i], ldy[i], ldz[i]);
	cube_surfaces[r]->set_vertex(lvp, rx, ry, rz, rdx[i], rdy[i], rdz[i]);
	  }
      
      v[i] = lvp;
    }
    
   
    // Build a new face if needed
    

	write_triangles(lvalue, trimesh, v, normal);


  }			
}



inline void knit_faces(int l, int r, int lx, int ly, int lz, int rx, int ry, int rz, trimesh_t * trimesh, cube_surface_t **cube_surfaces) {

  vertex_t *lvp = cube_surfaces[l]->get_vertex(lx, ly, lz, 0, 0, 0);
  vertex_t *rvp = cube_surfaces[r]->get_vertex(rx, ry, rz, 0, 0, 0);
        
 /* if ((lvp == (vertex_t*)0x14bd0d0) || (lvp == (vertex_t*)0x14bcf20) ||
	  (rvp == (vertex_t*)0x14bd0d0) || (rvp == (vertex_t*)0x14bcf20)) {
		  cout << "touching it!";
  }*/

  

  if (lvp != rvp) {
    
    if ((lvp == NULL) && (rvp != NULL)) {
      cube_surfaces[l]->set_vertex(rvp, lx, ly, lz, 0, 0, 0);
    }
    else if ((lvp != NULL) && (rvp == NULL)) {
      cube_surfaces[r]->set_vertex(lvp, rx, ry, rz, 0, 0, 0);
    }
    else {
      // Vertex collision needing rectification
      // DEBUG TRIPWIRE
	//	if ((rvp->x != lvp->x) || (rvp->y != lvp->y) || (rvp->z != lvp->z)) {
	//		cout << "overwriting wrong vertex.\n";
	//	}

      trimesh->replace_vertex(rvp, lvp);  // old, new
      cube_surfaces[r]->set_vertex(lvp, rx, ry,rz, 0, 0, 0);   
    }
  }
}  


inline void build_faces(trimesh_t *trimesh, cube_surface_t **cube_surfaces, int x, int y, int z, int halfsize, float stepsize) {
	int s = cube_surfaces[0]->side_length();
	int maxval = s-1;
	int lx,ly,lz, rx, ry, rz;

	int all1[4] = {1,1,1,1};
	int all0[4] = {0,0,0,0};
	int p0[4] = {0, 1, 1, 0};
	int p1[4] = {0, 0, 1, 1};
	vector_t normal;


	int *ldx;
	int *ldy;
	int *ldz;
	int *rdx;
	int *rdy;
	int *rdz;


	// Build the YZ plane faces
	lx = maxval;
	ldx = all1;
	ldy = p0;
	ldz = p1;

	rx = 0;
	rdx = all0;
	rdy = p0;
	rdz = p1;
	normal.set(1.0f, 0.0f, 0.0f);


	lx = maxval+1;
	for (ly=0; ly <=s; ly++) {
	  ry = ly;
	  for (lz=0; lz <= s; lz++) {
	    rz = lz;
	    knit_faces(0,1,lx,ly,lz,rx,ry,rz, trimesh, cube_surfaces);
	    knit_faces(2,3,lx,ly,lz,rx,ry,rz, trimesh, cube_surfaces);
	    knit_faces(4,5,lx,ly,lz,rx,ry,rz, trimesh, cube_surfaces);
	    knit_faces(6,7,lx,ly,lz,rx,ry,rz, trimesh, cube_surfaces);
	  }
	}
	lx = maxval;


	for (ly=0; ly < s; ly++) {
		ry = ly;
		for (lz=0; lz < s; lz++) {
			rz = lz;
			build_face(0,1, lx, ly, lz, rx, ry, rz, ldx, ldy, ldz, rdx, rdy, rdz, trimesh, cube_surfaces, normal, stepsize);
			build_face(2,3, lx, ly, lz, rx, ry, rz, ldx, ldy, ldz, rdx, rdy, rdz,trimesh, cube_surfaces, normal, stepsize);
			build_face(4,5, lx, ly, lz, rx, ry, rz, ldx, ldy, ldz, rdx, rdy, rdz,trimesh, cube_surfaces, normal, stepsize);
			build_face(6,7, lx, ly, lz, rx, ry, rz, ldx, ldy, ldz, rdx, rdy, rdz,trimesh, cube_surfaces, normal, stepsize);
	   }	
	}

	// Build the XY plane faces
	lz = 0;
	ldz = all0;
	ldy = p0;
	ldx = p1;

	rz = maxval;
	rdz = all1;
	rdy = p0;
	rdx = p1;
	normal.set(0.0f, 0.0f, -1.0f);

	rz = maxval+1;
	for (lx=0; lx <=s; lx++) {
	  rx = lx;
	  for (ly=0; ly <= s; ly++) {
	    ry = ly;
	    knit_faces(0,2,lx,ly,lz,rx,ry,rz, trimesh, cube_surfaces);
	    knit_faces(1,3,lx,ly,lz,rx,ry,rz, trimesh, cube_surfaces);
	    knit_faces(4,6,lx,ly,lz,rx,ry,rz, trimesh, cube_surfaces);
	    knit_faces(5,7,lx,ly,lz,rx,ry,rz, trimesh, cube_surfaces);
	  }
	}
	rz = maxval;


	for (lx=0; lx < s; lx++) {
		rx = lx;
		for (ly=0; ly < s; ly++) {
			ry = ly;
			build_face(0,2, lx, ly, lz, rx, ry, rz, ldx, ldy, ldz, rdx, rdy, rdz,trimesh, cube_surfaces,  normal, stepsize);
			build_face(1,3, lx, ly, lz, rx, ry, rz, ldx, ldy, ldz, rdx, rdy, rdz,trimesh, cube_surfaces,  normal, stepsize);
			build_face(4,6, lx, ly, lz, rx, ry, rz, ldx, ldy, ldz, rdx, rdy, rdz,trimesh, cube_surfaces,  normal, stepsize);
			build_face(5,7, lx, ly, lz, rx, ry, rz, ldx, ldy, ldz, rdx, rdy, rdz,trimesh, cube_surfaces,  normal, stepsize);
		}
	}

	// Build the XZ plane faces
	ly = maxval;
	ldy = all1;
	ldx = p1;
	ldz = p0;

	ry = 0;
	rdy = all0;
	rdx = p1;
	rdz = p0;
	normal.set(0.0f, 1.0f, 0.0f);


	ly = maxval+1;
	for (lx=0; lx <=s; lx++) {
	  rx = lx;
	  for (lz=0; lz <= s; lz++) {
	    rz = lz;
	    knit_faces(0,4,lx,ly,lz,rx,ry,rz, trimesh, cube_surfaces);
	    knit_faces(1,5,lx,ly,lz,rx,ry,rz, trimesh, cube_surfaces);
	    knit_faces(2,6,lx,ly,lz,rx,ry,rz, trimesh, cube_surfaces);
	    knit_faces(3,7,lx,ly,lz,rx,ry,rz, trimesh, cube_surfaces);
	  }
	}
	ly = maxval;


	for (lx=0; lx < s; lx++) {
		rx = lx;
		for (lz=0; lz < s; lz++) {
			rz = lz;
			build_face(0,4, lx, ly, lz, rx, ry, rz, ldx, ldy, ldz, rdx, rdy, rdz,trimesh, cube_surfaces,  normal, stepsize);
			build_face(1,5, lx, ly, lz, rx, ry, rz, ldx, ldy, ldz, rdx, rdy, rdz,trimesh, cube_surfaces,  normal, stepsize);
			build_face(2,6, lx, ly, lz, rx, ry, rz, ldx, ldy, ldz, rdx, rdy, rdz,trimesh, cube_surfaces,  normal, stepsize);
			build_face(3,7, lx, ly, lz, rx, ry, rz, ldx, ldy, ldz, rdx, rdy, rdz,trimesh, cube_surfaces,  normal, stepsize);
		}
	}



	// One more thing we have to do at this point is to knit the AXES inside the cube,
	// joining any duplicately created verticies spanning multiple non-facing cubes at the axes

	int m = maxval+1;
	

	// CHECK THIS LOGIC + GEOMETRY!!!!   TODO

	// Knit each of the 6 axis rays, going around in cube order
	//+Z
	for (lz=0; lz<=s; lz++) {
		knit_faces(0,1,m,m,lz, 0, m, lz,trimesh, cube_surfaces);
		knit_faces(1,5,0,m,lz, 0, 0, lz,trimesh, cube_surfaces);
		knit_faces(5,4,0,0,lz, m, 0, lz,trimesh, cube_surfaces);
	}
	//-Z
	for (lz=0; lz<=s; lz++) {
		knit_faces(2,3,m,m,lz, 0, m, lz,trimesh, cube_surfaces);
		knit_faces(3,7,0,m,lz, 0, 0, lz,trimesh, cube_surfaces);
		knit_faces(7,6,0,0,lz, m, 0, lz,trimesh, cube_surfaces);
	}
	
	//-X
	for (lx=0; lx<=s; lx++) {
		knit_faces(0,4,lx,m,0, lx, 0,0,trimesh, cube_surfaces);
		knit_faces(4,6,lx,0,0, lx, 0,m ,trimesh, cube_surfaces);
		knit_faces(6,2,lx,0,m, lx, m,m,trimesh, cube_surfaces);
	}
	//+X
	for (lx=0; lx<=s; lx++) {
		knit_faces(1,5,lx,m,0, lx, 0,0,trimesh, cube_surfaces);
		knit_faces(5,7,lx,0,0, lx, 0,m,trimesh, cube_surfaces );
		knit_faces(7,3,lx,0,m, lx, m,m,trimesh, cube_surfaces);
	}
	
	//+Y
	for (ly=0; ly<=s; ly++) {
		knit_faces(4,5,m,ly,0, 0,ly,0,trimesh, cube_surfaces);
		knit_faces(5,7,0,ly,0, 0,ly,m,trimesh, cube_surfaces); 
		knit_faces(7,6,0,ly,m, m,ly,m,trimesh, cube_surfaces);
	}
	//-Y
	for (ly=0; ly<=s; ly++) {
		knit_faces(0,1,m,ly,0, 0,ly,0,trimesh, cube_surfaces);
		knit_faces(1,3,0,ly,0, 0,ly,m,trimesh, cube_surfaces); 
		knit_faces(3,2,0,ly,m, m,ly,m,trimesh, cube_surfaces);
	}
	
	// Knit the origin, again in order
	knit_faces(0,1, m,m,0, 0,m,0,trimesh, cube_surfaces);
	knit_faces(1,5, 0,m,0, 0,0,0,trimesh, cube_surfaces);
	knit_faces(5,4, 0,0,0,  m,0,0,trimesh, cube_surfaces);
	knit_faces(4,2, m,0,0, m,m,m,trimesh, cube_surfaces);
	knit_faces(2,3, m,m,m, 0,m,m,trimesh, cube_surfaces);
	knit_faces(3,7, 0,m,m, 0,0,m,trimesh, cube_surfaces);








}

inline void leaf_build_face(trimesh_t *trimesh, char *data, vector_t *points, vector_t **voxel_centers, vertex_t **verticies,
							int lx,int ly, int lz, int rx, int ry, int rz, int *ldx, int *ldy, int *ldz, vector_t normal, int size, float stepsize,
							int x, int y, int z) {
	vertex_t *lvp;
	vector_t *left_point, *right_point;
	float fx, fy, fz;

	int ldi = lx + ly*size + lz*size*size;
	int rdi = rx + ry*size + rz*size*size;
	vertex_t *v[4];
		


	// If there is a boundary
	if (data[ldi] != data[rdi]) {
		// Build a face!
		for(int i=0; i<4; i++) {
		  int vi = (lz+ldz[i])*(size+1)*(size+1) + (ly+ldy[i])*(size+1) + lx+ldx[i];

			// Add vertex if needed
			if (verticies[vi] == NULL) {
				fx = points[ldi].x + (-0.5f + (float)ldx[i])*stepsize;
				fy = points[ldi].y + (-0.5f + (float)ldy[i])*stepsize;
				fz = points[ldi].z + (-0.5f + (float)ldz[i])*stepsize;
				lvp = new vertex_t(fx, fy, fz);
				float range = 0.501f*stepsize;
				lvp->si.set(fx-range, fx+range, fy-range, fy+range, fz-range, fz+range);
				

				// TODO REMOVE
				// Debug tripwire --- check validity of points data
			//	float fx2 = ((float)(x+lx) + 0.5f) * stepsize;
			//	float fy2 = ((float)(y+ly) + 0.5f)* stepsize;
			//	float fz2 = ((float)(z+lz) + 0.5f) * stepsize;
			//	fx = points[ldi].x;
			//	fy = points[ldi].y;
			//	fz = points[ldi].z;

				
				
			//	if (((fx-fx2)*(fx-fx2) + (fy-fy2)*(fy-fy2) + (fz-fz2)*(fz-fz2)) > 0.0001) {
			//		cout << "Points do not match!\n";
			//	}




				verticies[vi] = lvp;

				left_point = voxel_centers[ldi];
				if (left_point == NULL) {
					left_point = new vector_t(points[ldi]);
					voxel_centers[ldi] = left_point;
					trimesh->add_voxel_center(left_point);
				}
				right_point = voxel_centers[rdi];
				if (right_point == NULL) {
					right_point = new vector_t(points[rdi]);
					voxel_centers[rdi] = right_point;
					trimesh->add_voxel_center(right_point);
				}

				if (data[ldi]) {
					trimesh->add_vertex(lvp, left_point, right_point);
				}
				else {
					trimesh->add_vertex(lvp, right_point, left_point);
				}
			}
			v[i] = verticies[vi];
		}
		write_triangles((int)data[ldi], trimesh, v, normal);
	}
}


void octree_t::fill_leaf(trimesh_t *trimesh, cube_surface_t *cube_surface, int x, int y, int z, int size, float stepsize, int cache_offset) {

	// TODO: perhaps the octree class could have a scratchpad for these constant-size-per-octree
	// arrays instead of dynamically allocating them each time  like this

	int numel = size*size*size;
	int numelv = (size+1)*(size+1)*(size+1);
	char *data = new char[numel];
	vector_t *points = new vector_t[numel];
	vertex_t **verticies = new vertex_t*[numelv];

	vector_t **voxel_centers = new vector_t*[numel];
	vector_t normal;
	int lx,ly,lz, rx, ry, rz;

	memset(voxel_centers, 0, sizeof(vector_t*)*numel);
	memset(verticies, 0, sizeof(vertex_t*)*numelv);

	int p0[4] = {0, 1, 1, 0};
	int p1[4] = {0, 0, 1, 1};
	int all1[4] = {1,1,1,1};
	int *ldx;
	int *ldy;
	int *ldz;


	// Evaluate
	//this->eval_on_grid(space_interval, size, size, size, data, points);

	// evaluate manually because eval on grid is giving screwy results, with
	// points off by a full grid zone of full steps from xyz + 0.5 * stepsize...
	// later we should track this down....

	float fx, fy, fz;
	int ind = 0;
	for (lz=z; lz<(z+size); lz++) {
		fz = ((float)lz + 0.5f)*stepsize;
		for (ly=y; ly<(y+size); ly++) {
			fy = ((float)ly + 0.5f)*stepsize;
			for (lx=x; lx<(x+size); lx++) {
				fx = ((float)lx + 0.5f)*stepsize;
				data[ind] = this->cached_eval_at_point(fx,fy,fz,lx,ly,lz,cache_offset);
				points[ind].set(fx,fy,fz);
				ind++;
			}
		}
	}

    // Build XY plane faces, excluding the outer edges
	ldz = all1;
	ldx = p0;
	ldy = p1;
	normal.set(0.0f, 0.0f, 1.0f);

	for (lz = 0; lz<(size-1); lz++) {
		rz = lz+1;
		for (ly=0; ly < size; ly++) {
			ry = ly;
			for (lx =0; lx < size; lx++) {
				rx = lx;
				leaf_build_face(trimesh, data, points, voxel_centers, verticies, lx,ly,lz,rx,ry,rz, ldx, ldy, ldz, normal, size, stepsize, x, y, z);
			}
		}
	}

    // Build XZ plane faces, excluding the outer edges
	ldy = all1;
	ldz = p0;
	ldx = p1;
	normal.set(0.0f, 1.0f, 0.0f);

	for (ly = 0; ly<(size-1); ly++) {
		ry = ly+1;
		for (lz=0; lz < size; lz++) {
			rz = lz;
			for (lx =0; lx < size; lx++) {
				rx = lx;
				leaf_build_face(trimesh, data, points, voxel_centers, verticies, lx,ly,lz,rx,ry,rz, ldx, ldy, ldz, normal, size, stepsize, x, y, z);
			}
		}
	}

    // Build YZ plane faces, excluding the outer edges
	ldx = all1;
	ldy = p0;
	ldz = p1;
	normal.set(1.0f, 0.0f, 0.0f);

	for (lx = 0; lx<(size-1); lx++) {
		rx = lx+1;
		for (lz=0; lz < size; lz++) {
			rz = lz;
			for (ly =0; ly < size; ly++) {
				ry = ly;
				leaf_build_face(trimesh, data, points, voxel_centers, verticies, lx,ly,lz,rx,ry,rz, ldx, ldy, ldz, normal, size, stepsize, x, y, z);
			}
		}
	}

	// Now fill in cube surface data structure with surface data
	// A particularly simple way to do this is just to write every data point and have cube surface only write the surface ones
	// TODO later, we could have direct surface loops to avoid extra processing here



	int ldi;

	ldi = 0;
	for (lz=0; lz<size; lz++) {
		for (ly=0; ly<size; ly++) {
			for (lx=0; lx<size; lx++) {
				cube_surface->set_data(data[ldi], lx, ly, lz);
				cube_surface->set_point(voxel_centers[ldi], lx, ly, lz);
				ldi++;
			}
		}
	}
	ldi = 0;
	for (lz=0; lz<=size; lz++) {
		for (ly=0; ly<=size; ly++) {
			for (lx=0; lx<=size; lx++) {
				cube_surface->set_vertex(verticies[ldi], lx, ly, lz,0 ,0 ,0);
				ldi++;
			}
		}
	}

	delete[] data;
	delete[] points;
	delete[] verticies;
	delete[] voxel_centers;


}




// To create a trimesh, recurse down on the octree and create trimeshes 
// there, then fuse the lower layers together.
// percolates up trimesh and cube_surface --- both are return values
// and can go in as unitialized pointers;

// x,y,z are the integer coordinates of the lower-left corner of this octree segment
// size is the side length in voxels of this octree segment
// stepsize is the voxel pitch
void octree_t::trimesh_core(trimesh_t **my_trimesh, cube_surface_t **cube_surface, int x, int y, int z, int size, float stepsize, int cache_offset) {
	

   cube_surface_t *cube_surfaces[8];
   trimesh_t *temp_trimesh;
   int i;

   *my_trimesh = new trimesh_t();

   if (children == NULL) {
	   // Tree Leaf condition
	   if (value.is_resolved()) {
	      // For tree leaves that are already evaluated to true or false, there
		  // are no internal faces, so just return the new empty trimesh and 
	      // construct a cube surface that is all true or false
	//	   if (size > 4) {
	   //  cout << "Resolved leaf of size " << size << "\n";
	//	   }
	     *cube_surface = new cube_surface_t(value.get_boolean(), x, y, z, size, stepsize);
	   }
	   else {
		  // For tree leaves that are mixed, we need to evaluate on a grid and construct
		  // internal faces  (it is very inefficient to actually evaluate the octree all the way down to 
		  // the leaves --- it is better, at a certain point (about 4 x 4 x 4 blocks) to just evaluate
	   //  cout << "Leaf expression is : " << *expression << "\n";

	     *cube_surface = new cube_surface_t(0,x,y,z,size, stepsize); 
		  this->fill_leaf(*my_trimesh, *cube_surface,x,y,z, size,stepsize, cache_offset);
	   }
   }
   else {
	
	   // Recurse --- create and evaluate child trimeshes and surfaces
	   // Merge trimeshes together
	   int halfsize = size/2;
	   	   
		   children[2]->trimesh_core(&temp_trimesh, &(cube_surfaces[0]), x, y, z+halfsize, halfsize, stepsize, cache_offset);
		   (*my_trimesh)->merge(*temp_trimesh);
		   delete temp_trimesh;
	   	   children[3]->trimesh_core(&temp_trimesh, &(cube_surfaces[1]), x+halfsize, y, z+halfsize, halfsize, stepsize, cache_offset);
		   (*my_trimesh)->merge(*temp_trimesh);
		   delete temp_trimesh;
	   	   children[0]->trimesh_core(&temp_trimesh, &(cube_surfaces[2]), x, y, z, halfsize, stepsize, cache_offset);
		   (*my_trimesh)->merge(*temp_trimesh);
		   delete temp_trimesh;
	   	   children[1]->trimesh_core(&temp_trimesh, &(cube_surfaces[3]), x+halfsize, y, z, halfsize, stepsize, cache_offset);
		   (*my_trimesh)->merge(*temp_trimesh);
		   delete temp_trimesh;
	   	   children[6]->trimesh_core(&temp_trimesh, &(cube_surfaces[4]), x, y+halfsize, z+halfsize, halfsize, stepsize, cache_offset);
		   (*my_trimesh)->merge(*temp_trimesh);
		   delete temp_trimesh;
	   	   children[7]->trimesh_core(&temp_trimesh, &(cube_surfaces[5]), x+halfsize, y+halfsize, z+halfsize, halfsize, stepsize, cache_offset);
		   (*my_trimesh)->merge(*temp_trimesh);
		   delete temp_trimesh;
	   	   children[4]->trimesh_core(&temp_trimesh, &(cube_surfaces[6]), x, y+halfsize, z, halfsize, stepsize, cache_offset);
		   (*my_trimesh)->merge(*temp_trimesh);
		   delete temp_trimesh;
	   	   children[5]->trimesh_core(&temp_trimesh, &(cube_surfaces[7]), x+halfsize, y+halfsize, z, halfsize, stepsize, cache_offset);
		   (*my_trimesh)->merge(*temp_trimesh);
		   delete temp_trimesh;
	   



	   // Now go through the inner surfaces of this group-of-8 trimeshes.
	   // Consruct new faces and as needed on the edges, and knit old faces
	   // together by combining duplicate verticies
		   build_faces(*my_trimesh, cube_surfaces,x, y, z, halfsize, stepsize);

	   // Combine the eight cube surfaces into one outer cube surface
	   *cube_surface = new cube_surface_t(cube_surfaces);
	
	   // Delete old cube surfaces
	   for (i=0; i<8; i++) {
		   delete cube_surfaces[i];
	   }
   }
}




