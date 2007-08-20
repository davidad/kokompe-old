#include "tool_path.h"
#include "expand.h"
#include <iostream>
#include <cmath>

//Algorithm outline:
//Goal is to generate a series of cuts that will result in the marked regions being cut out
//
//calling compute_tool_path will generate the paths for cutting out the marked regions
//
// Things that would be nice to add:
//     Sorting for order that regions are cut so that outside regions are cut out last
//     Sorting to minimize total tool movement

//
// NEED TO REVERSE ORDER
//

typedef _vector2<unsigned int> lattice_point;
typedef unsigned char cell;
// Cell encoding:
// 0 - off of grid or empty
// 1 - internal point
// 2 - boundery that needs to be cut
// 3 - boundery that will be cut with current tool path and is start of cut
// 4 - boundery that will be cut with current tool path

bool needs_cutting(cell c) {return c == 2;}
void mark_as_cut(cell& c) {c = 4;}

std::ostream& operator<<(std::ostream& os, const lattice_point& p);

template <typename scalar>
std::ostream& operator<<
(std::ostream& os, const _vector2<scalar>& vec)
{
    return os<< "<" << vec.x() << ", " << vec.y() << ">";
}

class cell_array : public array_2d<cell>
{
	public:
		cell_array(unsigned int new_height, unsigned int new_width)
		: array_2d<cell>(new_height, new_width), outside(0)
		{
		}
		cell_array(const array_2d<char>& base)
		: array_2d<cell>(base.width, base.height), outside(0)
		{
			for(unsigned int i = 0; i < size; i++)
			{
				data[i] = base[i];
			}
		}
		cell outside;
	
		bool is_top_row(unsigned int i) const {return (i < width);}
		bool is_bottom_row(unsigned int i) const {return (i + width >= size);}
		bool is_left_column(unsigned int i) const {return ((i % width) == 0);}
		bool is_right_column(unsigned int i) const {return ((i + 1) % width == 0);}
				
		cell& n(unsigned int i){if(!is_top_row(i)) return data[i - width]; else return outside;}
		cell& s(unsigned int i){if(!is_bottom_row(i)) return data[i + width]; else return outside;}
		cell& w(unsigned int i){if(!is_left_column(i)) return data[i - 1]; else return outside;}
		cell& e(unsigned int i){if(!is_right_column(i)) return data[i + 1];	else return outside;}

		cell& ne(unsigned int i){if(!is_top_row(i) && !is_right_column(i)) return data[i - width + 1]; else return outside;}
		cell& se(unsigned int i){if(!is_bottom_row(i) && !is_right_column(i)) return data[i + width + 1]; else return outside;}
		cell& nw(unsigned int i){if(!is_top_row(i) && !is_left_column(i)) return data[i - width - 1]; else return outside;}
		cell& sw(unsigned int i){if(!is_bottom_row(i) && !is_left_column(i)) return data[i + width - 1]; else return outside;}
		
		unsigned int next_ccw_uncut(unsigned int i)
		{
			if(needs_cutting(w(i)))  return i - 1        ;
			if(needs_cutting(sw(i))) return i - 1 + width;
			if(needs_cutting(s(i)))  return i     + width;
			if(needs_cutting(se(i))) return i + 1 + width;
			if(needs_cutting(e(i)))  return i + 1        ;
			if(needs_cutting(ne(i))) return i + 1 - width;
			if(needs_cutting(n(i)))  return i     - width;
			if(needs_cutting(nw(i))) return i - 1 - width;
			return size;
		}
		
		lattice_point as_point(unsigned int i) {return lattice_point(i % width, i / width);}
};

class cut_stroke
{
	public:
		double max_error;
		std::list<lattice_point> points;
		lattice_point start;
		lattice_point end;
		bool add_point(const lattice_point& new_point)
		{
			_vector2<double> line_start(start[0], start[1]);
			double nx = double(start[1]) - double(new_point[1]);
			double ny = double(new_point[0]) - double(start[0]);
			_vector2<double> line_normal(nx,ny);
			normalize(line_normal);
			for(std::list<lattice_point>::iterator i = points.begin();
				i != points.end();
				i++)
			{
				_vector2<double> current_point((*i)[0], (*i)[1]);
				double dist_to_line = ((current_point - line_start) * line_normal);
				if(dist_to_line < 0) dist_to_line *= -1;
				if(dist_to_line > max_error)
				{
					return false;
				}
			}
			//std::cout << std::endl;
			
			points.push_back(new_point);
			end = new_point;
			return true;			
		}
		void set_start(const lattice_point& new_start)
		{
			points.clear();
			start = new_start;
			add_point(new_start);
		}
};

unsigned int mark_boundery_cells(cell_array& cells)
{
	unsigned int current_count = 0;
	for(int i = 0; i < cells.size; i++)
	{
		if(cells[i] != 0)
		{
			if(
				(cells.n(i) == 0) ||
				(cells.e(i) == 0) ||
				(cells.s(i) == 0) ||
				(cells.w(i) == 0)
			){
				cells[i] = 2;
				current_count++;
			}
		}
	}
	
	return current_count;
}


void display(const cell_array& cells, int current)
{
	for(int i = 0; i < cells.size; i++)
	{
		if(i == current)
		{
			std::cout << "X";
		//}else if(cells.is_top_row(i))
		//{
		//	std::cout << int(i % 10);
		//}else if(cells.is_right_column(i))
		//{
		//	std::cout << "r";
		//}else if(cells.is_left_column(i))
		//{
		//	std::cout << int((i / cells.width) % 10);
		//}else if(cells.is_bottom_row(i))
		//{
		//	std::cout << "b";
		}else 
		{
			std::cout << int(cells[i]);
		}

		std::cout << " ";
		if(cells.is_right_column(i)) std::cout << std::endl;
	}
	//std::cin.get();
}


