#pragma once
#include <string>

class math_string_gl_texture
{
	public:
		math_string_gl_texture();
		~math_string_gl_texture();

		void set_math_string(const std::string new_math_string);

		void bind();

		unsigned int get_nx() const {return nx;}		
		unsigned int get_ny() const {return ny;}		

		double get_min_x() const {return min_x;}
		double get_min_y() const {return min_y;}

		
		double get_max_x() const {return max_x;}
		double get_max_y() const {return max_y;}

		void set_max_x(double new_max_x);
		void set_max_y(double new_max_y);
		void set_min_x(double new_min_x);
		void set_min_y(double new_min_y);

	private:
		bool dirty;
		bool size_changed;
		unsigned int texture_id;
		std::string math_string;
		int nx;
		int ny;
		double min_x;
		double min_y;
		double max_x;
		double max_y;
};
