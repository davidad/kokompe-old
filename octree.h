#ifndef OCTREE_T
#define OCTREE_T

#include "expression.h"
#include "space_interval.h"
#include "vector.h"

class eval_info_t {
 public:
  space_interval_t eval_interval;
  float step[3];
  float step_recip[3];
  float n_recip[3];
  char *results;
  vector_t *result_points;
  int n[3];
};

class octree_t {
private:

  int expression_root;
  interval_t value;
  octree_t **children;
  octree_t *parent;
  space_interval_t space_interval;
  void eval_on_grid_core(eval_info_t *eval_info);
  void eval_zone_on_grid(eval_info_t *eval_info);
  
public:
  expression_t *expression;
  octree_t(expression_t& expression_in, space_interval_t& space_interval_in);
  octree_t(octree_t *parent_in, expression_t *expression_in, space_interval_t &space_interval);
  ~octree_t();
  void eval(int recursion_depth);
  void eval_on_grid(space_interval_t &eval_interval, int nx, int ny, int nz, char *results, vector_t *voxel_centers_from_evaluator);
  interval_t get_value_at_point(float x, float y, float z);
  int eval_at_point(float x, float y, float z);
  int differential_eval(float x1, float y1, float z1, float x2, float y2, float z2);

};

#endif
