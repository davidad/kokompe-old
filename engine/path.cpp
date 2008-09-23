#include "path.h"
#include <string>
#include <sstream>
using namespace std;


path_node_t::path_node_t() {
	type = LINE;
	arc_radius = 0.0f;
}


void path_t::add_vertex(vertex_t *v, vector_t *inside_point, vector_t *outside_point) {
	verticies.push_back(v);
	vertex_inside_point.push_back(inside_point);
	vertex_outside_point.push_back(outside_point);
}

void path_t::add_edge(vertex_t *v1, vertex_t *v2) {
	path_node_t * p;
	p = new path_node_t();
	p->verticies[0] = v1;
	p->verticies[1] = v2;
	all.push_back(p);
}


// Write an SVG file of the path
// for now, just write all of the edges as seperate SVG paths
void path_t::fill_svg(char **buffer, int *length) {
	
	list<path_node_t*>::iterator pi;
	ostringstream fileStream;

	// TODO: assume square for now
	float offset = -bb_x1;
	float factor = 1000.0f / (bb_x2-bb_x1);


	fileStream << "<?xml version=\"1.0\" standalone=\"no\"?>" << endl;
	fileStream << "<!DOCTYPE svg PUBLIC \"-//W3C//DTD SVG 1.1//EN\"" << endl;
	fileStream << "\"http://www.w3.org/Graphics/SVG/1.1/DTD/svg11.dtd\">" << endl;
	fileStream <<  "<svg width=\"100%\" height=\"100%\" version=\"1.1\"" << endl;
	fileStream << "xmlns=\"http://www.w3.org/2000/svg\">" << endl;

	for(pi = all.begin(); pi !=all.end(); pi++) {
		fileStream << "<path d=\"M";
		fileStream << ((offset+(*pi)->verticies[0]->x)*factor) << " ";
		fileStream << ((offset+(*pi)->verticies[0]->y)*factor) << " ";
		fileStream << "L";
		fileStream << ((offset+(*pi)->verticies[1]->x)*factor) << " ";
		fileStream << ((offset+(*pi)->verticies[1]->y)*factor) << " ";
		fileStream << "\" stroke=\"blue\" stroke-width=\"2\" />" << endl;
	}

	fileStream << "</svg>" << endl;

	// Package string into a char buffer
	string file = fileStream.str();
	*length = file.length();
	*buffer = new char[(*length)+1];
	strcpy(*buffer, file.c_str());  //null terminator allocated for but not included in length
	
}
