
//go2

//#include <stdfx.h>

#include <stack>
#include <vector>
#include <list>
#include <iostream>
#include <cmath>
using namespace std;


#include <stdlib.h>
#include <stdio.h>
#include <errno.h>

#include "expression.h"

// Functions and function pointer list
// unary minus is discovered by context in infix parser (a special case) and
// converted to the specical character shown here

// In the infix representation, unary operators are prefix (e.g. -, sin, !)

static const int fcn_num = 18;
static const string fcn_name[] = {"+", "-", "*", "/", ">", "<", "==", "~", "&", "|", "**", "<=", ">=", "sin", "cos", "sqrt", "m","exp"};
static interval_t (*fcn_evaluator[])(const interval_t&, const interval_t&) = {
  &(interval_t::add),
  &(interval_t::sub),
  &(interval_t::mul),
  &(interval_t::div),
  &(interval_t::greater_than),
  &(interval_t::less_than),
  &(interval_t::equals),
  &(interval_t::bool_not),
  &(interval_t::bool_and),
  &(interval_t::bool_or),

  &(interval_t::power),
  &(interval_t::less_than_or_equals),           // hack  < = <=   !!!!!
  &(interval_t::greater_than_or_equals),       // hack  > = >=   !!!!!
  &(interval_t::sin), 
  &(interval_t::cos),
  &(interval_t::sqrt),
  &(interval_t::unary_minus),
  &(interval_t::exp)};



static const int fcn_args[] = {2, 2, 2, 2, 2, 2, 2, 1, 2, 2, 2, 2, 2, 1, 1, 1, 1,1};  

// This isn't used anymore --- just put 0 for new opeators (AK - 2/22/08)
static const int fcn_lazy[] = {0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0,0};

// operator precedence
static const unsigned int fcn_precedence[] = {13, 13, 12, 12, 18, 18, 18, 10, 15, 17, 9, 18, 18, 5, 5, 5, 11,5};
static const unsigned int highest_precedence = 1;
static const unsigned int lowest_precedence = 24; 

// operator associativity in infix string
// BY PRECEDENCE GROUP
//  --- 0 = left, 1 = right
// starting at precedence group "0"
static const int assoc[] = {0, 0, 0, 0, 0, 1, 0, 0, 0, 1,
			    1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 
			    0, 0, 0, 0, 0};

// Operator precedence and associativity from
// http://www.ibiblio.org/g2swap/byteofpython/read/operator-precedence.html
//
// also, unary operators are assumed to be prefix operators in the infix string // and so MUST be right-associative

// Varible list of function parsing.
// There are only three variables, X, Y, and Z
// corresponding to intervals of the three space dimensions

static const int var_num = 6;
static const string var_name[] = {"X", "Y", "Z", "x", "y", "z"};
static const int var_code[] = {0, 1, 2, 0, 1, 2};  



// This is a mathematical expression.  It is constructed by passing
// in an infix expression, and can be evaluated to produce a result.
// It is stored as an abstract syntax tree, with function pointers,
// lightning-fast evaluation and pruning as we descend into the octree.


// Plain Constructor (used when taking derivatives)
expression_t::expression_t() {
	num_children = 0;
	children = NULL;
	evaluator = NULL;
	var = -1;
	clause_number = -1;
	clause_table = NULL;
}

// Basic Internal Constructor
expression_t::expression_t(int num_children_in) {
  num_children = num_children_in;
  children = new expression_t*[num_children];
  var = -1;
  clause_number = -1;
  clause_table = NULL;
}

// Copy Constructor
expression_t::expression_t(const expression_t &a) {
  int i;
  
  num_children = a.num_children;
  var = a.var;
  data = a.data;
  evaluator = a.evaluator;
  //lazy = a.lazy;
  children = new expression_t*[a.num_children];
  
  for (i=0; i<a.num_children; i++) {
    children[i] = new expression_t(*(a.children[i])); 
  }
   clause_number = a.clause_number;
  // Do not bother to copy the clause table when copying an expression --- it can be
  // regenerated if needed
  clause_table = NULL;

}

// Destructor
expression_t::~expression_t() {
  int i;
  
  if (children != NULL) {

  for (i=0; i<num_children; i++) {
    delete children[i]; 
  }
  delete []children;    
  }
  delete clause_table;
}


