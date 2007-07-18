//using namespace std;

#include "interval.h"
#include "space_interval.h"
#include <string>

#ifndef EXPRESSION_T
#define EXPRESSION_T


class expression_t {
private:
  interval_t data;
  interval_t (*evaluator)(const interval_t &a, const interval_t &b);
  expression_t **children;
  int num_children;
  int var;
    //int lazy;  

public:
  expression_t(int num_children_in);
  expression_t(const expression_t &a);
  expression_t(string postfix);
  ~expression_t();
  interval_t eval(space_interval_t &vars);
  interval_t prune(space_interval_t &vars, int do_prune, int *would_prune, int child_num, expression_t *parent);


};

string convert_infix_to_postfix(string infix);

#endif
