#include "ppm_image.h"

#include <iostream>
#include <fstream>
using namespace std;

ppm_image::ppm_image(int width, int height)
{
	my_width = width;
	my_height = height;
	my_size = width * height;
	red = new unsigned char[my_size];
	green = new unsigned char[my_size];
	blue = new unsigned char[my_size];
	
	for(int i = 0; i < my_size; i++)
	{
		red[i] = 255;
		green[i] = 255;
		blue[i] = 255;
	}
}

ppm_image::~ppm_image()
{
	delete red;
	delete green;
	delete blue;			
}
		
void ppm_image::write_to_stream(std::ostream& os, int scale)
{	
	os << "P6 " << my_width * scale << " " << my_height * scale << " 255\n";
	for(int y = 0; y < my_height * scale; y++)
		for(int x = 0; x < my_width * scale; x++)
		{
			int index = int(x / scale) + int(y / scale) * my_width;
			os << red[index] << green[index] << blue[index];
		}
}

void ppm_image::set_pixel(const point& location, const color& c)
{
	set_pixel(location, c.r,c.g,c.b);
}

void ppm_image::set_pixel(const point& location,
			const unsigned char new_red,
			const unsigned char new_green,
			const unsigned char new_blue)
		{
			int index = location[0] + location[1] * my_width;
			
			red[index] = new_red;
			green[index] = new_green;
			blue[index] = new_blue;
		}
