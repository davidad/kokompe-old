#include "vvolume.h"
#include <algorithm>

#include <iostream>


double min(double a, double b)
{
	return a < b ? a : b;
}

double max(double a, double b)
{
	return a > b ? a : b;
}

double min(double a, double b, double c)
{
	double min_val = a;
	if(b < min_val) min_val = b;
	if(c < min_val) min_val = c;
	return min_val;		
}

double max(double a, double b, double c)
{
	double max_val = a;
	if(b > max_val) max_val = b;
	if(c > max_val) max_val = c;
	return max_val;		
}

vector3 vvolume::compute_normal(vector3* v)
{
	vector3 a = v[0];
	vector3 b = v[1];
	vector3 c = v[2];
	
	vector3 raw_normal = (b - a) ^ (c - a);
	
	return raw_normal.unit();
}

vector3 vvolume::compute_slope(vector3 normal, vector3 v0)
{
	//we will represent plane in the form ax + by + cz + d = 0 	
	scalar a, b, c, d;
	
	a = normal[0];
	b = normal[1];
	c = normal[2];
	d = -(normal * v0);
	
	//now to get the z axis for a given x,y we will use:
	// z = -(ax + by + d) / c
	// z = (-a / c) * x + (-b / c) * y + (-d / c)
	
	if(abs(c) > 1e-10)
	{
		return -vector3(a / c, b / c, d / c);
	}
	else
	{
		return vector3(0, 0, v0.z());
	}
}

/*
scalar vvolume::get_z(vector2 p, vector3 slope)
{
	return p[0] * slope[0] + p[1] * slope[1] + slope[2];
}
*/

vector3 vvolume::get_color(vector3 base_color, vector3 normal)
{
	//return vector3(0,0,position[2] / 750);
	
	vector3 final_color;
	final_color[0] = base_color[0] * ambient_light_color[0];
	final_color[1] = base_color[1] * ambient_light_color[1];
	final_color[2] = base_color[2] * ambient_light_color[2];
	
	//flip normal if it doesn't face towards screen
	/*if(normal * vector3(0,0,1) < 0)
	{
		lighting_strength *= -1;
	}
	*/
	double lighting_strength = light_direction * normal;
	lighting_strength = (lighting_strength + 1) * 0.25;
    if(!xyz_lighting) {
        final_color[0] += base_color[0] * lighting_strength * light_source_color[0];
        final_color[1] += base_color[1] * lighting_strength * light_source_color[1];
        final_color[2] += base_color[2] * lighting_strength * light_source_color[2];
    } else {
        final_color[0] += base_color[0] * 0.4 * (1 + (vector3(1,0,0)*normal));
        final_color[1] += base_color[1] * 0.4 * (1 + (vector3(0,1,0)*normal));
        final_color[2] += base_color[2] * 0.4 * (1 + (vector3(0,0,1)*normal));
    }

	return final_color;
	
}

struct compare_y
{
	bool operator()(const vector3& a, const vector3& b) const
	{
		return a[1] < b[1];
	}
};


void vvolume::fill_2d_triangle(vector3* v, vector3 tri_color, vector3 slope, vector3 normal)
{
	vector3 final_color = get_color(tri_color, normal);
	
	vector3 sorted_v[3];
	sorted_v[0] = v[0];
	sorted_v[1] = v[1];
	sorted_v[2] = v[2];

	std::sort(sorted_v, sorted_v + 3, compare_y()); //it feels silly to do this for three elements

	vector3& a(sorted_v[0]);
	vector3& b(sorted_v[1]);
	vector3& c(sorted_v[2]);

	double min_x = min(a.x(), b.x(), c.x());
	double max_x = max(a.x(), b.x(), c.x());
	
	double dx1, dx2, dx3;
	if (b.y()-a.y() > 0) dx1=(b.x()-a.x())/(b.y()-a.y()); else dx1=0;
	if (c.y()-a.y() > 0) dx2=(c.x()-a.x())/(c.y()-a.y()); else dx2=0;
	if (c.y()-b.y() > 0) dx3=(c.x()-b.x())/(c.y()-b.y()); else dx3=0;
		
	double y_start = a.y();
	double y_mid = b.y();
	double y_stop = c.y();
		
	for(double y = y_start; y <= y_mid; y += 1)
	{
		double dy = y - a.y();
		double x1 = a.x() + dy * dx1;
		double x2 = a.x() + dy * dx2;
		horz_line(x1, x2, y, min_x, max_x, final_color, slope);
	}
	
	for(double y = y_mid; y <= y_stop; y += 1)
	{
		double dy = y - a.y();
		double x1 = b.x() + (y - b.y()) * dx3;
		double x2 = a.x() + dy * dx2;
		horz_line(x1, x2, y, min_x, max_x, final_color, slope);
	}
	
	
}

