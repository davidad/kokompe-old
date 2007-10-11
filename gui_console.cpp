#include </usr/include/python2.4/Python.h>
#include "gui_console.h"
#include "kokompe.h"
#include <GL/glut.h>
#include "commands.h"
#include <iostream>
#include <sstream>
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
    {"write", gui_print, METH_VARARGS, "Prints a string to the gui console."},
    {NULL, NULL, 0, NULL}
};


gui_console::gui_console()
: prompt(">>>")
{
	is_hidden = false;
	is_active = true;

	command_buffer = "";
	cursor_position = 0;
	Py_Initialize();
	Py_InitModule("gui_console", gui_methods);
	PyRun_SimpleString("import gui_console\n" "import sys");
	PyRun_SimpleString("sys.stdout = gui_console");
	PyRun_SimpleString("sys.stderr = gui_console");
	active_console = this;
	scroll_offset = 0;
	command_history_index = 0;
}

gui_console::~gui_console()
{
	Py_Finalize();
}

void gui_console::set_dimensions(int new_w, int new_h)
{
	w = new_w;
	h = new_h;
}

void gui_console::render()
{
	if(get_is_hidden()) return;
	
	glPushAttrib(GL_ALL_ATTRIB_BITS);

    glDisable(GL_DEPTH_TEST);   // Depth testing must be turned off
    glDisable(GL_LIGHTING);     // We also want to use solid colors
    
	glMatrixMode (GL_PROJECTION);
	glPushMatrix();

	glLoadIdentity ();
	gluOrtho2D (0.0, w / 8, h / 13, 0.0);
	
	glMatrixMode (GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity ();
	glColor4f(0,0,0,1);

	int rows_to_display = (h / 13) * .5;
	current_raster_line = 0;
		
	int i = screen_buffer.size() - rows_to_display + scroll_offset;
	if(i < 0) i = 0;

	while(i < screen_buffer.size() && current_raster_line < rows_to_display - 1)
	{
		render_text_line(screen_buffer[i]);
		i++;
	}

	if(get_is_active())
	{
		//blinking cursor (might not be animated/displayed if screen is not constantly updated)
		if( (glutGet(GLUT_ELAPSED_TIME) / 500) % 2)
		{
			//here we want to get the position of the cursor
			//this is kind of ugly, but at least we only do it once
			int row_offset = (cursor_position + prompt.length()) / ((w / 8) - 1);
			int column = (cursor_position + prompt.length()) % ((w / 8) - 1);
			
			glBegin(GL_LINES);
			glVertex3d(column + 0.1, current_raster_line + row_offset + .1,0);
			glVertex3d(column + 0.1, current_raster_line + row_offset + 1.3,0);
			glEnd();
		}
	
		render_text_line(prompt + command_buffer);
		glutPostRedisplay();
	}
	
	glBegin(GL_LINES);
	glVertex3d(0,current_raster_line + .25,0);
	glVertex3d(w,current_raster_line + .25,0);
	glEnd();
	
	glPopMatrix();
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
	glPopAttrib();
}

void gui_console::render_text_line(const std::string& line)
{
	glRasterPos2d(0, current_raster_line + 1);
	current_raster_line++;
	int column = 0;
	for(string::const_iterator s = line.begin(); s != line.end(); s++)
	{
		if(column + 2 > (w / 8))
		{
			glutBitmapCharacter (GLUT_BITMAP_8_BY_13, '\\');
			glRasterPos2d(0, current_raster_line + 1);
			current_raster_line++;
			column = 0;
		}
		glutBitmapCharacter (GLUT_BITMAP_8_BY_13, *s);
		column++;
	}
}

void gui_console::handle_key(char key)
{
	
	//std::cout << "keyboard key(" << int(key) << "): " << key << std::endl;
	
	if(key == 13) //enter
	{
		if(command_buffer != "")
		{
			screen_buffer.push_back(command_buffer);
			screen_buffer.push_back("");

			command_history.push_back(command_buffer);
			command_history_index = -1;
			
			scroll_offset = 0;
			PyRun_SimpleString(command_buffer.c_str());
			command_buffer = "";
			cursor_position = 0;
			glutPostRedisplay();
		}
		else
		{
			screen_buffer.push_back("");
		}
		return;
	}
	else if(key == 8) //backspace
	{
		std::string::iterator curser = command_buffer.begin();
		cursor_position--;
		if(cursor_position >= 0)
		{
			curser += cursor_position;
			command_buffer.erase(curser);
		}
		else
		{
			cursor_position = 0;
		}
		glutPostRedisplay();
		return;
	}
	else if(key == 9) //tab
	{
		command_buffer += "    ";
		glutPostRedisplay();
		return;		
	}
	else if(key == 127) //delete
	{
		std::string::iterator curser = command_buffer.begin();
		curser += cursor_position;
		command_buffer.erase(curser);
		glutPostRedisplay();
		return;
	}
	else
	{
		std::string::iterator curser = command_buffer.begin();
		curser += cursor_position;
		command_buffer.insert(curser, key);
		cursor_position++;
		glutPostRedisplay();
	}
}

void gui_console::print(unsigned char c)
{	
	//std::cout << "P(" << int(c) << "):" << c << std::endl;

	if(c == 13 || c == 10) //enter
	{
		screen_buffer.push_back("");
		return;
	}
	if(c == 9) //tab
	{
		screen_buffer.push_back("    ");
		return;
	}
	screen_buffer.back().push_back(c);
	glutPostRedisplay();
}

void gui_console::print(const std::string& str)
{	
	for(std::string::const_iterator i = str.begin(); i != str.end(); i++)
	{
		unsigned char c = (*i);
		if(c == 13 || c == 10) //enter
		{
			screen_buffer.push_back("");
			return;
		}
		if(c == 9) //tab
		{
			screen_buffer.push_back("    ");
			return;
		}
		
		if(!screen_buffer.size())
		{
			screen_buffer.push_back("");
		}
	
		screen_buffer.back().push_back(c);
	
	}

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

void gui_console::cursor_left()
{
	cursor_position--;
	if(cursor_position < 0) cursor_position = 0;
	glutPostRedisplay();
}

void gui_console::cursor_right()
{
	cursor_position++;
	if(cursor_position >= command_buffer.length()) cursor_position = command_buffer.length() - 1;
	glutPostRedisplay();
}
void gui_console::last_command()
{
	command_history_index++;
	if(command_history_index >= command_history.size())
	{
		command_history_index = command_history.size() - 1;
	}

	if(command_history_index < 0)
	{
		command_buffer = "";
	}
	else
	{
		command_buffer = command_history[command_history.size() - 1 - command_history_index];
		cursor_position = command_buffer.length();
	}
	glutPostRedisplay();
}

void gui_console::next_command()
{
	command_history_index--;
	if(command_history_index < 0) command_history_index = 0;
	command_buffer = command_history[command_history.size() - 1 - command_history_index];
	cursor_position = command_buffer.length();
	glutPostRedisplay();
}

void gui_console::cursor_home()
{
	cursor_position = 0;
	glutPostRedisplay();
}

void gui_console::cursor_end()
{
	cursor_position = command_buffer.length();
	glutPostRedisplay();
}
		

bool gui_console::get_is_hidden()
{
	return is_hidden;
}

void gui_console::set_is_hidden(bool new_val)
{
	is_hidden = new_val;
	if(is_hidden == true) is_active = false; //console is never active while hidden
	glutPostRedisplay();
}

bool gui_console::get_is_active()
{
	return is_active;
}

void gui_console::set_is_active(bool new_val)
{
	is_active = new_val;
	if(is_active == true) is_hidden = false;
	glutPostRedisplay();
}