// Constructor
// Creates an expression from a postfix expression.
expression_t::expression_t(string postfix) {
  expression_t *tmp;
  float x;
  int i,j;
  int done;
  interval_t y;

  // Start with an empty clause table - not used in this fcn
  clause_number = -1;
  clause_table = NULL;

  // Debugging: print function address list:
  //for (i=0; i< fcn_num; i++) {
  //  cout << "function " << fcn_name[i] << " address " << (long)fcn_evaluator[i] << "\n";
  //}

  
  //string postfix = infix_to_postfix(infix);
  string token;
  stack<expression_t*, vector<expression_t*> > parse_stack;

  int delimeter = 0;
  
  // Parses a postfix string expression and converts to
  // the abstract syntax tree internal representation
  // The postfix expression is space-delimeted by
  // single spaces.
  
  // See http://www.math.ucla.edu/~nathan/10b.1.06w/lectures/day25.html
  
  // Here is how to convert a postfix expression into an expression tree:
  //cout << "starting up.\n";
  //cout << "postfix: " << postfix << "\n";
  delimeter = 1;
  
  while(delimeter >=0) {
    //cout << "start of loop" << "\n";

    // 1. Examine the next element in the input.
    delimeter = postfix.find_first_of(" ");
    token = postfix.substr(0, delimeter);
    if (delimeter >= 0)
      postfix = postfix.substr(delimeter+1, postfix.npos);
    else
      postfix = "";
    
    done = 0;
    //cout << "delimeter:" << delimeter << " token: '" << token << "' postfix: '" << postfix << "'\n";

    

    // OPERATORS
    for(i=0; i < fcn_num; i++) {
      //  3. If it is an operator, then
      //cout << "operator compare string is: '" << token << "' and function name string is '" << fcn_name[i] << "'\n"; 

      if (token.compare(fcn_name[i]) == 0) {
	//cout << "operator\n";
	// 1. create a node
	tmp = new expression_t(fcn_args[i]);
	// 2. copy the operator in data part
	tmp->evaluator = fcn_evaluator[i];
	//tmp->lazy = fcn_lazy[i];
	// 3. POP address of node from stack and assign it to node->right_child
	// 4. POP address of node from stack and assign it to node->left_child   (extended here for arb # of args)
	for (j=fcn_args[i]-1; j >=0; j--) {
	  //cout << "about to top.\n";
	  tmp->children[j] = parse_stack.top();
	  //cout << (long)tmp->children[j];

	  //cout << "about to pop.\n";
	  parse_stack.pop();
	  //cout << "popped.\n";
	}  
	//cout << "children: " << (long)tmp->children[0] << " " << (long)tmp->children[1] << "\n";
	// 5. PUSH new node's address on stack 
	parse_stack.push(tmp);
	done = 1;
	break;
      }
    }
    
    // VARIABLES (e.g. X, Y, Z)
    if (done == 0)
      for(i=0; i < var_num; i++) {
	//  2. If it is an operand then
	if (token.compare(var_name[i]) == 0) {
	  //cout << "variable\n";
	  // 1. create a leaf node, i.e. a node having no child
	  tmp = new expression_t(0);
	  // 2. copy the operand in data part
	  tmp->var = var_code[i];
	  // 3. PUSH node's address on stack 
	  parse_stack.push(tmp);
	  done = 1;
	  break;
	}
      }
    
    // NUMBERS 
    if (done==0) {
      //cout << "number\n";
		// Parse Numeric Constant
		// Could be True, False, or something recognized by atof
		if ((token.compare("False") == 0) || (token.compare("false") == 0) || (token.compare("FALSE") == 0)) {
		  y.set_boolean(0);
		  errno = 0;
		}
		else if ((token.compare("True") == 0) || (token.compare("true") == 0) || (token.compare("TRUE") == 0)) {
		  y.set_boolean(1);
		  errno = 0;
		}
		else {			 
		  x = (float)atof(token.c_str());
		  y.set_real_number(x);  // construct a new interval equal to this number
		  //cout << "number is " << x << "\n";
		}
        if (errno) {
	        cout << "Error parsing postfix expression.";
			exit(189);
		}
      else {
	// 1. create a leaf node, i.e. a node having no child
	tmp = new expression_t(0);
	// 2. copy the operand in data part       
	tmp->data = y;
	// 3. PUSH node's address on stack 
	parse_stack.push(tmp);
	  }
	}
    // 4. If there is more input go to step 1      
  }
  
  // 5. If there is no more input, POP the address from stack, which is the address of the ROOT node of Expression Tree. 
  
  // The node on the top of the stack is the root of the tree.
  // But we want THIS node to be the root of the tree.
  
  // Get the root node
  tmp = parse_stack.top();
  parse_stack.pop();

  //cout << "children: " << (long)tmp->children[0] << " " << (long)tmp->children[1] << "\n";
  
  
  // Make a shallow copy, pointing to the same children
  //  children = tmp->children;
  children = new expression_t*[tmp->num_children];
  for(i=0; i<tmp->num_children; i++) {
    children[i] = tmp->children[i];
  }

  num_children = tmp->num_children;
  var = tmp->var;
  evaluator = tmp->evaluator;
  //lazy = tmp->lazy; 
  data = tmp->data;
 
  //cout << "children: " << (long)children[0] << " " << (long)children[1] << "\n";

  // Delete old root node
  tmp->num_children = 0;    // Keep children from getting recursively deleted

  //cout << "children: " << (long)children[0] << " " << (long)children[1] << "\n";



  delete tmp;

  //cout << "children: " << (long)children[0] << " " << (long)children[1] << "\n";

}


// Evaluate an expression

interval_t expression_t::eval(space_interval_t &vars) {
  /*  interval_t null_interval;  */
  interval_t reg;

  /*interval_t arg1;
  interval_t arg2;
  interval_t result;*/

  /*  cout << "entering eval \n";
  //cout << "num_children: " << num_children << "\n";
  //cout << "var: " << var << "\n";
  //cout << "data: " << data << "\n";
  //cout << "this: " << (long)this << "\n";
  //for (i=0; i < num_children; i++) {
  //  cout << "Child " << i << " " << (long)children[i] << "\n";
  //}
  cout << "evaluator: " << (long)evaluator << "\n";*/


  //  return(null_interval);
  
  if (var > -1) {
    return(vars.get_var_value(var));
  }
  else {     
    
    switch (num_children) {
    case 0:
      return(data);
      break;
    case 1:
      return( (*evaluator)(children[0]->eval(vars), reg) );
      break;
    case 2:
      
      /* this implementation of lazy evaluation was faster
	 than the one below, but it was still slower than
	 brute force. 

        if (lazy) {
	reg = children[1]->eval(vars);
	if (reg.is_true() && (evaluator == &(interval_t::bool_or ))) {
	  return(reg);
	}
	else if (reg.is_false() && (evaluator == &(interval_t::bool_and))) {
	  return(reg);
	}
	else {
	  return((*evaluator)(children[0]->eval(vars), reg));
	}
      }
      else { */

	return((*evaluator)(children[0]->eval(vars),  children[1]->eval(vars)));

	//}

	


      
      /*arg1 = children[0]->eval(vars);
      // Lazy evaluation (in benchmarking makes code run SLOWER 
      if ( arg1.is_resolved() && 
	   ( ( (evaluator == &(interval_t::bool_or )) && arg1.is_true()  ) || 
	     ( (evaluator == &(interval_t::bool_and)) && arg1.is_false() ) ) ) {
	// Arg 2 is irrelevant --- just put anything
	arg2.set_boolean(0);
      }
      else {
	// Arg 2 is needed. Evaluate it!
	arg2 = children[1]->eval(vars);
      }
      arg2 = children[1]->eval(vars);
      result = (*evaluator)( arg1 , arg2 );
      return(result );
	
      return((*evaluator)(children[0]->eval(vars),  children[1]->eval(vars)));*/
	

      break;
    }
  }
  return(reg);
}