tool_path compute_tool_path(const array_2d<char>& obj, double tool_radius, double max_error)
{
	if(tool_radius < 1.42)
	{
		std::cerr << "Warning: Tool radius must be greater than 1.42, tool path will not be generated" << std::endl;
		//throw; //if the tool is too small we can get weird topologies that will break things
		return tool_path();
	}
	
	//std::cout << "Copying object." << std::endl;
	
	cell_array cells(obj);
	//std::cout << "copy is " << cells.width << " by " << cells.height << " and has size " << cells.size << std::endl;
	//std::cout << "Expanding object." << std::endl;
	expand_object(cells, tool_radius);
	
	//These three variables will hold intermediate parts of the path
	tool_path current_path;
	segment current_segment;
	cut_stroke current_stroke;
	current_stroke.max_error = max_error;
	
	//std::cout << "Marking boundery cells." << std::endl;
	int num_uncut_cells = mark_boundery_cells(cells);
	unsigned int current_cell = 0;
	unsigned int last_cell = 0;
		
	//select a start cell
	while(!needs_cutting(cells[current_cell]) && current_cell < cells.size) current_cell++;
	//and then initialize the current stroke
	current_segment.push_back(cells.as_point(current_cell));
	
	//std::cout << "Generating tool path to cover " << num_uncut_cells << " cells." << std::endl;

	
	while(num_uncut_cells > 0)
	{
		if(current_cell >= cells.size)
		{
			std::cerr << num_uncut_cells << " cells remaining." << std::endl;
			std::cerr << "Could not find new cell to cut." << std::endl;
			break; //stop if there are no connected cells
		}
		//std::cout << num_uncut_cells << " cells remaining." << std::endl;
		
		//build a stroke
		//std::cout << "Generating new stroke." << std::endl;
		current_stroke.set_start(cells.as_point(current_cell));
		//cells[current_cell] = 3; //DUBUG CODE
		//std::cout << "new stroke starts at " << cells.as_point(current_cell) << std::endl;
		for(;;)
		{
			//display(cells, current_cell);
			last_cell = current_cell;
			//std::cout << "examining " << cells.as_point(current_cell) << std::endl;
			current_cell = cells.next_ccw_uncut(current_cell); //get next cell to cut
			if(current_cell >= cells.size)
			{
				break; //stop if there are no connected cells
			}
			//std::cout << "will cut to " << cells.as_point(current_cell) << std::endl;
			if(!current_stroke.add_point(cells.as_point(current_cell)))
			{
				current_cell = last_cell;
				break; //stop if we need to start a new line
			}
			//if(!needs_cutting(cells[current_cell])) std::cerr << cells.as_point(current_cell) << " was already cut." << std::endl;
			mark_as_cut(cells[current_cell]);
			//std::cout << cells.as_point(current_cell) << " marked as cut " << std::endl;
			num_uncut_cells--;
		}
		
		//add the new stroke to the current segment
		//std::cout << "Adding stroke from " << current_stroke.start << " to " << current_stroke.end << " to segment." << std::endl;
		current_segment.push_back(current_stroke.end);
		//do we need to start a new segment?
		if(current_cell >= cells.size)
		{
			//std::cout << "Moving to new segment." << std::endl;
			
			//IT IS VERY IMPORTANT THAT THIS IS ADDED TO THE _FRONT_
			//if not we will cut out sections in the wrong order
			current_path.push_front(current_segment);
			current_segment.clear();
			current_cell = 0;
			while(!needs_cutting(cells[current_cell]) && current_cell < cells.size) current_cell++;
		}
		
	}
	
	//std::cout << "Full path generated." << std::endl;
	
	//display(cells, 0);   AK
	
			
	return current_path;
}


std::ostream& operator<<(std::ostream& os, const lattice_point& p)
{
	long t = -1; //Fix to proper value
	os.write(p, 8);
	os.write(t, 4);
        //"Forrest Path" version below.
	//os << "[" << p[0] << ", " << p[1] << "]";
	return os;
}

std::ostream& operator<<(std::ostream& os, const tool_path& path)
{
	for(tool_path::const_iterator s = path.begin(); s != path.end(); s++)
	{
		os << "{";
		for(segment::const_iterator v = (*s).begin(); v != (*s).end(); v++)
		{
			if(v != (*s).begin())
			{
				os << ", ";
			}
			os << (*v);
		}
		os << "}\n";
	}
	return os;
}


std::ostream& write_scaled_lattice_point(std::ostream& os, const lattice_point& p, 
					 float xstart, float ystart, float xscale, float yscale)
{
  os << "[" << (((float)p[0])*xscale)+xstart  << ", " << (((float)p[1])*yscale+ystart) << "]";
  return os;
}


std::ostream& write_scaled_path(std::ostream& os, const tool_path& path,
				float xstart, float ystart, float xscale, float yscale)
{
	for(tool_path::const_iterator s = path.begin(); s != path.end(); s++)
	{
		os << "{";
		for(segment::const_iterator v = (*s).begin(); v != (*s).end(); v++)
		{
			if(v != (*s).begin())
			{
				os << ", ";
			}
			write_scaled_lattice_point(os, *v, xstart, ystart, xscale, yscale);
		}
		os << "}\n";
	}
	return os;
}


