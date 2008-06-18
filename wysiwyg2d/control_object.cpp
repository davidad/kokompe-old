#include <GL/glut.h>
#include "../math/math_gl.h"

#include "control_object.h"


bool point_control::check_hit(_vector2<double> check_position, double radius)
{
	return (check_position - my_position).length_squared() < (radius * radius);
}

_vector2<double> point_control::get_state()
{
	return my_position;
}


//basic rendering function
void point_control::render_static()
{
	glDisable(GL_TEXTURE_2D);
	glEnable(GL_POINT_SMOOTH);
	glPointSize(5.0);
	glColor3d(0.0,0.8,0.0);
	glBegin(GL_POINTS);
	glVertex(my_position);
	glEnd();	
}

//rendering for when mouse is over the object
void point_control::render_hover()
{
	glDisable(GL_TEXTURE_2D);
	glEnable(GL_POINT_SMOOTH);
	glPointSize(8.0);
	glColor3d(0.0,1.0,0.0);
	glBegin(GL_POINTS);
	glVertex(my_position);
	glEnd();	
}

void point_control::render_drag(_vector2<double> mouse_position)
{
	glDisable(GL_TEXTURE_2D);
	glEnable(GL_POINT_SMOOTH);
	glPointSize(5.0);
	glColor3d(0.1,0.9,0.1);
	glBegin(GL_POINTS);
	glVertex(mouse_position);
	glEnd();	
}

void point_control::set_state(_vector2<double> new_param)
{
	my_position = new_param;
}

	