// Re-roots a subtree at the current expression node: shallow-deleting the contents
// of the current node and replacing it with a shallow copy of the old node, child 
// pointers intact, then deleting the old subtree root node
void expression_t::reroot(expression_t *subtree) {
	  data = subtree->data;
	  evaluator = subtree->evaluator;
	  delete[] children;
	  children = subtree->children;
	  num_children = subtree->num_children;
	  var = subtree->var;

	  subtree->children = NULL;  // array ownership was transfered above --- keep children + children array from getting deleted
	  delete subtree;  // actually only deletes old root node of subtree
}



// Simplifies an expression for faster evaluation on subparts of this evaluation domain
// 
// Specifically: 
//
// Subtrees that evaluate to a real number, TRUE, or FALSE are replaced with a literal
// subtrees rooted by AND operators with TRUE as one arg are are replaced by their other arg
// subtrees rooted by OR operrators with FALSE as one arg are replacd by their other arg
// not of greater than -> less than, etc.
// if do_float_opt = 1:
// Division by a real number literal is replaced with multiplication by the reciprocal
// Addition, Subtraction, Multiplication, or Division by 0 / 1 are simplified (very common with neil.lib)

// There are a whole bunch of other optimizations that could be added, TOOD
// e.g. negation of subtraction reverses args
// cosine of negation can remove negation
// But I don't know if these are worth the time to check..

interval_t expression_t::prune(space_interval_t &vars, int do_prune, int *would_prune, int child_num, expression_t *parent, int do_float_opt) {
  interval_t null_interval;  
  interval_t arg1;
  interval_t arg2;
  interval_t result;
  interval_t tmp;
  expression_t *subtree;


  // default is 0
  *would_prune = 0;


	// Quick-and-dirty invalivation of clause table on any pruning activity.
    // Maybe there is a better place to put this that invalidates less
	if (clause_table != NULL)
		clause_table->dirty = 1;

  /*  cout << "entering eval \n";
  //cout << "num_children: " << num_children << "\n";
  //cout << "var: " << var << "\n";
  //cout << "data: " << data << "\n";
  //cout << "this: " << (long)this << "\n";
  //for (i=0; i < num_children; i++) {
  //  cout << "Child " << i << " " << (long)children[i] << "\n";
  //}
  cout << "evaluator: " << (long)evaluator << "\n";*/


  //  return(null_interval);
  
  if (var > -1) {
    return(vars.get_var_value(var));
  }
  else {     
    
    switch (num_children) {
    case 0:
      // LITERAL 
      return(data);
      break;
    case 1:
      // UNARY OPERATOR
      arg1 = children[0]->prune(vars, do_prune, would_prune, 0, this, do_float_opt);
      result = (*evaluator)(arg1, null_interval);

      // If this subtree is resolved
      if (result.is_resolved()) {
	// Delete this subtree's children and make this a literal leaf
	*would_prune = 1;
	if (do_prune) {
	  delete children[0];
	  num_children = 0;
	  data = result;
	}
      }
      	  // Simplify not greater than to less than, etc. 
	      // This actually gets used a huge amount as the expression is simplified down the tree.
	  else if (evaluator == &(interval_t::bool_not)) {
		  subtree = children[0];
		  if (subtree->evaluator == &(interval_t::greater_than)) {
			  *would_prune = 1;
			  if (do_prune) {
				  this->reroot(subtree);
				  evaluator = &(interval_t::less_than_or_equals);
			  }
		  }
		  else if (subtree->evaluator == &(interval_t::less_than)) {
			  *would_prune = 1;
			  if (do_prune) {
				  this->reroot(subtree);
				  evaluator = &(interval_t::greater_than_or_equals);
			  }
		  }
		  if (subtree->evaluator == &(interval_t::greater_than_or_equals)) {
			  *would_prune = 1;
			  if (do_prune) {
				  this->reroot(subtree);
				  evaluator = &(interval_t::less_than);
			  }
		  }
		  else if (subtree->evaluator == &(interval_t::less_than_or_equals)) {
			  *would_prune = 1;
			  if (do_prune) {
				  this->reroot(subtree);
				  evaluator = &(interval_t::greater_than);
			  }
		  }
	  }



      return(result);
      break;
    case 2:
      // BINARY OPERATOR
      arg1 = children[0]->prune(vars, do_prune, would_prune, 0, this, do_float_opt);

      // Lazy evaluation
      if ( arg1.is_resolved() && 
	   ( ( (evaluator == &(interval_t::bool_or )) && arg1.is_true()  ) || 
	     ( (evaluator == &(interval_t::bool_and)) && arg1.is_false() ) ) ) {
	// Arg 2 is irrelevant, both children will get deleted --- just put anything
	arg2.set_boolean(0);
      }
      else {
	// Arg 2 is needed. Evaluate it!
	arg2 = children[1]->prune(vars, do_prune, would_prune, 1, this, do_float_opt);
      }
      result = (*evaluator)( arg1 , arg2 );
      

      // If this node evaluates to a literal
      if (result.is_resolved()) {
	// Delete this subtree's children and make this a literal leaf
	// Note: This handles a lot of supposed other cases,
	// such as AND with false or OR with true, or boolean negation of true or false
	*would_prune = 1;
	if (do_prune) {
	  delete children[0];
	  delete children[1];
	  num_children = 0;
	  data = result;
	}
      }
      // If this node is an AND with one TRUE arg
      else if  ( (evaluator ==  &(interval_t::bool_and)) &&
		  (arg1.is_true() || arg2.is_true() )) {
		// (parent != NULL) ) {
	*would_prune = 1;
	if (do_prune) {
	// Make this support anding with true at the expression root as well,
    // by shallow copying the non-true arg into the current node, then shallow-deleting
	// the two original child nodes

	  if (arg1.is_true()) {
	    subtree = children[1];
	    delete children[0];
	  }
	  else {
	    subtree = children[0];
	    delete children[1];
	  }
	  this->reroot(subtree);

	}
      }
      // If this node is an OR with one FALSE arg
      else if  ( (evaluator ==  &(interval_t::bool_or)) &&
		  (arg1.is_false() || arg2.is_false() )) {
		  //(parent != NULL)) {
	*would_prune = 1;
	if (do_prune) {
	  if (arg1.is_false()) {
	    subtree = children[1];
	    delete children[0];
	  }
	  else {
	    subtree = children[0];
	    delete children[1];
	  }
	  this->reroot(subtree);
	}
	  }

	  else if (do_float_opt) {


	   // NOTE: In this sequence, if a case is met, no further cases will be checked.  I think this is OK
	   // due to the details --- but if you are adding a new optimization here, you should think it through
	   // and make sure these are all actually mutually exclusive.

	  // Check for identity element 0 on arg1
	   if (arg1.is_real_number() && arg1.get_lower() == 0) {
		  if (evaluator == &(interval_t::add)) {
			  // Adding zero : replace add with other arg
			  *would_prune = 1;
			  if (do_prune) {
				subtree = children[1];
				delete children[0];
				reroot(subtree);
			  }
		  }
		  else if (evaluator == &(interval_t::sub)) {
			  // 0 - arg --> unary minus arg
			  *would_prune = 1;
			  if (do_prune) {
			    delete children[0];  // delete the zero
				evaluator = &(interval_t::unary_minus);
				subtree = children[1]; // save the right argt
				delete[] children;  // replace the old 2-long children array with a 1-long children array
				num_children = 1;
				children = new expression_t*[1];
				children[0] = subtree;  // make the right arg the child
			  }
		  }
		  else if (evaluator == &(interval_t::mul)) {
			  // Multiplying by zero --- replace multiply with literal zero (which will then recurse up)
			  *would_prune = 1;
			  if (do_prune) {			  
				subtree = children[0];
				delete children[1];
				reroot(subtree);
			  }
		  }
	  }
	  // Check for identity element 0 on arg2
	  else if (arg2.is_real_number() && arg2.get_lower() == 0) {
		  if (evaluator == &(interval_t::add)) {
			  // Adding zero : replace add with other arg
			  *would_prune = 1;
			  if (do_prune) {	
			  subtree = children[0];
			  delete children[1];
			  reroot(subtree);
			  }
		  }
		  else if (evaluator == &(interval_t::sub)) {
			  // Subtracting zero : replace sub with first arg
			  *would_prune = 1;
			  if (do_prune) {	
			  subtree = children[0];
			  delete children[1];
			  reroot(subtree);
			  }
		  }
		  else if (evaluator == &(interval_t::mul)) {
			  // Multiplying by zero --- replace multiply with literal zero (which will then recurse up)
			 *would_prune = 1;
			  if (do_prune) {				  
			  subtree = children[1];
			  delete children[0];
			  reroot(subtree);
			  }
		  }
	  }
	  // DOES DIVISION BY 1 PROPAGATE THROUGH?  OR SHOULD IT BE EXPLICITLY HANDLED?
	  // Check for identity element 1 on arg1
	  else if (arg1.is_real_number() && arg1.get_lower() == 1) {
		  if  (evaluator == &(interval_t::mul)){
			  // Multiplying 1 * expression --- just keep expression
			  *would_prune = 1;
			  if (do_prune) {			  
				subtree = children[1];
				delete children[0];
				reroot(subtree);
			  }
		  }
	  }
	  // Check for identity element 1 on arg2
	  else if (arg2.is_real_number() && arg2.get_lower() == 1) {
		  if ((evaluator == &(interval_t::mul)) || (evaluator == &(interval_t::div))) {
			  // Multiplying or Dividing  expression * 1
			  *would_prune = 1;
			  if (do_prune) {			  
				subtree = children[0];
				delete children[1];
				reroot(subtree);
			  }
		  }
	  }

      // If this node is DIV by a real number literal, replace with mul by recip
	  // Note: We need to create a new leaf here because the RHS literal might be a whole evaluated subtree, not a leaf
      else if ( (evaluator == &(interval_t::div)) &&
		(arg2.is_real_number()) ) {
	*would_prune = 1;

	if (do_prune) {
	//	cout << *this << "\n";
		
		// Create a new literal leaf			
	  subtree = new expression_t(0);
	  // Set to reciprocal of arg2
	  subtree->data.set_real_number(1.0f/arg2.get_lower());
	 // tmp.set_real_number(1.0);
	 // subtree->data = interval_t::div(tmp, arg2);	  
	  // Delete old arg2
	  delete children[1];	  
	  // Replace with new literal leaf
	  children[1] = subtree;
	  // Change op to multiply
	  evaluator = &(interval_t::mul);

	//  cout << *this << "\n";
	}
      }
	  }
      return(result);
      break;
    }
  }
  return(null_interval);
}

