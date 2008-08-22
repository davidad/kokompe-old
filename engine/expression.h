//using namespace std;

#include "interval.h"
#include "space_interval.h"
#include <string>
#include <map>

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
  interval_t *cache;
  int vardep;
  int marked; 
  int ref_count;
  int parity;
 

    //int lazy;  
  void build_children(int num_children_in);
  void reroot(expression_t *subtree, expression_t *root);
  void clause_table_recurser(clause_table_t *clause_table);
  void mark_clause_numbers_recurser(int *max_clause_number);
  int count_clauses();

  static void graph_delete(expression_t *a);
  expression_t* find_duplicate(expression_t * a);
  int replace_references(expression_t *oldp, expression_t *newp);
  void mark_and_combine(expression_t *root);
  interval_t cached_eval_core(space_interval_t &vars, int lx, int ly, int lz, int cache_offset, int parity);
  interval_t prune_core(space_interval_t &vars, int do_prune, int *would_prune, int child_num, expression_t *parent, int do_float_opt, expression_t *root);
  void clear_parity();
  expression_t(const expression_t *a, map<expression_t*,expression_t*>* node_map);
  void copy_core(const expression_t *a, map<expression_t*,expression_t*>* node_map);


  static expression_t* flat_copy(const expression_t &a);

public:
  clause_table_t *clause_table;
  int clause_number;						// which global real-expression-part clause identifying number this expression node is part of


  expression_t();
  expression_t(int num_children_in);
  expression_t(const expression_t &a);
  expression_t(string postfix);
  ~expression_t();
  interval_t eval(space_interval_t &vars);
  interval_t cached_eval(space_interval_t &vars, int lx, int ly, int lz, int cache_offset);  
  interval_t prune(space_interval_t &vars, int do_prune, int *would_prune, int child_num, expression_t *parent, int do_float_opt);
  void derivative(expression_t *in_expression, int d_var);
  void mark_clause_numbers();
  void create_clause_table();
  void create_cache(int size);
  void delete_cache();
  int mark_dependence();
  void unmark();

  void convert_to_graph();
  interval_t eval_core(space_interval_t &vars, int parity);

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
