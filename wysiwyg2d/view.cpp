#include <GL/glut.h>
#include <map>
#include <fstream>
#include <iostream>

#include "../math/math_iostream.h"
#include "../math/math_gl.h"
#include "view.h"		
#include "control_object.h"

std::fstream debug_log("debug_log.txt", std::ios_base::out);

std::map<int, view*> open_views;

view* get_active_view()
{
	int current_window = glutGetWindow();

	if(open_views.count(current_window) != 0)
	{
		return open_views[current_window];
	}
	else
	{
		return 0;
	}

}

void global_render()
{
	view* current_view = get_active_view();
	//debug_log << "rendering " << current_view << std::endl;
	if(current_view)
	{	
		current_view->render();
	}
	//debug_log << current_view << " done" << std::endl;
}

void global_keyboard_key(unsigned char key, int x, int y)
{
	view* current_view = get_active_view();
	//debug_log << "keyboard event for " << current_view << std::endl;
	if(current_view)
	{	
		current_view->keyboard_key(key, x, y);
	}
	//debug_log << current_view << " done" << std::endl;
}

void global_mouse(int button, int state, int x, int y)
{
	view* current_view = get_active_view();
	//debug_log << "mouse event for " << current_view << std::endl;
	if(current_view)
	{	
		current_view->mouse(button, state, x, y);
	}
	//debug_log << current_view << " done" << std::endl;
}

void global_mouse_passive_move(int x, int y)
{
	view* current_view = get_active_view();
	debug_log << "mouse passive event for " << current_view << std::endl;
	if(current_view)
	{	
		current_view->mouse_passive_move(x, y);
	}
	debug_log << current_view << " done" << std::endl;
}

void global_mouse_drag(int x, int y)
{
	view* current_view = get_active_view();
	//debug_log << "mouse drag event for " << current_view << std::endl;
	if(current_view)
	{	
		current_view->mouse_drag(x, y);
	}
	//debug_log << current_view << " done" << std::endl;
}


void global_reshape(int width, int height)
{
	view* current_view = get_active_view();
	//debug_log << "reshape event for " << current_view << std::endl;
	if(current_view)
	{	
		current_view->reshape(width, height);
	}
	//debug_log << current_view << " done" << std::endl;
}

void global_menu_select(int selection_index)
{
	view* current_view = get_active_view();
	debug_log << "menu event for " << current_view << std::endl;
	if(current_view)
	{	
		current_view->menu_select(selection_index);
	}
	debug_log << current_view << " done" << std::endl;
}

void create_basic_menu()
{
	int new_menu = glutCreateMenu(global_menu_select);
	glutSetMenu(new_menu);
	glutAttachMenu(GLUT_RIGHT_BUTTON);
	glutAddMenuEntry("New Control Point", 1);
	glutAddMenuEntry("Dummy menu entry", 2);
}



view::view(design& d, application& parent)
 : viewed_design(&d),
	my_parent(&parent)

{
	window_id = glutCreateWindow("New View");
    glutDisplayFunc(global_render);
    glutKeyboardFunc(global_keyboard_key);
    glutPassiveMotionFunc(global_mouse_passive_move);
    glutMouseFunc(global_mouse);
    glutMotionFunc(global_mouse_drag);
    glutPassiveMotionFunc(global_mouse_passive_move);
    glutReshapeFunc(global_reshape);
	create_basic_menu();
	open_views[window_id] = this;

	pixels_per_object_unit = 320;
	screen_center = _vector2<double>(0,0);

	dragged_control = NULL;


}


view::~view()
{	
	close();
}

void view::render()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glLoadIdentity();
	
	gluOrtho2D(-1.0,1.0,1.0,-1.0);
	
	viewed_design->render(screen_upper_left(), screen_lower_right()); //render evalutated string, relationships, and control points

	control_object* hover_object = viewed_design->check_hit(mouse_position, .04);

	if(hover_object)
	{
		hover_object->render_hover();
	}

	if(dragged_control)
	{
		glColor3d(0.0,0.0,1.0);
		glBegin(GL_LINES);
		glVertex(dragged_control->get_state());	
		glVertex(mouse_position);
		glEnd();
		dragged_control->render_drag(mouse_position);
	}

	//render dragged objects
	//render toolbars etc.

 	glutSwapBuffers();
}

void view::keyboard_key(unsigned int key, int x, int y)
{
	my_parent->initiate_shutdown();
}

void view::reshape(int width, int height)
{
	glViewport(0,0,width,height);
}

void view::mouse(int button, int state, int x, int y)
{
	mouse_position = mouse_to_object_space(x, y);
	if(button == GLUT_LEFT_BUTTON)
	{	
		if(state == GLUT_DOWN)
		{
			dragged_control = viewed_design->check_hit(mouse_position, .04);
			drag_start = mouse_position;
		}
		else
		{
			if(dragged_control)
			{
				viewed_design->move(dragged_control, mouse_position);
				dragged_control = NULL;
			}
		}
	}
	glutPostRedisplay();
}

void view::mouse_passive_move(int x, int y)
{
	mouse_position = mouse_to_object_space(x, y);
	glutPostRedisplay();
}

void view::mouse_drag(int x, int y)
{
	mouse_position = mouse_to_object_space(x, y);
	if(dragged_control)
	{
		viewed_design->move(dragged_control, mouse_position);
	}
	glutPostRedisplay();
}

void view::menu_select(int selection_index)
{
	if(selection_index == 1) {
		viewed_design->add_control_point(mouse_position);
	}
}

void view::close()
{
	if(window_id != -1)
	{
		open_views.erase(window_id);
	 	glutDestroyWindow(window_id);
		window_id = -1;
	}
}
_vector2<double> view::screen_upper_left()
{
	int dims[4];
	glGetIntegerv(GL_VIEWPORT, dims);
	int width = dims[2];
	int height = dims[3];

	double object_width = (double) width / pixels_per_object_unit;
	double object_height = (double) width / pixels_per_object_unit;
	
	_vector2<double> sul = screen_center;
	sul[0] -= object_width * 0.5;
	sul[1] -= object_height * 0.5;
	
	return sul;
}

_vector2<double> view::screen_lower_right()
{
	int dims[4];
	glGetIntegerv(GL_VIEWPORT, dims);
	int width = dims[2];
	int height = dims[3];

	double object_width = (double) width / pixels_per_object_unit;
	double object_height = (double) width / pixels_per_object_unit;
	
	_vector2<double> slr = screen_center;
	slr[0] += object_width * 0.5;
	slr[1] += object_height * 0.5;
	
	return slr;
}

_vector2<double> view::mouse_to_object_space(int x, int y)
{
	int dims[4];
	glGetIntegerv(GL_VIEWPORT, dims);
	int width = dims[2];
	int height = dims[3];
	double norm_x = (double) x / (double) width;
	double norm_y = (double) y / (double) height;
	
	double obj_x = screen_upper_left()[0] + norm_x * (screen_lower_right()[0] - screen_upper_left()[0]);
	double obj_y = screen_upper_left()[1] + norm_y * (screen_lower_right()[1] - screen_upper_left()[1]);

	return _vector2<double>(obj_x, obj_y);
}