void expression_t::build_children(int num_children_in) {
	num_children = num_children_in;
	children = new expression_t*[num_children_in];	
	for(int i=0; i < num_children_in; i++)
		children[i] = new expression_t();

}

int float_equals(float x, float y) {
  float tol = ( fabsf(x) + fabsf(y) )*1e-6f;
  
  if (fabsf(x-y) < tol) 
    return(1);
  else
    return(0); 
}


/** Derivative
 * This method sets an empty expression to the symbolic derivative of the supplied expression_in, 
 with respect to the variable with code var 
 (This is used to find surface normals to the object)
 
 THIS METHOD ASSUMES AN EMPTY EXPRESSION, FOR NOW.  WOULD BE EASY TO FIX. */


void expression_t::derivative(expression_t *expression_in, int d_var) {
  //cout << "starting derivative";
  if (expression_in->num_children == 0) {
    // Not an operator --- either a variable or constant
    // If it is the differention variable, the derivative is 1 --- for any other variable or constant, it is 0.
    //cout << "expression_in -> var " << expression_in->var << " d_var " << d_var << endl;
    
    if (expression_in->var == d_var) 
      data.set_real_number(1.0f);
    else
      data.set_real_number(0.0f);
    var = -1;
  }
  else {
    //cout << "found operator";
    // Derivative of an operator expression
    if ((expression_in->evaluator) == &(interval_t::add)) {
      // ADDITION
      // Derivative of a sum is the sum of the derivatives
      evaluator = &(interval_t::add); 	
      build_children(2);
      children[0]->derivative(expression_in->children[0], d_var);
      children[1]->derivative(expression_in->children[1], d_var);
      
    }
    // For our purposes in finding normals, greater than, less than, and equals
    // all have a derivative equivilent to MINUS
    else if (((expression_in->evaluator) == &(interval_t::sub)) ||
	     ((expression_in->evaluator) == &(interval_t::greater_than)) ||
	     ((expression_in->evaluator) == &(interval_t::less_than)) ||
	     ((expression_in->evaluator) == &(interval_t::equals)) ||
				 ((expression_in->evaluator) == &(interval_t::less_than_or_equals)) ||
				 ((expression_in->evaluator) == &(interval_t::greater_than_or_equals))) {
			// SUBTRACTION
			// Derivative of a difference is the difference of the derivatives
			evaluator = &(interval_t::sub);
			build_children(2);
			children[0]->derivative(expression_in->children[0], d_var);
			children[1]->derivative(expression_in->children[1], d_var);
		}
		else if ((expression_in->evaluator) == &(interval_t::mul)) {
			// MULTIPLICATION
			// Product rule
			evaluator = &(interval_t::add);
			build_children(2);
			
			children[0]->evaluator = &(interval_t::mul);
			children[0]->num_children = 2;
			children[0]->children = new expression_t*[2];	
			children[0]->children[0] = new expression_t(*expression_in->children[0]);		// copy left arg
			children[0]->children[1] = new expression_t();
			children[0]->children[1]->derivative(expression_in->children[1], d_var);		// derivative of right arg

			children[1]->evaluator = &(interval_t::mul);
			children[1]->num_children = 2;
			children[1]->children = new expression_t*[2];
			children[1]->children[0] = new expression_t(*expression_in->children[1]);		// copy right arg
			children[1]->children[1] = new expression_t();
			children[1]->children[1]->derivative(expression_in->children[0], d_var);		// derivative of left arg
		}

		else if ((expression_in->evaluator) == &(interval_t::div)) {
			// DIVISION
			// Quotient rule
			evaluator = &(interval_t::div);
			build_children(2);

			children[0]->evaluator = &(interval_t::sub);
			children[0]->build_children(2);
			
			children[0]->children[0]->evaluator = &(interval_t::mul);
			children[0]->children[0]->num_children = 2;
			children[0]->children[0]->children = new expression_t*[2];	
			children[0]->children[0]->children[0] = new expression_t(*expression_in->children[1]);		// copy demominator
			children[0]->children[0]->children[1] = new expression_t();
			children[0]->children[0]->children[1]->derivative(expression_in->children[0], d_var);		// derivative of numerator

			children[0]->children[1]->evaluator = &(interval_t::mul);
			children[0]->children[1]->num_children = 2;
			children[0]->children[1]->children = new expression_t*[2];	
			children[0]->children[1]->children[0] = new expression_t(*expression_in->children[0]);		// copy numerator
			children[0]->children[1]->children[1] = new expression_t();
			children[0]->children[1]->children[1]->derivative(expression_in->children[1], d_var);		// derivative of denominator

			children[1]->evaluator = &(interval_t::power);
			children[1]->num_children = 2;
			children[1]->children = new expression_t*[2];
			children[1]->children[0] = new expression_t(*expression_in->children[1]);					// copy denomiator
			children[1]->children[1] = new expression_t();
			children[1]->children[1]->data.set_real_number(2.0f);										// squared
		}
		else if ((expression_in->evaluator) == &(interval_t::power)) {
		  // For powers involving differntion variables, this is tricky.
		  // Using x^y = e ^ (ln(x)*y) , I think that the answer is,
		  // for general functions x,y, possibly involving the differention variable:
		  // deriv(x^y) = e^(ln(x)y)*( (x'*y/x) + ln(x)*y' )
		  // This reduces to the right answer, y*x^(y-1)*x' for y contant, and 
		  // to the right answer for x^x, which is commonly tabulated.
		 
		  // However, interval_t does not support raising to non-constant powers anyway.
		  // So I will leave support for non-constant powers as a TODO to come in with
		  // proper support for ln, exp, etc. and for now, just say 
		  // y' = nx^(n-1)*x'
		  
		  if ((expression_in->children[1]->num_children == 0) &&
		      (expression_in->var = -1)) {
		    // vaild case, raising an expression to a constant power
		    // expressions cannot contain intervals themselves, so just get lower
		    float power = expression_in->children[1]->data.get_lower();
		    
		    evaluator = &(interval_t::mul);
		    build_children(2);
		    children[0]->data.set_real_number(power);
		    children[1]->evaluator = &(interval_t::mul);
		    children[1]->num_children = 2;
		    children[1]->children = new expression_t*[2];	
		    
		    if (float_equals(power,2.0f)) {
		      // for x^2, deriv is 2xx', so just copy left arg
		      children[1]->children[0] = new expression_t(*expression_in->children[0]);		// copy left arg
		    }
		    else {
		      // general case, x^(power-1)
		      children[1]->children[0] = new expression_t();
		      children[1]->children[0]->evaluator = &(interval_t::power);
		      children[1]->children[0]->num_children = 2;
		      children[1]->children[0]->children = new expression_t*[2];
		      children[1]->children[0]->children[0] = new expression_t(*expression_in->children[0]);
		      children[1]->children[0]->children[1] = new expression_t();
		      children[1]->children[0]->children[1]->data.set_real_number(power-1.0f);		      
		    }
		    children[1]->children[1] = new expression_t();
		    children[1]->children[1]->derivative(expression_in->children[0], d_var);		// derivative of left arg
		  }
		  else {
		    cerr << "Error: Derivative of a power raised to a non-contant expression.  Not currently supported." << endl;	
		    exit(232);
		  }
		}
                else if ((expression_in->evaluator) == &(interval_t::exp)) {
		  // EXPONENTIAL
		  // Derivative of exponential is exponential of same arg,
		  // times derivative of arg
		  evaluator = &(interval_t::mul);
		  build_children(2);
		  
		  children[0]->evaluator = &(interval_t::exp);
		  children[0]->num_children = 1;
		  children[0]->children = new expression_t*[1];
		  children[0]->children[0] = new expression_t(*expression_in->children[0]);  	  
		  children[1]->derivative(expression_in->children[0], d_var);
		}
		else if ((expression_in->evaluator) == &(interval_t::sin)) {
			// SINE
			// Derivative of sine is cosine of same argument, times derivative of argument
			evaluator = &(interval_t::mul);
			build_children(2);

			children[0]->evaluator = &(interval_t::cos);
			children[0]->num_children = 1;
			children[0]->children = new expression_t*[1];
			children[0]->children[0] = new expression_t(*expression_in->children[0]);   // copy arg for cos arg
			
			children[1]->derivative(expression_in->children[0], d_var);
		}
		else if ((expression_in->evaluator) == &(interval_t::cos)) {
			// COSINE
			// Derivative of cosine is negative sine of same argument, times derivative of argument
	
			evaluator = &(interval_t::unary_minus);
			build_children(1);

			children[0]->evaluator = &(interval_t::mul);
			children[0]->build_children(2);

			children[0]->children[0]->evaluator = &(interval_t::sin);
			children[0]->children[0]->num_children = 1;
			children[0]->children[0]->children = new expression_t*[1];
			children[0]->children[0]->children[0] = new expression_t(*expression_in->children[0]);   // copy arg for cos arg
			
			children[0]->children[1]->derivative(expression_in->children[0], d_var);
		}
		else if ((expression_in->evaluator) == &(interval_t::sqrt)) {
			// SQUARE ROOT
			// Derivative of square root is 0.5 * x'/sqrt(x)

			evaluator = &(interval_t::mul);
			build_children(2);

			children[0]->data.set_real_number(0.5f);													// multiply by 0.5

			children[1]->build_children(2);
			children[1]->evaluator = &(interval_t::div);
			children[1]->children[0]->derivative(expression_in->children[0], d_var);					// chain rule - mul by deriv of arg
			
			children[1]->children[1]->num_children = 1;
			children[1]->children[1]->children = new expression_t*[1];
			children[1]->children[1]->evaluator = &(interval_t::sqrt);
			children[1]->children[1]->children[0] = new expression_t(*expression_in->children[0]);		// copy arg into sqrt
		}
		else {
			// If unknown, set to 0. (HACK FOR NOW !!!!!)
			data.set_real_number(0.0f);
			var = -1;
		}
	}
}


