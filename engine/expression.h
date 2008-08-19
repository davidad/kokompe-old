//using namespace std;

#include "interval.h"
#include "space_interval.h"
#include <string>

#ifndef EXPRESSION_T
#define EXPRESSION_T

class clause_table_t;

class expression_t {
private:
  interval_t data;
  interval_t (*evaluator)(const interval_t &a, const interval_t &b);
  expression_t **children;
  int num_children;
  int var;


    //int lazy;  
  void build_children(int num_children_in);
  void reroot(expression_t *subtree);
  void clause_table_recurser(clause_table_t *clause_table);
  void mark_clause_numbers_recurser(int *max_clause_number);
  int count_clauses();

public:
  clause_table_t *clause_table;
  int clause_number;						// which global real-expression-part clause identifying number this expression node is part of

  expression_t();
  expression_t(int num_children_in);
  expression_t(const expression_t &a);
  expression_t(string postfix);
  ~expression_t();
  interval_t eval(space_interval_t &vars);
  interval_t prune(space_interval_t &vars, int do_prune, int *would_prune, int child_num, expression_t *parent, int do_float_opt);
  void derivative(expression_t *in_expression, int d_var);
  void mark_clause_numbers();
  void create_clause_table();

  friend ostream& operator<<(ostream &s, const expression_t &expr);

};

ostream& operator<<(ostream &s, const expression_t &expr);


string convert_infix_to_postfix(string infix);

class clause_table_t {
public:
	int num_clauses;
	int dirty;
	int next_number;
	expression_t **clauses;

	clause_table_t(int num_clauses_in);
	~clause_table_t();
};





#endif
