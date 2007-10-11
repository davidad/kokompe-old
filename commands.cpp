#include </usr/include/python2.4/Python.h>

#include <GL/glut.h>
#include <string>
#include <map>
#include <fstream>

#include "commands.h"
#include "console_commands.h"
#include "kokompe.h"

static PyObject* load_math_string(PyObject *self, PyObject *args);

static PyMethodDef kokompe_commands[] = {
    {"load_math_string", load_math_string, METH_VARARGS,"Accepts a math string and renders it."},
    {NULL, NULL, 0, NULL}
};

void init_py_commands()
{
	Py_InitModule("kokompe", kokompe_commands);
	PyRun_SimpleString("import kokompe\n");
        init_console_module();
}

static PyObject* load_math_string(PyObject *self, PyObject *args)
{
	const char *instring;
    if (!PyArg_ParseTuple(args, "s", &instring))
	{
		std::cout << "Error: could not parse args for load_math_string" << std::endl;
		return NULL;
	}

	active_kokompe->render_math_string(std::string(instring));
 	glutPostRedisplay();

    Py_INCREF(Py_None);
    return Py_None;
}

std::map<char, std::string> char_commands;
std::map<int, std::string> special_commands;


void run_string(const std::string& command, bool is_silent)
{
       if(!is_silent)
       {
               gui->console << command << "\n";
       }
       PyRun_SimpleString(command.c_str());
}

void bind_char(char key, const std::string& command)
{
       if(key == '~' || key == '`')
       {
               gui->console << "cannot bind " << key << " because it is reserved\n";
               return;
       }

       if(command != "")
       {
               char_commands[key] = command;
       }
       else if(char_commands.count(key))
       {
                       char_commands.erase(char_commands.find(key));
       }

}

void bind_special(int key, const std::string& command)
{
       if(command != "")
       {
               special_commands[key] = command;
       }
       else if(special_commands.count(key))
       {
               special_commands.erase(special_commands.find(key));
       }
}

void process_key(char key)
{
       if(key == '~')
       {
               gui->console.set_is_hidden(!gui->console.get_is_hidden());
               return;
       }

       if(key == '`')
       {
               gui->console.set_is_active(!gui->console.get_is_active());
               return;
       }

       if(gui->console.get_is_active())
       {
               gui->console.handle_key(key);
       }
       else
       {
               if(char_commands.count(key)) run_string(char_commands[key]);
               else gui->console << "Unbound key: " << key << " (" << int(key) << ")\n";
       }

}

void process_special(int key)
{
       if(special_commands.count(key))
       {
          run_string(special_commands[key], true);
       }
       else
       {
               gui->console << "Unbound special key: " << int(key) << "\n";
       }
}