ostream& operator<< (ostream &s, const expression_t &expr) {
	if (expr.num_children == 0) {
		if (expr.var == -1)
			return(s << expr.data);
		else
			return(s << var_name[expr.var]);
	}
	else {
		for (int i=0; i< fcn_num; i++) {
			if (fcn_evaluator[i] == expr.evaluator) {
				if ((fcn_args[i]) == 1)
					return(s << " " << fcn_name[i] << " ( " << *expr.children[0] << " ) ");
				else
					return(s << " ( " << *expr.children[0] << " ) " << fcn_name[i] << " ( " << *expr.children[1] << " ) "); 
			}
		}
		return(s);
	}
}


void add_token(list<string> &token_list, const string &token) {
	if (token.size() > 0) {
		if (token[0] != ' ') {				// do not add space or empty tokens
			token_list.push_back(token);
			//cout << "Added '" << token << "' token.\n";
		}
	}									
}

#define SPECIAL 0
#define NORMAL 1
#define DELIMETER 2


void print_tokens(list<string> &tokens) {

  list<string>::iterator theiterator;

   for (theiterator = tokens.begin(); theiterator != tokens.end(); theiterator++) {
     cerr << *theiterator;
   }
   cerr << "\n";


   }


// Converts an infix string to a postfix string,
// using operator precedence, associtivity, and symbol
// information from the table at the start o
string convert_infix_to_postfix(string infix) {
	string postfix;
	list<string> tokens;
	string this_char;
	int last_char_type = DELIMETER;
	int this_char_type;
	unsigned int i;	

	string special_characters = "+*/><=~&|~!@#$%^&*|\\:;',?";
	string delimeters = " ()-";
	

	// Tokenize the string into a linked list
	
	// A delimeter or a transition between special and non-special characters
	// triggers the creation of a new token.
	
	// (Negative numbers are treated as positive numbers that get unary minus applied.)

	// The minus sign is included in the list of delimeters, which makes it 
	// always get its own token.  This is a hack, as it prevents the use of
	// multicharacter operators including minus, but I did this so that unary
	// minus would get handled correctly.
	
	// If you want some fun, try evaluating "a = 3 + + + 4" in your C complier.
	// Believe it or not, it is not a syntax error.
	
	
	unsigned int token_start = 0;
	
	//cout << "Infix string is: '" << infix << "' with " << infix.size() << " positions \n"; 
	
	
  for(i=0; i < infix.size(); i++) {
	  
	  // Classify this character
	 // cout << "classify..." << i << "\n";
		this_char = infix.substr(i, 1);
		//cout << "This char: '" << this_char << "'\n";
	  
	  //cout << "special char: '" << this_char.find_first_of(special_characters) << "'\n";
	  //cout << "delimeters: '" << this_char.find_first_of(delimeters) << "'\n";

		if (this_char.find_first_of(delimeters) == 0) {
			this_char_type = DELIMETER;
		} 	
		else if (this_char.find_first_of(special_characters) == 0) {
			this_char_type = SPECIAL;
		}
		else {
			this_char_type = NORMAL;
		}
  		//cout << "This character classified as " << this_char_type << "\n";
		
	  // Handle delimeters
			 // cout << "handle delimeters...";
      if (this_char_type == DELIMETER) {
		// cout << "add delimeter tokens...";
		  // tokenize everything accumulated before the delimeter
		add_token(tokens, infix.substr(token_start, i - token_start));
		// tokenize the delimeter too
		add_token(tokens, infix.substr(i, 1));
		// start the next token after the delimeter
		token_start = i+1;
	  }
	  else {
		if (i > 0) {
			
				//  cout << "check for transition...";
			// Create new tokens at transitions between normal and special chars
			if ((this_char_type == SPECIAL) && (last_char_type == NORMAL) ||
				(this_char_type == NORMAL) && (last_char_type == SPECIAL)) {
				// cout << "add token at transition...";
				add_token(tokens, infix.substr(token_start, i - token_start));
				token_start = i;
			}					
		}			  
	  }
	  last_char_type = this_char_type;
  }	  
  
  // Add the last token
  add_token(tokens, infix.substr(token_start, infix.size() - token_start));


  //cerr << "original tokenized string:" ;
  //print_tokens(tokens);
  //cerr << endl;

  // Handle overloading of "-" character as both unary minus and binary subtraction
  // Go through the string left-to-right.  For all "-" characters, if preceded
  // by an operator or left paren, call it unary minus "m", otherwise leave it
  // subtraction "-"

  list<string>::iterator theiterator;
  int last_says_unary_minus = 1;
  string this_token;
  int j;

  
  for(theiterator = tokens.begin(); theiterator != tokens.end(); theiterator++) {
    this_token = *theiterator;

    if (last_says_unary_minus && (this_token.compare("-") == 0))  {
      theiterator = tokens.erase(theiterator);
      tokens.insert(theiterator, "m");
	  --theiterator;
	//  if (theiterator != tokens.begin())  // insert skips over a token?
	//	  theiterator--;
      //cout << "found unary minus.\n";
    }
	else {
		last_says_unary_minus = 0;
		if ((*theiterator).compare("(") == 0)
			last_says_unary_minus = 1;
		else {
			for (j=0; j < fcn_num; j++) {
			  if (fcn_name[j].compare(*theiterator) == 0) {
			    last_says_unary_minus = 1;
					break;
				}
			}
		}
	}
  }
 
  //cerr << "handled unary minus string: ";
  //print_tokens(tokens);
  //cerr << endl;

  // Now enforce operator precedence and associativity 
  // This implementation uses the straightforward parentheses-insertion
  // method used in the FORTRAN compiler
	
	// Go through the token list , looking for tokens,
	// and insert parentheses to bind the left-and-right (or right) operators
	// to the argument
	
  //print_tokens(tokens);


  int done, paren_count;
  list<string>::iterator paren_iterator;
  unsigned int precedence;
  int args;
  int loopnotdone;
 

  for(precedence = highest_precedence; precedence <= lowest_precedence; precedence++) {
    //cout << "precedence:" << precedence << " with assoc " << assoc[precedence] << "\n";
  
    loopnotdone = 1;
	if (assoc[precedence]) 
		theiterator = --(tokens.end());
	else
		theiterator = tokens.begin();

	while(loopnotdone) {



	//for (theiterator = --(tokens.end()) ;
	// theiterator != tokens.begin() ; 
	// theiterator-- ) {


      //      cout << "past loop\n";
      //cout << *theiterator << "\n";

      // Is this an operator at this precedence level?
      done = 0;
      for (j=0; j < fcn_num; j++) {
	//cout << "considering " << *theiterator << "\n";
	if ((fcn_precedence[j] == precedence) && (fcn_name[j].compare(*theiterator) == 0)) {
	  done = 1;
	  args = fcn_args[j];
          //cout << "found op " << *theiterator << "\n";
	  break;
	}
      }
      
      if (done == 1) {
	// We have a match!  Now insert parens around the immediate
	// left and right arguments of this operator to enforce
	// proper operator precedence and associativity
	//cerr << "processing operator: " << fcn_name[j] << " with " << fcn_args[j] << " args." << endl;

	paren_count = 0;

	// FOR A UNARY OPERATOR: Puts a paren immediately before op, and after end of right arg
	// FOR A BINARY OPERATOR: Puts a paren before left op and after right op
	// Should always write exactly one left and one right paren

	
	if (args == 1) {
	  //cout << "recognized unary operator." << "\n";
	  // For a unary operator, insert the parens directly before the operator
	  theiterator = tokens.insert(theiterator, "(");  
	  
	  theiterator++; 
	  // theiterator++;
	  // print_tokens(tokens);
	  
	  
	}
	else {
	  // For a binary operator, bind the left arg
	  
	  // Left paren
	  for (paren_iterator = --theiterator; paren_iterator != tokens.begin(); paren_iterator--) {
	  // skip over a matched set of parens
	    if ((*paren_iterator).compare(")") == 0)
	      paren_count++;
	  else if ((*paren_iterator).compare("(") == 0)
	    paren_count--;
	  
	  if (paren_count < 0) {
	    cout << "Parse error: mismatched parentheses.\n";
	    //  exit(1);
	  }

	  if (paren_count == 0) {
	    // and insert a left paren here
	    tokens.insert(paren_iterator, "(");
		//--paren_iterator; // TESTING..........
	    //cout << "inserting (\n";
	    //print_tokens(tokens);
	    break;
	  }
	  }

	  // Handle loop end condition: if we are all the way back to the beginning
	  // of the expression, then we never broken out of the loop and the left
	  // parent was not placed.  Re-run the loop body once more for the first
	  // character of the string.

	  if (paren_iterator == tokens.begin()) {
	    // skip over a matched set of parens
	    if ((*paren_iterator).compare(")") == 0)
	      paren_count++;
	    else if ((*paren_iterator).compare("(") == 0)
	      paren_count--;
	    
	    if (paren_count < 0) {
	      //cout << "Parse error: mismatched parentheses.\n";
	      //exit(1);
	    }
	    
	    if (paren_count == 0) {
	      // and insert a left paren here
	      tokens.insert(paren_iterator, "(");
		  //--paren_iterator; /// TESTING...
	      //cout << "insertingat start (\n";
	      //print_tokens(tokens);
	    }
	    
	  }
	  ++theiterator;
	}
	
      
	//cout << "done with (\n";
	
      
      
	//	++theiterator;
	// Right paren
	paren_count = 0;

	
	/*	if (theiterator == tokens.end()) {
	  //cout << "sitting at end\n";
	  tokens.insert(theiterator,")");
	}
	else {
	  //cout << "for right paren sitting on top of " << *theiterator << "\n";
	  }*/
      
      for (paren_iterator = ++theiterator; paren_iterator != tokens.end(); paren_iterator++) {
	  // skip over a matched set of parens
	  if ((*paren_iterator).compare("(") == 0)
	    paren_count++;
	  else if ((*paren_iterator).compare(")") == 0)
	    paren_count--;
	  
	  if (paren_count < 0) {
	    cout << "Parse error: mismatched parentheses.\n";
	    //exit(1);
	  }


	  // do not put the left paren just after an operator.  this is only an issue
	  // when doing binary operators with higher precedence than unary operators,
	  // such as ** and m
	  if (paren_count == 0) {
	    int insert_paren = 1;
	    // check for fcn
	    for (int k=0; k < fcn_num; k++) {
	      if ((*paren_iterator).compare(fcn_name[k]) == 0) {
		insert_paren = 0;
		break;
	      }
	    }

	    // and insert a right paren here
	    if (insert_paren) {
	      tokens.insert(++paren_iterator, ")");
		//--paren_iterator; /// TESTING..
	    //cout << "inserting )\n";
	    //print_tokens(tokens);	    
	    break;
	    }
	  }
      }
      //cout << "done with )\n";
	


      
      theiterator--;


      // print string sofar
      //print_tokens(tokens);
      //cerr << endl;


      }

	  // Loop end conditions
	  if (assoc[precedence]) {
	  
		if (theiterator == tokens.begin())
			loopnotdone = 0;
		else {
			theiterator--;
		}
	  }
	  else {
		  theiterator++;
		  if (theiterator == tokens.end())
			  loopnotdone = 0;

	  }

	}
  }
 
  //cerr << "\n";
  //cerr << "New String:\n";
  
  //print_tokens(tokens);


  // Now that we have taken care of tokenization, precedence, associativity, and
  // overloading disambiguation, convert to postfix string

  // Alg: On open paren, increment paren level counter + discard
  // On non-var token, copy to output and append space
  // On op token, push onto stack with paren level of operator
  // On right paren, decrement paren level counter, discard, pop operator if it is at that paren level
  
  stack<string> op_stack;
  stack<int> op_paren_level_stack;
  int paren_level;
  int is_operator;

  paren_level = 0;
  for(theiterator = tokens.begin(); theiterator != tokens.end(); theiterator++) {
    this_token = *theiterator;  
    //cout << "in loop for " << this_token << "\n";
    
    if (this_token.compare("(") == 0) { 
      paren_level++;
      //cout << "found left paren\n";
    }
    else if (this_token.compare(")") == 0) {
      while( (op_stack.size() > 0) && (op_paren_level_stack.top() == paren_level)) {
	postfix.append(op_stack.top());
	postfix.append(" ");
	op_paren_level_stack.pop();
	op_stack.pop();
      }
      paren_level--;
    }
    else {
      // check for op
      is_operator = 0;
      for (j=0; j < fcn_num; j++) {
	if (fcn_name[j].compare(this_token) == 0) {
	  is_operator = 1;
	  break;
	}	   
      }
      
      if (is_operator) {
	//cout << "is operator" << "\n";
	// Push onto stack
	op_stack.push(this_token);
	op_paren_level_stack.push(paren_level);
      }
      else {
	//cout << "is var" << "\n";
	// Copy to output string
	postfix.append(this_token);
	postfix.append(" ");	
      }
    }
  }

  postfix.erase(postfix.size()-1);
  
	// at this point:
	  // could actually convert to postfix
	  // could just immediately dump to abstract syntax tree (prob. better)
	
	
	
	return(postfix);	
}



