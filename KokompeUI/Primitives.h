#pragma once
#include <string>
#include <stdlib.h>
#include "Base.h"

using namespace std;

/*
 *  Each primitive MUST implement:
 *
 *	string GetMathString()
 *	pair<vector<string>, vector<string> > GetProperties()		- first vector is property names, second is descriptions.
 *	pair<vector<void*>, vector<int> > GetPropertyReferences()	- first vector is pointers to the property variables, second is:
 *
 *		0	for void  (should NEVER be used)
 *		1	for signed integer
 *		2	for unsigned integer
 *		3	for double precision floating point number
 *		100	for string
 *	
 *	If you add something to this list, add handlers for the same at relevant locations. Any unknown value will be handled as an integer.
 *
 */

class Primitive {
	public:
		Primitive() {}
		virtual string GetMathString() { return ""; }
};

class MathString: public Primitive {
	public:
		string GetMathString() { return str; }
		void SetMathString(string s) { str = s; }
	protected:
		string str;
};

class Circle: public Primitive {
	public:
		Circle(int x, int y, double r): x(x), y(y), r(r) {};
		Circle(double r): r(r) { x = 0; y = 0; }
		Circle() { x = 0; y = 0; r = 0; }
		// string GetMathString() { return string("((x+") + string(x) + string(")^2 + (y+") + string(y) + string(")^2 < ") + string(r) + string(")"); }
		void SetX(int xval) { x = xval; }
		void SetY(int yval) { y = yval; }
		void SetRadius(double rad) {r = rad; }
	private:
		int x;
		int y;
		double r;
};

class Rectangle: public Primitive {
	public:
		Rectangle(int x1, int y1, int x2, int y2): m_x1(x1), m_x2(x2), m_y1(y1), m_y2(y2) {};
		Rectangle() { m_x1 = m_x2 = m_y1 = m_y2 = 0; };
		// TODO: FIXME: Need to assert   x2 > x1  &c...
		// string GetMathString() { return "( (x > " +itoa(m_x1) + ") & (x < " + itoa(m_x2) + ") & (y > " + itoa(m_y1) + ") & (y < " + itoa(m_y2) + ") )"; };
		void SetX(int x1, int x2) { m_x1 = x1; m_x2 = x2; };
		void SetY(int y1, int y2) { m_y1 = y1; m_y2 = y2; };
	private:
		int m_x1;
		int m_x2;
		int m_y1;
		int m_y2;
};

