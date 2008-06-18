#include <GL/glut.h>
#include <iostream>
#include <sstream>

#include "design.h"


design::design()
{
	//my_controls.insert(new point_control(_vector2<double>(0.0,0.2)));
	//my_controls.insert(new point_control(_vector2<double>(0.2,0.0)));
}

design::~design()
{
	//delete all of the control objects
	for(std::set<control_object*>::iterator iter = my_controls.begin(); iter != my_controls.end(); iter++)
	{
		delete (*iter);
	}
}

std::string design::get_math_string()
{
	//eventually this will trace the full graph from...
	//    control_objects -> parameters
	//         parameters -> relationships -> parameters
	//         parameters -> shapes -> clauses
	//             clauses -> operators -> clauses
	//                 -> math_string
	std::stringstream mstr;
	
	if(my_controls.size() == 0)
	{		
		mstr << "false";
	}
	else if(my_controls.size() == 1)
	{
		_vector2<double> center = (*my_controls.begin())->get_state();
		mstr << "((x - " << center[0] << ") ** 2 + (y - " << center[1] << ") ** 2 < .2)";
	} else
	{
		std::set<control_object*>::iterator iter = my_controls.begin();
		_vector2<double> center = (*iter)->get_state();
		mstr << "~((x - " << center[0] << ") ** 2 + (y - " << center[1] << ") ** 2 < .2) & (";
		iter++;
		mstr << "((x - " << center[0] << ") ** 2 + (y - " << center[1] << ") ** 2 < .2)";
		
		for(; iter != my_controls.end(); iter++)
		{
			_vector2<double> center = (*iter)->get_state();
			mstr << " | ((x - " << center[0] << ") ** 2 + (y - " << center[1] << ") ** 2 < .2)";
		}
		mstr << ")";
	}

	
	return mstr.str();
}

void design::render(_vector2<double> upper_left, _vector2<double> lower_right)
{
	const double left = upper_left[0];
	const double right = lower_right[0];
	const double top = upper_left[1];
	const double bottom = lower_right[1];

	const double width = right - left;
	const double height = top - bottom;

	evaluated_string.set_min_x(left);	
	evaluated_string.set_min_y(top);	

	evaluated_string.set_max_x(right);	
	evaluated_string.set_max_y(bottom);	

	evaluated_string.set_math_string(get_math_string());

	glLoadIdentity();
	gluOrtho2D(left,right,bottom,top);

	//render the evalutated string
	glEnable(GL_TEXTURE_2D);
	glColor3d(1.0,1.0,1.0);
	evaluated_string.bind();

	glTranslatef(0.0f,0.0f,-0.5f);

	glBegin(GL_QUADS);
	glTexCoord2f(0.0f, 0.0f); glVertex2f(left, top);
	glTexCoord2f(1.0f, 0.0f); glVertex2f(right, top);
	glTexCoord2f(1.0f, 1.0f); glVertex2f(right, bottom);
	glTexCoord2f(0.0f, 1.0f); glVertex2f(left,  bottom);
	glEnd();

	//render all of the relationships
	//TODO

	//render all of the control objects
	for(std::set<control_object*>::iterator iter = my_controls.begin(); iter != my_controls.end(); iter++)
	{
		(*iter)->render_static();
	}
		
}

control_object* design::check_hit(_vector2<double> p, double r)
{
	for(std::set<control_object*>::iterator iter = my_controls.begin(); iter != my_controls.end(); iter++)
	{
		if((*iter)->check_hit(p, r)) {
			return *iter;
		}
	}

	return NULL;
}

void design::add_control_point(_vector2<double> p)
{
	my_controls.insert(new point_control(p));
}

void design::move(control_object* obj, _vector2<double> destination)
{
	obj->set_state(destination);	
}

