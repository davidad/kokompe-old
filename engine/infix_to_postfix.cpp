#include <string>
#include <iostream>
using namespace std;

#include "expression.h"

int main() {
  string infix;
  string inputline;
  string postfix;

  // Gobble input until EOF is reached
  while (getline(cin, inputline)) {
      infix += inputline;
  }

  if(infix.length() == 0) {
	return(0);
  }

  // Convert to postfix and print to stdout
  postfix = convert_infix_to_postfix(infix);
  cout << postfix;
  
  return(0);
}