void vvolume::horz_line(double x1, double x2, double y, double min_x, double max_x, vector3 line_color, vector3 slope)
{
	if(y < 0 || y >= height)
	{
		return;
	}
	
	double x_start = min(x1, x2);
	double x_end = max(x1, x2);
	double z_start = x_start * slope[0] + y * slope[1] + slope[2];
	double dzdx = slope[0];
		
	//if(x_start < min_x) x_start = min_x;
	//if(x_end > max_x) x_end = max_x;
		
	int int_x_start = int(x_start);
	int int_x_end = int(x_end + 1);
	
	for(int x = int_x_start; x <= int_x_end; x++)
	{
		double dx = double(x + .5) - x_start;
		double z = z_start + dx * dzdx;
		if(x >= 0 && x < width)
		{
			if(z_buffer(x,y) > z)
			{
				colors(x, y) = line_color;
				z_buffer(x, y) = z;
			}
		}
	}
}
		
/*
void broken_fill_2d_triangle(vector3* v, vector3 tri_color, vector3 slope, vector3 normal)
{
	vector3 sorted_v[3];
	sorted_v[0] = v[0];
	sorted_v[1] = v[1];
	sorted_v[2] = v[2];
	current_v = sorted_v;	
	
	
	std::sort(sorted_v, sorted_v + 3, compare_y()); //it feels silly to do this for three elements
	
	double dx1, dx2, dx3;
	
	vector3& a(sorted_v[0]);
	vector3& b(sorted_v[1]);
	vector3& c(sorted_v[2]);
	
	//DEBUG SECTION:
	double area = ((b - a) ^ (c - a)).length();
	double max_dimension = max((a - b).length(), (b - c).length(), (c - a).length());
	//std::cout << "Filling: " << a << ", " << b << ", " << c << std::endl;
	//std::cout << "Area: " << area << " Max_d: " << max_dimension << std::endl;
	
	//END DEBUG SECTION
	
	if (b.y()-a.y() > 0) dx1=(b.x()-a.x())/(b.y()-a.y()); else dx1=0;
	if (c.y()-a.y() > 0) dx2=(c.x()-a.x())/(c.y()-a.y()); else dx2=0;
	if (c.y()-b.y() > 0) dx3=(c.x()-b.x())/(c.y()-b.y()); else dx3=0;

	double start_x;
	double end_x;
	double y;
	double dy = 0;
	
	start_x = end_x = a.x();
	y = a.y();
	
	if(y < 0)
	{
		dy = -y;
		y = 0;
	}
	
	double mid_y = min(b.y(), height - 1);
	double max_y = min(c.y(), height - 1);
	double min_x = min(a.x(), b.x(), c.x());
	double max_x = max(a.x(), b.x(), c.x());
	double dy_mid = min(height - 1, mid_y - a.y());
	double dy_stop = min(height - 1, max_y - a.y());
	
	
	
	int x1, x2;
	
	//std::cout << "Mid_y: " << mid_y << std::endl;
	
	for(dy = 0; dy < dy_mid; dy += 1)
	{
		x1 = int(a.x() + dy * dx1);
		x2 = int(a.x() + dy * dx2);
		int start_x = min(x1, x2);
		int stop_x = max(x1, x2);
		
		//if(start_x < min_x) start_x = min_x;
		//if(stop_x > max_x) stop_x = max_x;
		//horz_line(start_x, stop_x, a.y() + dy, tri_color, slope, normal);
		horz_line(start_x, stop_x, int(a.y() + dy + .5), tri_color, slope, normal);
		//std::cout << "Start/Stop x: " << start_x << " / " << stop_x << std::endl;
	}
	
	for(; dy < dy_stop + 1; dy += 1)
	{
		x1 = b.x() + dy * dx3;
		x2 = a.x() + dy * dx2;
		double start_x = min(x1, x2);
		double stop_x = max(x1, x2);
		
		//if(start_x < min_x) start_x = min_x;
		//if(stop_x > max_x) stop_x = max_x;
		horz_line(start_x, stop_x, int(a.y() + dy + .5), tri_color, slope, normal);
		
		
	}
	
	/*
	
	
	
	if(dx1 > dx2) 
	{
		start_x += dx2 * dy;
		end_x += dx1 * dy;
		
		while(y<=mid_y)
		{
			horz_line(min(min_x,start_x), max(max_x, end_x), y, tri_color, slope, normal);
			y++;
			start_x+=dx2;
			end_x+=dx1;
		}
		end_x = b.x();
		y = mid_y;
		
		while(y<=max_y)
		{
			horz_line(min(min_x,start_x), max(max_x, end_x), y, tri_color, slope, normal);
			y++;
			start_x+=dx2;
			end_x+=dx3;
		}
	} 
	else
	{
		start_x += dx1 * dy;
		end_x += dx2 * dy;
		
		while(y<=mid_y)
		{
			horz_line(min(min_x,start_x), max(max_x, end_x), y, tri_color, slope, normal);
			y++;
			start_x+=dx1;
			end_x+=dx2;
		}
		start_x = b.x();
		y = mid_y;
		while(y<=max_y)
		{
			horz_line(min(min_x,start_x), max(max_x, end_x), y, tri_color, slope, normal);
			y++;
			start_x+=dx3;
			end_x+=dx2;
		}
	}
	

}
*/
/*
void horz_line_old(int start_x, int end_x, int y, vector3 line_color, vector3 slope, vector3 normal)
{
	if(y < 0 || y >= colors.height) return;
	if(start_x > colors.width) return;
	if(end_x < 0) return;
	
	int safe_start_x = start_x - 1;
	int safe_end_x = end_x + 1;
	
	if(safe_start_x < 0) safe_start_x = 0;
	if(safe_end_x >= colors.width) safe_end_x = colors.width - 1;
				
	double max_z = max(current_v[0][2], current_v[1][2], current_v[2][2]);
	double min_z = min(current_v[0][2], current_v[1][2], current_v[2][2]);
	
	for(int x = safe_start_x; x <= safe_end_x; x++)
	{
		double z = (x * slope[0] + y * slope[1] + slope[2]);
		
		#if 1
		if(z > max_z)
		{
			z = (min_z + max_z) / 2;
		}
		else if(z < min_z)
		{
			z = (min_z + max_z) / 2;
		}	

		#endif		
		
		if(z_buffer(x,y) > z)
		{
			colors(x, y) = get_color(line_color, vector3(double(x),double(y),z), normal);
			z_buffer(x, y) = z;
		}
	}
}
	*/
	

