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
      delete children;
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
          delete children;
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
      return(result.get_boolean());
    }
  }
  else {
    return(children[space_interval.get_zone(x,y,z)]->eval_at_point(x, y, z));
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

  //  cout << "eval zone.\n";

  // no action needed for known false zones, since the array
  // is already initialized to zeros
  
  // cout << value << "\n";

    if (value.is_true() || value.is_mixed()) {
  
      //  cout << "passed value test.\n";
  
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
    
    //cout << start_index[0] << " " << start_index[1] << " " << start_index[2] << "\n";
    //cout << start_point[0] << " " << start_point[1] << " " << start_point[2] << "\n";
    //cout << num_pts[0] << " " << num_pts[1] << " " << num_pts[2] << "\n";

    //cout << eval_info->step[0] << " " << eval_info->step[1] << " " << eval_info->step[2] << "\n";



    int ystart, zstart;
    // Compute the x, y, and z stride for the array inside the evaluation loop
    xstride = 1;
    ystride = eval_info->n[0];
    zstride = (eval_info->n[1])*(eval_info->n[0]);
    
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
    
    else {
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
    } 
}


void octree_t::eval_on_grid(space_interval_t &eval_interval, int nx, int ny, int nz, char *results) {
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
  
  // Initialize array to zeros
  for(i=0; i<(nx*ny*nz); i++) {
    results[i] = 0;
  }

  this->eval_on_grid_core(&eval_info);
}
