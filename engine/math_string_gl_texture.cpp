#include <string>
#include <GL/glut.h>
#include <iostream>

#include "math_string_gl_texture.h"
#include "octree.h"
#include "expression.h"
#include "space_interval.h"
#include "interval.h"

#include "ppm_image.h"
#include "array_2d.h"


void math_string_gl_texture::set_max_x(double new_max_x)
{
	if(max_x != new_max_x) {
		max_x = new_max_x;
		dirty = true;
	}	
}

void math_string_gl_texture::set_max_y(double new_max_y)
{
	if(max_y != new_max_y) {
		max_y = new_max_y;
		dirty = true;
	}
}

void math_string_gl_texture::set_min_x(double new_min_x)
{
	if(min_x != new_min_x) {
		min_x = new_min_x;
		dirty = true;
	}
}

void math_string_gl_texture::set_min_y(double new_min_y)
{
	if(min_y != new_min_y) {
		min_y = new_min_y;
		dirty = true;	
	}
}

math_string_gl_texture::math_string_gl_texture()
{
	texture_id = 0;
	math_string = "x**2 + y**2 < .75";
	nx = 512;
	ny = 512;
	min_x = -1;
	min_y = -1;
	max_x = 1;
	max_y = 1;
	dirty = true;
}

void math_string_gl_texture::set_math_string(const std::string new_math_string)
{
	if(math_string != new_math_string)
	{
		math_string = new_math_string;
		dirty = true;
	}
}

void math_string_gl_texture::bind()
{
	//return the current texture if nothing needs to be changed
	if(dirty == false)
	{
		glBindTexture(GL_TEXTURE_2D, texture_id);
		return;
	}

	//make new texture if needed
	if(texture_id == 0)
	{
		glGenTextures(1, &texture_id);
	}

	interval_t x,y,z;
	x.set_real_interval(min_x, max_x);
	y.set_real_interval(min_y, max_y);
	z.set_real_interval(-0.0001, 0.0001);
	space_interval_t si(x,y,z);
	

	std::string optimized_expression = "(Z == 0) & (" + math_string + ")";
	expression_t ex(convert_infix_to_postfix(optimized_expression));
	
    octree_t octree(ex, si);
	
	int recursion_depth = 5;
  	octree.eval(recursion_depth);

	char* results = new char[nx * ny];
	si.set(min_x, max_x, min_y, max_y, 0, 0);

	std::cout << "Evaluating \"" << math_string << "\"" << std::endl;

	octree.eval_on_grid(si, nx, ny, 1, results, NULL);

	std::cout << "\t...Done. " << std::endl;

	for(int i = 0; i < (nx * ny); i++)
	{		
		if(results[i] != 0)
		{
			results[i] = 255;
		}
	}	
	
	glBindTexture(GL_TEXTURE_2D, texture_id);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, nx, ny, 0, GL_LUMINANCE, GL_UNSIGNED_BYTE, results);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
	
	dirty = false;	
}


math_string_gl_texture::~math_string_gl_texture()
{
	glDeleteTextures(1, &texture_id);
}


