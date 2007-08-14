#pragma once
#include "math_util.h"
#include "ppm_image.h"
#include "array_2d.h"


class vvolume
{
	private:
		
		const matrix4 projection_matrix;
		const vector3 light_direction;
		const vector3 light_source_color;
		const vector3 ambient_light_color;
		const unsigned int width;
		const unsigned int height;
		array_2d<vector3> colors;
		array_2d<scalar> z_buffer;
	
		vector3 compute_normal(vector3* v);
		vector3 compute_slope(vector3 normal, vector3 v0);
		vector3 get_color(vector3 base_color, vector3 position, vector3 normal);
		void fill_2d_triangle(vector3* v, vector3 tri_color, vector3 slope, vector3 normal);
		void vert_line(int x, double y1, double y2, vector3 line_color, vector3 slope, vector3 normal);
		//void horz_line(int start_x, int end_x, int y, vector3 line_color, vector3 slope, vector3 normal);
		void horz_line(double x1, double x2, double y, double min_x, double max_x, vector3 line_color, vector3 slope);
	public:
		vvolume::vvolume(unsigned int new_width, unsigned int new_height, 
			matrix4 new_projection_matrix, vector3 new_ambient_light_color,
			vector3 new_light_direction, vector3 new_light_source_color);
		void vvolume::draw_triangle(vector3* verticies, vector3 tri_color);
		void vvolume::draw_triangle(vector3* verticies, vector3 normal, vector3 tri_color);
		void write_to_disk(const char* filename);
};
