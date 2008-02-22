/* A simple standalone XML-RPC server written in C. */

#include <stdlib.h>
#include <stdio.h>
#ifndef WIN32
#include <unistd.h>
#endif


#include <string>
#include <iostream>
#include <cmath>
using namespace std;

#include "octree.h"
#include "expression.h"
#include "space_interval.h"
#include "interval.h"
#include "vector.h"
#include "trimesh.h"



#include <xmlrpc-c/base.h>
#include <xmlrpc-c/server.h>
#include <xmlrpc-c/server_abyss.h>

#include "config.h"  /* information about this build environment */


float max(float a, float b, float c) {
  if ((a >= b) && (a >= c)) 
    return(a);
  else if ((b >= a) && (b >= c))
    return(b);
  else
    return(c);
}



static xmlrpc_value *
render_stl(xmlrpc_env *   const env, 
           xmlrpc_value * const param_array, 
           void *         const user_data ATTR_UNUSED) {
  

  // Inputs
  char *math_string;
  double min_feature;
  xmlrpc_double xmin, xmax, ymin, ymax, zmin, zmax;

  // Outputs
  string status_message;
  int status_code;
  xmlrpc_double user_time;
  char *stl;
  int stl_length;
  xmlrpc_value *result;

  /* Parse our argument array. */
  xmlrpc_decompose_value(env, param_array, "(sddddddd)", &math_string, &min_feature,
			 &xmin, &xmax, &ymin, &ymax, &zmin, &zmax);

  if (env->fault_occurred)
    return NULL;
  
  /* Do the work */

  // Compute the required recursion depth and number of points per side
  // Currently the Octree needs to be uniform --- this is a TODO to fix.
  float xlen = (float)xmax-(float)xmin;
  float ylen = (float)ymax-(float)ymin;
  float zlen = (float)zmax-(float)zmin;

  float maxlen = max(xlen, ylen, zlen);
  
  // Compute octree recursion depth and number of points per side
  // We need to make sure each feature is captured by at least two points,
  // and want there to be 2-3 points per direction inside the smallest
  // octree cells
  int recursion_depth = (int)floorf(log2f( maxlen / min_feature ));
  int nx = 3*(int)ceilf(xlen / min_feature);
  int ny = 3*(int)ceilf(ylen / min_feature);
  int nz = 3*(int)ceilf(zlen / min_feature);

  // Convert infix string to postfix string  
  string infix(math_string);
  string postfix = convert_infix_to_postfix(infix);
  
  // Create abstract syntax tree
  expression_t ex(postfix);

  // Create intervals
  interval_t x,y,z;
  x.set_real_interval((float)xmin,(float)xmax);
  y.set_real_interval((float)ymin,(float)ymax);
  z.set_real_interval((float)zmin,(float)zmax);
  space_interval_t si(x,y,z);

  // Construct and evaluate octree.
  octree_t octree(ex, si);
  octree.eval(recursion_depth);

  // Create and evaluate a trimesh
  trimesh_t trimesh;
  trimesh.populate(&octree,&si, nx, ny, nz);
  trimesh.refine();
  trimesh.mark_triangles_needing_division();
  trimesh.add_centroid_to_object_distance();
  trimesh.move_verticies_toward_corners();
  trimesh.recalculate_normals();

  // Fill STL data
  trimesh.fill_stl(&stl, &stl_length);
  
    // Fill Status Information
  status_message = "Success";
  status_code = 0;
  user_time = 0.0;

    // Build XML-RPC result
  result = xmlrpc_build_value(env, "(sid6)", status_message.c_str(), status_code, user_time, stl, stl_length);
  
  // Return result
  return(result);  
}



int 
main(int           const argc, 
     const char ** const argv) {

    xmlrpc_server_abyss_parms serverparm;
    xmlrpc_registry * registryP;
    xmlrpc_env env;

    if (argc-1 != 1) {
        fprintf(stderr, "You must specify 1 argument:  The TCP port "
                "number on which the server will accept connections "
                "for RPCs.  You specified %d arguments.\n",  argc-1);
        exit(1);
    }
    
    xmlrpc_env_init(&env);

    registryP = xmlrpc_registry_new(&env);

    xmlrpc_registry_add_method(
        &env, registryP, NULL, "render.stl", &render_stl, NULL);

    /* In the modern form of the Abyss API, we supply parameters in memory
       like a normal API.  We select the modern form by setting
       config_file_name to NULL: 
    */
    serverparm.config_file_name = NULL;
    serverparm.registryP        = registryP;
    serverparm.port_number      = atoi(argv[1]);
    serverparm.log_file_name    = "/tmp/xmlrpc_log";

    printf("Running OpenCAD Geometry Rendering Server...\n");

    xmlrpc_server_abyss(&env, &serverparm, XMLRPC_APSIZE(log_file_name));
    
    cout << "server ended.\n";

    /* xmlrpc_server_abyss() never returns */

    return 0;
}
