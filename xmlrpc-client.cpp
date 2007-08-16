/* XML-RPC Test Client for Rendering Server
   Sends the teapot to XML-RPC server and saves resulting STL file. */

#include <string>
#include <iostream>
using namespace std;


#include <stdlib.h>
#include <stdio.h>

#include <xmlrpc-c/base.h>
#include <xmlrpc-c/client.h>

#include "config.h"  /* information about this build environment */

#define NAME "Xmlrpc-c Test Client"
#define VERSION "1.0"

static void 
die_if_fault_occurred (xmlrpc_env *env) {
    if (env->fault_occurred) {
        fprintf(stderr, "XML-RPC Fault: %s (%d)\n",
                env->fault_string, env->fault_code);
        exit(1);
    }
}



int 
main(int           const argc, 
     const char ** const argv ATTR_UNUSED) {

    xmlrpc_env env;
    xmlrpc_value *result;
    xmlrpc_int32 sum;
    char * const serverUrl = "http://localhost:8080/RPC2";
    char * const methodName = "render.stl";

    if (argc-1 > 0) {
        fprintf(stderr, "This program has no arguments\n");
        exit(1);
    }

    /* Initialize our error-handling environment. */
    xmlrpc_env_init(&env);

    /* Start up our XML-RPC client library. */
    xmlrpc_client_init2(&env, XMLRPC_CLIENT_NO_FLAGS, NAME, VERSION, NULL, 0);
    die_if_fault_occurred(&env);


    // Set up args

    string math_string = "X**2 + Y**2 + Z**2 < 0.5";
    xmlrpc_double min_feature = 0.1;
    xmlrpc_double xmin = -1.0;
    xmlrpc_double xmax = 1.0;
    xmlrpc_double ymin = -1.0;
    xmlrpc_double ymax = 1.0;
    xmlrpc_double zmin = -1.0;
    xmlrpc_double zmax = 1.0;


    cout << "Making XMLRPC call to server " << serverUrl << "\n";
    cout << "Method " << methodName << "\n";
    cout << "Math String: " << math_string << "\n";
    cout << "Minimum Feature: " << min_feature << "\n";
    cout << "xmin: << " << xmin << " xmax: " << xmax << "\n";
    cout << "ymin: << " << ymin << " ymax: " << ymax << "\n";
    cout << "zmin: << " << zmin << " zmax: " << zmax << "\n";
 
    
    /* Make the remote procedure call */
    result = xmlrpc_client_call(&env, serverUrl, methodName,
                                "(sddddddd)", math_string.c_str(), min_feature, xmin, xmax, ymin, ymax, zmin, zmax);
    die_if_fault_occurred(&env);
    
    /* Get our sum and print it out. */
    //xmlrpc_read_int(&env, result, &sum);
 
    die_if_fault_occurred(&env);
    printf("The sum is %d\n", sum);
    
    /* Dispose of our result value. */
    xmlrpc_DECREF(result);

    /* Clean up our error-handling environment. */
    xmlrpc_env_clean(&env);
    
    /* Shutdown our XML-RPC client library. */
    xmlrpc_client_cleanup();

    return 0;
}

