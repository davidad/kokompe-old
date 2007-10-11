#include </usr/include/python2.4/Python.h>
#include "gui_console.h"
#include <GL/glut.h>
#include "commands.h"

#include <iostream>
using std::vector;
using std::string;

gui_console* active_console = 0;

static PyObject*
gui_print(PyObject *self, PyObject *args)
{
    const char *print_string;
    if (!PyArg_ParseTuple(args, "s", &print_string))
	{
		std::cout << "Error: could not parse args for gui_print" << std::endl;
		return NULL;
	}

	if(!active_console) return NULL;
	
	int i = 0;

	while(print_string[i] != 0)
	{
		active_console->print(print_string[i]);
		i++;
	}
	glutPostRedisplay();

    Py_INCREF(Py_None);
    return Py_None;
}

static PyMethodDef gui_methods[] = {
    {"write", gui_print, METH_VARARGS,
     "Prints a string to the gui console."},
    {NULL, NULL, 0, NULL}
};


gui_console::gui_console()
{
	is_hidden = false;
	command_buffer = "";
	command_history.push_back("");
	command_history.push_back("");
	command_history.push_back("");
	command_history.push_back("execfile(\"../input.py\")");
	command_history.push_back("execfile(\"input.py\")");
	Py_Initialize();
	Py_InitModule("gui_console", gui_methods);
	PyRun_SimpleString("import gui_console\n" "import sys");
	PyRun_SimpleString("sys.stdout = gui_console");
	PyRun_SimpleString("sys.stderr = gui_console");
	init_py_commands();
	active_console = this;
	scroll_offset = 0;
	command_history_index = 0;
}

gui_console::~gui_console()
{
	is_hidden = false;
	command_buffer = "";
	Py_Finalize();
  
}

void gui_console::set_dimensions(int new_w, int new_h)
{
	w = new_w;
	h = new_h;
}

void gui_console::render()
{
	if(is_hidden) return;

    glDisable(GL_DEPTH_TEST);   // Depth testing must be turned on
    glDisable(GL_LIGHTING);     // Enable lighting calculations
    glDisable(GL_LIGHT0);       // Turn on light #0.

	glMatrixMode (GL_PROJECTION);
	glLoadIdentity ();
	gluOrtho2D (0.0, w / 8, h / 13, 0.0);
	//glScaled(-1,1,1);
	glMatrixMode (GL_MODELVIEW);
	glLoadIdentity ();
	glColor4f(0,0,0,1);

	int rows_to_display = (h / 13) * .5;
	int render_row = 0;
	
	int i = screen_buffer.size() - rows_to_display + scroll_offset;
	if(i < 0) i = 0;

	while(i < screen_buffer.size() && render_row < rows_to_display - 1)
	{
		glRasterPos2d(0, render_row + 1);
		render_row++;
		int column = 0;
		for(string::iterator s = screen_buffer[i].begin(); s != screen_buffer[i].end(); s++)
		{
			if(column + 2 > (w / 8))
			{
				glutBitmapCharacter (GLUT_BITMAP_8_BY_13, '\\');
				glRasterPos2d(0, render_row + 1);
				render_row++;
				column = 0;
			}
			glutBitmapCharacter (GLUT_BITMAP_8_BY_13, *s);
			column++;
		}
		i++;
	}

	glRasterPos2d(0, render_row + 1);
	render_row++;				
	glutBitmapCharacter (GLUT_BITMAP_8_BY_13, '>');
	for(string::iterator s = command_buffer.begin(); s != command_buffer.end(); s++)
	{
		glutBitmapCharacter (GLUT_BITMAP_8_BY_13, *s);
	}


	glBegin(GL_LINES);
	glVertex4d(0,render_row + 1.5,0,1);
	glVertex4d(w,render_row + 1.5,0,1);
	glEnd();
		
}

void gui_console::handle_key(char key)
{
	
	//std::cout << "keyboard key(" << int(key) << "): " << key << std::endl;
	
	if(key == 126) //tilde (~)
	{
		is_hidden = !is_hidden;
		glutPostRedisplay();
		return;		
	}

	if(is_hidden)
	{
		//special non-console commands
		//handle_command(key);
		return;
	}

	if(key == 13) //enter
	{
		if(command_buffer != "")
		{
			screen_buffer.push_back(command_buffer);
			command_history.push_back(command_buffer);
			command_history_index = 0;
			screen_buffer.push_back("");
			scroll_offset = 0;
			PyRun_SimpleString(command_buffer.c_str());
			command_buffer = "";
			glutPostRedisplay();
		}
		return;
	}
	
	if(key == 8) //backspace
	{
		if(command_buffer.length() != 0)
		{
			command_buffer.erase(--command_buffer.end());
			glutPostRedisplay();
		}
		return;
	}

	if(key == 9) //tab
	{
		command_buffer += "    ";
		glutPostRedisplay();
		return;		
	}


	
	command_buffer.push_back(key);
	glutPostRedisplay();
}

void gui_console::print(char c)
{	
	//std::cout << "P(" << int(c) << "):" << c << std::endl;

	if(c == 13 || c == 10) //enter
	{
		screen_buffer.push_back("");
		return;
	}

	screen_buffer.back().push_back(c);
	glutPostRedisplay();
}

void gui_console::scroll(int rows)
{
	scroll_offset += rows;
	glutPostRedisplay();
}

void gui_console::scroll_abs(int row)
{
	if(row >= 0)
	{
		scroll_offset = row;
	}
	else
	{
		scroll_offset = screen_buffer.size();
	}
	glutPostRedisplay();
}

void gui_console::last_command()
{
	command_history_index++;
	if(command_history_index >= command_history.size()) command_history_index = command_history.size() - 1;
	if(command_history_index < 0) command_buffer = "";
	else command_buffer = command_history[command_history.size() - 1 - command_history_index];
	glutPostRedisplay();
}

void gui_console::next_command()
{
	command_history_index--;
	if(command_history_index < 0) command_history_index = 0;
	command_buffer = command_history[command_history.size() - 1 - command_history_index];
	glutPostRedisplay();
}

bool gui_console::get_is_hidden()
{
	return is_hidden;
}
