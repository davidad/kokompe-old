#pragma once
class view;

#include "application.h"
#include "design.h"
#include "../engine/math_string_gl_texture.h"
#include "../math/vector2.h"

class view
{
    public:
		view(design& d, application& parent);
		~view();
		void close();
		void render();
		void keyboard_key(unsigned int key, int x, int y);
		void reshape(int width, int height);
		void mouse(int button, int state, int x, int y);
		void mouse_passive_move(int x, int y);
		void mouse_drag(int x, int y);
		void menu_select(int selection);
    private:
		int window_id;
		design* viewed_design;
		application* my_parent;

		double pixels_per_object_unit;
		_vector2<double> screen_center;

		_vector2<double> screen_upper_left();
		_vector2<double> screen_lower_right();
		
		_vector2<double> mouse_position;

		_vector2<double> mouse_to_object_space(int x, int y);
		
		_vector2<double> drag_start;
		control_object* dragged_control;
};






