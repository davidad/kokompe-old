#include "expression.h"
#include "space_interval.h"

#ifndef OCTREE_T
#define OCTREE_T

class eval_info_t {
 public:
  space_interval_t eval_interval;
  float step[3];
  float step_recip[3];
  float n_recip[3];
  char *results;
  int n[3];
};

class octree_t {
private:
  expression_t *expression;
  int expression_root;
  interval_t value;
  octree_t **children;
  octree_t *parent;
  space_interval_t space_interval;
  void eval_on_grid_core(eval_info_t *eval_info);
  void eval_zone_on_grid(eval_info_t *eval_info);
  
public:
  octree_t(expression_t& expression_in, space_interval_t& space_interval_in);
  octree_t(octree_t *parent_in, expression_t *expression_in, space_interval_t &space_interval);
  ~octree_t();
  void eval(int recursion_depth);
  void eval_on_grid(space_interval_t &eval_interval, int nx, int ny, int nz, char *results);
  interval_t get_value_at_point(float x, float y, float z);
  int eval_at_point(float x, float y, float z);
};

#endif
