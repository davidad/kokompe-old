#include <sstream>
#include <iostream>
#include <string>
#include <vector>
#include <list>
#include "vector2.h"

using namespace std;

// Horrible way to do this, sorry.
template < class T >
string ToString(const T &arg) {
	ostringstream	out;
	out << arg;
	return(out.str());
}


string toolpath_to_epilog(list<list<_vector2<unsigned int> > > data) {
	/*
	 * Forrest Path to EPI converter.
	 *
	 * At the moment we're just doing really simple conversion of the data structure to EPI
	 * ignoring all the important stuff such as speed, rate, power,...
	 */
	string out = "";
	list<list<_vector2<unsigned int> > >::const_iterator set;
	list<_vector2<unsigned int> >::const_iterator coord;	
	
	out += "%-12345X@PJL JOB NAME=boundaries\n";
	out += "E@PJL ENTER LANGUAGE=PCL\n";
	out += "&y1A&l0U&l0Z&u600D*p0X";
	out += "*p0Y*t600R*r0F&y50P&z50S";
	out += "*r6600T*r5100S*r1A*rC%1BIN;";
	// These are hardcoded for 3mm plywood. I think. :P
	out += "XR1000;YP100;ZS5;";
	
	for (set = data.begin(); set != data.end(); set++) {
		coord = (*set).begin();
		out += "PU" + ToString((*coord).x()) + "," + ToString((*coord).y()) + ";";
		if ((*set).begin() == (*set).end()) { continue; }
		coord++;
		for (; coord != (*set).end(); coord++) {
			out += "PD" + ToString((*coord).y()) + "," + ToString((*coord).y()) + ";";
		}
	}
	
	out += "%0B%1BPUE%-12345X@PJL EOJ ";
	return out;
}


int main() {
	
	
}