vvolume::vvolume(unsigned int new_width, unsigned int new_height, 
	matrix4 new_projection_matrix, vector3 new_ambient_light_color,
	vector3 new_light_direction, vector3 new_light_source_color, bool crazy_lighting)
	: projection_matrix(new_projection_matrix),
	light_direction((projection_matrix * new_light_direction).unit()),
	light_source_color(new_light_source_color),
	ambient_light_color(new_ambient_light_color),
    xyz_lighting(crazy_lighting),
	width(new_width),
	height(new_height),
	colors(width, height),
	z_buffer(width, height)
{
	colors = vector3(70,130,180) / 255.0;
	z_buffer = width * height;
}

void vvolume::draw_triangle(vector3* vertices, vector3 tri_color)
{
	vector3 transformed_vertices[3];
	transformed_vertices[0] = projection_matrix * vertices[0];
	transformed_vertices[1] = projection_matrix * vertices[1];
	transformed_vertices[2] = projection_matrix * vertices[2];
	
	vector3 normal = compute_normal(transformed_vertices);
	vector3 slope = compute_slope(normal, transformed_vertices[0]);
	
	fill_2d_triangle(transformed_vertices, tri_color, slope, normal);
}

void vvolume::draw_triangle(vector3* vertices, vector3 normal, vector3 tri_color)
{
	vector3 transformed_vertices[3];
	transformed_vertices[0] = projection_matrix * vertices[0];
	transformed_vertices[1] = projection_matrix * vertices[1];
	transformed_vertices[2] = projection_matrix * vertices[2];
	
	vector3 transformed_normal = ((projection_matrix * normal) - projection_matrix.get_translation()).unit();
	vector3 computed_normal = compute_normal(transformed_vertices);
	
	/*
	if((computed_normal - transformed_normal).length() > .01)
	{
		std::cout << "computed: " << computed_normal <<  std::endl;
		std::cout << "given: " << transformed_normal <<  std::endl;
		std::cout << "started with: " << normal <<  std::endl;
	
	}
	*/
	
	
	vector3 slope = compute_slope(transformed_normal, transformed_vertices[0]);
	
	//std::cout << "Normal: " << normal << std::endl;
	//std::cout << "Slope: " << slope << std::endl;
	
	
	fill_2d_triangle(transformed_vertices, tri_color, slope, transformed_normal);	
	
}

void vvolume::write_to_stream(std::ostream& os)
{
	ppm_image image(width, height);

	for(int x = 0; x < width; x++)
	{
		for(int y = 0; y < height; y++)
		{
			point location = point(x, y);
			vector3 raw_color = colors(x, y);
			//use z_buffer(x, y) to adjust brightness by dist to screen
			if(raw_color[0] < 0) raw_color[0] = 0;
			if(raw_color[0] > 1) raw_color[0] = 1;
			if(raw_color[1] < 0) raw_color[1] = 0;
			if(raw_color[1] > 1) raw_color[1] = 1;
			if(raw_color[2] < 0) raw_color[2] = 0;
			if(raw_color[2] > 1) raw_color[2] = 1;
				
			raw_color *= 255.0;
			
			unsigned char red = (unsigned char)(raw_color[0]);
			unsigned char green = (unsigned char)(raw_color[1]);
			unsigned char blue = (unsigned char)(raw_color[2]);
			
			image.set_pixel(point(x,y), red, green, blue);
		}			
	}
	
	image.write_to_stream(os);
}

std::ostream& operator<<(std::ostream& os, vvolume& v)
{
	v.write_to_stream(os);
	return os;
}
