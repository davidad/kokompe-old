#include "octree.h"


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
  }


  // Constructor
  // Used internally for recursively evaluating
  octree_t::octree_t(octree_t *parent_in, expression_t *expression_in, space_interval_t &space_interval_in):space_interval(space_interval_in) {
    parent = parent_in;

    expression = expression_in;
    expression_root = 0;

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

    if (recursion_depth > 0) {

      // Evaluate the expression and test to see if it can be 
      // pruned for further evaluation on this interval
      value = expression->prune(space_interval, 0, &would_prune, 0, NULL);  
    
      if ((would_prune) && (!(value.is_resolved()))) {
        
        if (expression_root == 0) {     
          // Copy the expreesion to a node managed by this element
          expression = new expression_t(*expression);
          expression_root = 1;
        }
        
        // Prune the copied expression
        expression->prune(space_interval, 1, &would_prune, 0, NULL);
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
// checking to see if zone hint is causing problems --- uncomment 
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
		//	cout << "found " << unequal_count << " unequal clauses!" << endl;
	    // This case (rare, but happens) could be addressed with a Sat-Solver
		// type algorithim to check to see which of the unequal clauses actually
		// changes the value of the expression --- given a proper representation
		// of the boolean part of the expression, could be fast.
		
		  // allowing this case to deal with degeneracy in castle

  //clause = -1;
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
