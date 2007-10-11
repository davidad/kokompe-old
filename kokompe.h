#pragma once

#include "math.h"
#include "gui_console.h"
#include "eval_geometry.h"
#include "color.h"
#include "camera.h"

#include <string>

class kokompe
{
	private:
		trimesh_t trimesh;
		Camera camera;
		color background_color;
	public:
		//functions that will be pythonized
		//void set_display_color(color new_color);
		void render_math_string(std::string instring);

		//core functions
		void init(int argc, char** argv);
		void run();

		void render();
		void handle_key(char key);
		void special(int key);
		void reshape(int new_width, int new_height);
		void mouse(int button, int state, int x, int y);
		void mouse_motion(int x, int y);

		gui_console console;

};

extern kokompe* active_kokompe;
#define gui active_kokompe
