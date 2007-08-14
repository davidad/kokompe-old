#pragma once
#include "vector2.h"
typedef _vector2<int> point;

class color
{
	public:
		color(int red, int green, int blue)
		{
			r = red;
			g = green;
			b = blue;
		}
		unsigned char r;
		unsigned char g;
		unsigned char b;
};

class ppm_image
{
	private:
		int my_width;
		int my_height;
		int my_size;
		unsigned char* red;
		unsigned char* green;
		unsigned char* blue;
	
	public:
		ppm_image(int width, int height);
		~ppm_image();
		void write_to_disk(const char* filename, int scale = 1);
		void set_pixel(const point& location, const color& c);
		void set_pixel(const point& location, 
			const unsigned char new_red,
			const unsigned char new_green,
			const unsigned char new_blue);
};
