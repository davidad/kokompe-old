#include <iostream>
using std::cout;
using std::endl;

//This include file has everything for the vector 3 class
#include "vector3.h"

//This file has basic stuff for using with std::ostream
#include "math_ostream.h"


//all of the basic math utility classes are templated so you will probably want a typedef for the classes you are using

typedef double scalar; 
typedef _vector3<scalar > vector3; 

int main(int argc, char** argv)
{
    //I have attempted to make all of the common operators function correctly
    vector3 a = vector3(2.0, 0.0, 0.0);
    vector3 b = vector3(0.0, 1.0, 0.0);
    vector3 c = a ^ b; //cross product (careful though, because the order of operation is wrong)
    vector3 test;
    test[0] = a * b; //array style component reference and dot product
    test = a + b; //sum
    test = a * 2.0; //scaling
    test.y() = a.length();
    test.z() = a.length_squared();
    //...
    
    cout << "a = " << a << endl;
    cout << "b = " << b << endl;
    cout << "c = " << c << endl;
    cout << "test = " << test << endl;

    return 0;
}
