#include </usr/include/python2.4/Python.h>
#include </usr/include/python2.4/numarray/libnumarray.h>

using namespace std;
#include <iostream>

#include "octree.h"
#include "expression.h"
#include "space_interval.h"
#include "interval.h"

// Pointer to library-scope octree instance on heap.
octree_t *octree;

/* CreateTree
 *
 * Usage: 
 *
 * geomeval.createTree(infixExpression, xMin, xMax,
 *                                      yMin, yMax,
 *                                      zMin, zMax,
 *                                      recursionDepth)
 *
 * Creates an octree evaluation context for an infix expression,
 * and evaluates it to a given recursion depth.  Run time increases
 * geometrically with recursionDepth --- for eventual 200x200x200
 * voxel grids, a recursionDepth of 6 seems to work well.
 *
 * Returns an integer, currently always 0. */

static PyObject *
createTree(PyObject *self, PyObject *args)
{
  const char *infix_cstr;
  float x_min, x_max, y_min, y_max, z_min, z_max;
  int recursion_depth;

  // Initialize numarray library: required before any
  // numarray calls
  import_libnumarray();

  // Get arguments from Python
  if (!PyArg_ParseTuple(args, "sffffffi",
			&infix_cstr,
			&x_min, &x_max,
			&y_min, &y_max,
			&z_min, &z_max,
			&recursion_depth))
    return NULL;
  
  // Convert infix string to postfix
  string infix(infix_cstr);
  string postfix = convert_infix_to_postfix(infix);

  // Create expression object
  expression_t ex(postfix);

  // Create evaluation interval object
  interval_t X, Y, Z;
  X.set_real_interval(x_min, x_max);
  Y.set_real_interval(y_min, y_max);
  Z.set_real_interval(z_min, z_max);
  space_interval_t si(X, Y, Z);

  // Create octree with expression and evaluation interval
  octree = new octree_t(ex, si);
  
  // Evaluate tree and prune expression down to recursion depth
  octree->eval(recursion_depth);

  // Return Zero
  int sts = 0;
  return(Py_BuildValue("i", sts));
}

/* deleteTree
 * 
 * Usage:
 *
 * geomeval.deleteTree()
 *
 * Deletes the evaluation context created by createTree,
 * freeing its memory. */ 
 
static PyObject *
deleteTree(PyObject *self, PyObject *args) {
  delete octree;

  int sts = 0;
  return(Py_BuildValue("i", sts));

}

/* evalZSlive 
 *
 * Usage:
 *
 * geomeval.evalZSlice(xMin, xMax, yMin, yMax, z, nX, nY)
 *
 * Evaluate an expression on a rectangular perpindicular to the 
 * Z axis.  The grid has nX points along the X axis and nY points
 * along the Y axis.
 *
 * Note: An evaluation context must have been created using createTree.
 * The bounds of the evaluation region given to this function must
 * lie inside the region specified to createTree.  This funcction uses
 * the pruned subexpressions generated by createTree.  Up to a point,
 * the larger the recursion depth given to createTree, the faster it will run.
 *
 * Returns a NumArray compatible matrix of char's, 1 if the 
 * expression is true, 0 if it is false. */

static PyObject *
evalZSlice(PyObject *self, PyObject *args) {

  float x_min, x_max, y_min, y_max, z;
  int nx, ny;
  char *results;

  // Get arguments from Python
  if (!PyArg_ParseTuple(args, "fffffii",
			&x_min, &x_max,
			&y_min, &y_max,
			&z,
			&nx, &ny))
    return NULL;

  // Create Space Interval
  interval_t X, Y, Z;
  X.set_real_interval(x_min, x_max);
  Y.set_real_interval(x_min, x_max);
  Z.set_real_number(z);
  space_interval_t si(X,Y,Z);

  // Allocate memory to store results
  results = new char[(long)nx*(long)ny];

  // Evaluate expression on grid using octree
  octree->eval_on_grid(si, nx, ny, 1, results);

  // Return NumArray compatible matrix with results
  return (PyObject*)NA_NewArray((void *)results, tUInt8, 2, nx, ny);
}


static PyMethodDef GeomEvalMethods[] = {
    
    {"createTree",  createTree, METH_VARARGS, "Setup evaluation context."},
    {"evalZSlice",  evalZSlice, METH_VARARGS, "Evaluate Z slice."},
    {"deleteTree",  deleteTree, METH_VARARGS, "Delete evaluation context."},

    {NULL, NULL, 0, NULL}        /* Sentinel */
};

PyMODINIT_FUNC
initgeomeval(void)
{
    (void) Py_InitModule("geomeval", GeomEvalMethods);
}
