#ifndef DERIVATIVE_TABLE_T
#define DERIVATIVE_TABLE_T

#include <iostream>
using namespace std;

#include "expression.h"
#include "vector.h"

// A table containting the x/y/z derivatives of a each clause of an expression.
// Used to evaluate surface normals analytically and detect/improve edges/corners.

class derivative_table_t {
private:
	expression_t **derivatives_x;
	expression_t **derivatives_y;
	expression_t **derivatives_z;
	int num_clauses;
	int max_clause;


public:
	derivative_table_t();
	~derivative_table_t();

	void create(expression_t &ex);
	vector_t evaluate_normal(vector_t point, int clause);

    friend ostream& operator<<(ostream &s, const derivative_table_t &dt);
};

ostream& operator<<(ostream &s, const derivative_table_t &dt);


#endif
