
//go2

//#include <stdfx.h>

#include <stack>
#include <vector>
#include <list>
#include <iostream>
using namespace std;


#include <stdlib.h>
#include <stdio.h>
#include <errno.h>

#include "expression.h"

// Functions and function pointer list
// unary minus is discovered by context in infix parser (a special case) and
// converted to the specical character shown here

// In the infix representation, unary operators are prefix (e.g. -, sin, !)

static const int fcn_num = 17;
static const string fcn_name[] = {"+", "-", "*", "/", ">", "<", "==", "~", "&", "|", "**", "<=", ">=", "sin", "cos", "sqrt", "m"};
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
  &(interval_t::less_than),           // hack  < = <=   !!!!!
  &(interval_t::greater_than),       // hack  > = >=   !!!!!
  &(interval_t::sin), 
  &(interval_t::cos),
  &(interval_t::sqrt),
  &(interval_t::unary_minus)};



static const int fcn_args[] = {2, 2, 2, 2, 2, 2, 2, 1, 2, 2, 2, 2, 2, 1, 1, 1, 1};  
static const int fcn_lazy[] = {0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0};

// operator precedence
static const unsigned int fcn_precedence[] = {13, 13, 12, 12, 18, 18, 18, 10, 15, 17, 9, 18, 18, 5, 5, 5, 11};
static const unsigned int highest_precedence = 1;
static const unsigned int lowest_precedence = 24; 

// operator associativity in infix string
// by precedence group
//  --- 0 = left, 1 = right
// starting at precedence group "0"
static const int assoc[] = {0,0,0,1,1,0,0,0,0,0,0,1,0,0,0,1,1,0};  


// Operator precedence and associativity from
// http://www.ibiblio.org/g2swap/byteofpython/read/operator-precedence.html
//
// also, unary operators are assumed to be prefix operators in the infix string and so MUST be right-associative

// Varible list of function parsing.
// There are only three variables, X, Y, and Z
// corresponding to intervals of the three space dimensions

static const int var_num = 3;
static const string var_name[] = {"X", "Y", "Z"};
static const int var_code[] = {0, 1, 2};  



// This is a mathematical expression.  It is constructed by passing
// in an infix expression, and can be evaluated to produce a result.
// It is stored as an abstract syntax tree, with function pointers,
// lightning-fast evaluation and pruning as we descend into the octree.


// Basic Internal Constructor
expression_t::expression_t(int num_children_in) {
  num_children = num_children_in;
  children = new expression_t*[num_children];
  var = -1;
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
  
}

// Destructor
expression_t::~expression_t() {
  int i;
  
  for (i=0; i<num_children; i++) {
    delete children[i]; 
  }
  delete children;     
}


// Constructor
// Creates an expression from a postfix expression.
expression_t::expression_t(string postfix) {
  expression_t *tmp;
  float x;
  int i,j;
  int done;
  interval_t y;


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


// Simplifies an expression for faster evaluation on subparts of this evaluation domain
// 
// Specifically: 
//
// Subtrees that evaluate to a real number, TRUE, or FALSE are replaced with a literal
// subtrees rooted by AND operators with TRUE as one arg are are replaced by their other arg
// subtrees rooted by OR operrators with FALSE as one arg are replacd by their other arg
// Division by a real number literal is replaced with multiplication by the reciprocal


interval_t expression_t::prune(space_interval_t &vars, int do_prune, int *would_prune, int child_num, expression_t *parent) {
  interval_t null_interval;  
  interval_t arg1;
  interval_t arg2;
  interval_t result;
  interval_t tmp;
  expression_t *subtree;



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
      arg1 = children[0]->prune(vars, do_prune, would_prune, 0, this);
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

      return(result);
      break;
    case 2:
      // BINARY OPERATOR
      arg1 = children[0]->prune(vars, do_prune, would_prune, 0, this);

      // Lazy evaluation
      if ( arg1.is_resolved() && 
	   ( ( (evaluator == &(interval_t::bool_or )) && arg1.is_true()  ) || 
	     ( (evaluator == &(interval_t::bool_and)) && arg1.is_false() ) ) ) {
	// Arg 2 is irrelevant, both children will get deleted --- just put anything
	arg2.set_boolean(0);
      }
      else {
	// Arg 2 is needed. Evaluate it!
	arg2 = children[1]->prune(vars, do_prune, would_prune, 1, this);
      }
      result = (*evaluator)( arg1 , arg2 );
      

      // If this node evaluates to a literal
      if (result.is_resolved()) {
	// Delete this subtree's children and make this a literal leaf
	// Note: This handles a lot of supposed other cases,
	// such as AND with false or OR with true
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
		 (arg1.is_true() || arg2.is_true() ) &&
		 (parent != NULL) ) {
	*would_prune = 1;
	if (do_prune) {
	  if (arg1.is_true()) {
	    subtree = children[1];
	    delete children[0];
	  }
	  else {
	    subtree = children[0];
	    delete children[1];
	  }
	  // Remove this node from the tree
	  parent->children[child_num] = subtree;
	  num_children = 0;
	  delete this;
	}
      }
      // If this node is an OR with one FALSE arg
      else if  ( (evaluator ==  &(interval_t::bool_or)) &&
		 (arg1.is_false() || arg2.is_false() ) &&
		 (parent != NULL)) {
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
	  // Remove this node from the tree
	  parent->children[child_num] = subtree;
	  num_children = 0;
	  delete this;
	}
      }
      // If this node is DIV by a real number literal, replace with mul by recip
      else if ( (evaluator == &(interval_t::div)) &&
		(arg2.is_real_number()) ) {
	*would_prune = 1;

	if (do_prune) {
	  // Create a new literal leaf
	  subtree = new expression_t(0);
	  // Set to reciprocal of arg2
	  tmp.set_real_number(1.0);
	  subtree->data = interval_t::div(tmp, arg2);	  
	  // Delete old arg2
	  delete children[1];	  
	  // Replace with new literal leaf
	  children[1] = subtree;
	  // Change op to multiply
	  evaluator = &(interval_t::mul);
	}
      }
      return(result);
      break;
    }
  }
  return(null_interval);
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
     cout << *theiterator;
   }
   cout << "\n";


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
	paren_count = 0;

	
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

	// Looks like this code binds the right arg
   // (and is broken)

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

	  if (paren_count == 0) {
	    // and insert a right paren here
	    tokens.insert(++paren_iterator, ")");
		//--paren_iterator; /// TESTING..
	    //cout << "inserting )\n";
	    //print_tokens(tokens);	    
  break;
	  }
	  }
      //cout << "done with )\n";
	


      
      theiterator--;
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
 
  //cout << "\n";
  //cout << "New String:\n";
  
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