// Assigns clause numbers to an expression.  Each real-valued clause gets a new
// number, startng with the > < = >= <= operator, and everything above that stays 
// at -1
void expression_t::mark_clause_numbers() {
	int *max_clause;

	max_clause = new int;
	*max_clause = 0;
	this->mark_clause_numbers_recurser(max_clause);
	delete max_clause;
}

void expression_t::mark_clause_numbers_recurser(int *max_clause) {
	if ((evaluator == &(interval_t::less_than_or_equals)) ||
		(evaluator == &(interval_t::greater_than_or_equals)) ||
		(evaluator == &(interval_t::greater_than)) || 
		(evaluator == &(interval_t::less_than)) ||
		(evaluator == &(interval_t::equals))) {

		// New clause on a real->boolean conversion operator
		clause_number = *max_clause;
		(*max_clause)++;
	}
	else {
		// If not yet to a clause, recurse to any children
		for (int i=0; i<num_children; i++) {
			children[i]->mark_clause_numbers_recurser(max_clause);
		}
	}
}



// Populate's an expression's clause table
void expression_t::create_clause_table() {
	// If there is already a clause table, delete it
	if (clause_table != NULL)
		delete clause_table;
	clause_table = new clause_table_t(this->count_clauses());
	this->clause_table_recurser(clause_table);
	clause_table->dirty = 0;
}


