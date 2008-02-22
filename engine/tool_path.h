#pragma once

#include <list>
#include "array_2d.h"
#include "vector2.h"
#include <ostream>

//A tool path has some number of segments
//each segment has some number of vertexes
//(For a 3d object you will need one toolpath for each slice)

//all vertecies of the generated toolpath are grid points

typedef std::list<_vector2<unsigned int> > segment;
typedef std::list<segment > tool_path;
tool_path compute_tool_path(const array_2d<char>& obj, double tool_radius, double max_error);

std::ostream& operator<<(std::ostream& os, const tool_path& path);

std::ostream& write_scaled_path(std::ostream& os, const tool_path& path, 
				float xstart, float ystart, float xscale, float yscale);

