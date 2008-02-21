
#include <iostream>
#include <cmath>
using namespace std;

#include "derivative_table.h"
#include "vector.h"
#include "interval.h"
#include "space_interval.h"


// A table containting the x/y/z derivatives of a each clause of an expression.
// Used to evaluate surface normals analytically and detect/improve edges/corners.

derivative_table_t::derivative_table_t() {
	num_clauses = 0;
	max_clause = -1;
	derivatives_x = NULL;
	derivatives_y = NULL;
	derivatives_z = NULL;

}


// Build the derivative table of an expression
void derivative_table_t::create(expression_t &ex) {
  int i;

	// Generate the clause table for the expression
	ex.create_clause_table();

	// Get the number of clauses from the expression clause table and store locally
	num_clauses = ex.clause_table->num_clauses;

	// Find the maximum clause number in the expression
	max_clause = -1;
	for (i=0; i<num_clauses; i++) {
	  if (ex.clause_table->clauses[i]->clause_number > max_clause)
	    max_clause = ex.clause_table->clauses[i]->clause_number;
	}
	
	// Create the derivate table arrays
	derivatives_x = new expression_t *[max_clause+1];
	derivatives_y = new expression_t *[max_clause+1];
	derivatives_z = new expression_t *[max_clause+1];

	// Zero out all the pointers in the derivative table array, since 
	// not all clause numbers are necessarily valid at this point
	// due to expression pruning

	for (i=0; i<(max_clause+1); i++) {
	  derivatives_x[i] = NULL;
	  derivatives_y[i] = NULL;
	  derivatives_z[i] = NULL;	  
	}

	// Compute the derivative w.r.t x, y, z and store
	for (i=0; i<num_clauses; i++) {
	  // a special version of prune would be needed for pruning here,
	  // because this pruning is over the whole interval.

		int c = ex.clause_table->clauses[i]->clause_number;
		derivatives_x[c] = new expression_t();
		derivatives_x[c]->derivative(ex.clause_table->clauses[i], 0);
				    
		derivatives_y[c] = new expression_t();
		derivatives_y[c]->derivative(ex.clause_table->clauses[i], 1);
		
		derivatives_z[c] = new expression_t();
		derivatives_z[c]->derivative(ex.clause_table->clauses[i], 2);
				
	}

}

// DESTRUCTOR
derivative_table_t::~derivative_table_t() {
  // Delete all the stored expressions
  for (int i=0; i< (max_clause+1); i++) {
    delete derivatives_x[i];
    delete derivatives_y[i];
    delete derivatives_z[i];
  }
  delete []derivatives_x;
  delete []derivatives_y;
  delete []derivatives_z;
}

// EVALUATE NORMAL (OR ANTINORMAL) TO A SHAPE FROM A GIVEN CLAUSE AT A GIVEN POINT
vector_t derivative_table_t::evaluate_normal(vector_t point, int clause) {
	vector_t normal;
	space_interval_t p;

	p.set_point(point.x, point.y, point.z);

	interval_t xv, yv, zv;
	xv = derivatives_x[clause]->eval(p);
	yv = derivatives_y[clause]->eval(p);
	zv = derivatives_z[clause]->eval(p);

	normal.set(xv.get_lower(), yv.get_lower(), zv.get_lower());
	return(normalize(normal));
}

ostream& operator<<(ostream &s, const derivative_table_t &dt) {
	s << "----- Expression Derivative Table with " << dt.num_clauses << " entries ---- " << endl; 
	for (int i=0; i < dt.num_clauses; i++) {
		s << "Clause " << i << ":" << endl;
		s << "     X: " << *dt.derivatives_x[i] << endl;
		s << "     Y: " << *dt.derivatives_y[i] << endl;
		s << "     Z: " << *dt.derivatives_z[i] << endl;
	}
	return(s);
}