// Walks an expression's tree and fills in clause table entries as it finds them
void expression_t::clause_table_recurser(clause_table_t* clause_table_in) {
	if (clause_number != -1) {
		clause_table_in->clauses[clause_table_in->next_number] = this;
		clause_table_in->next_number++;
	}
	else {
		for(int i=0; i<num_children; i++) {
			children[i]->clause_table_recurser(clause_table_in);
		}
	}
}

// Walks an expression's tree and counts the number of clauses
int expression_t::count_clauses() {
	int clause_sum = 0;
	if (clause_number != -1) {
	  return(1);
	}
	else {
		for(int i=0; i<num_children; i++) {
			clause_sum += children[i]->count_clauses();
		}
		return(clause_sum);
	}
}



/// IMPLEMENTATION OF CLAUSE_TABLE:
// MAY WANT TO MOVE TO SEPERATE FILE


clause_table_t::clause_table_t(int num_clauses_in) {
	num_clauses = num_clauses_in;
	if (num_clauses > 0) {						// This was added to fix a crash on end but didn't help
		clauses = new expression_t*[num_clauses];
	}
	else {
		clauses = NULL;
	}
	next_number = 0;
}

clause_table_t::~clause_table_t() {
	if (num_clauses > 0) {						// This was added for fix a crash on end but didn't help
		delete []clauses;
	}
}
