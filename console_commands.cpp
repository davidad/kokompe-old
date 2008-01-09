#ifdef WIN32
	#ifdef DEBUG
		#undef DEBUG
		#undef _DEBUG
		#include <Python.h>
		#define DEBUG
		#define _DEBUG
	#else
		#include <Python.h>
	#endif
#else
	#include </usr/include/python2.4/Python.h>
#endif


#ifdef WIN32
#include <glut.h>
#else
#include <GL/glut.h>
#endif

#include "console_commands.h"
#include "commands.h"
#include "kokompe.h"

static PyObject* bind_char_py(PyObject *self, PyObject *args);
static PyObject* bind_special_py(PyObject *self, PyObject *args);

static PyObject* last_command(PyObject *self, PyObject *args);
static PyObject* next_command(PyObject *self, PyObject *args);
static PyObject* cursor_left(PyObject *self, PyObject *args);
static PyObject* cursor_right(PyObject *self, PyObject *args);
static PyObject* cursor_home(PyObject *self, PyObject *args);
static PyObject* cursor_end(PyObject *self, PyObject *args);


static PyMethodDef console_methods[] = {
   {"next_command", next_command, METH_VARARGS, "Goes to next command in command history."},
   {"last_command", last_command, METH_VARARGS, "Goes to previous command in command history."},
   {"cursor_left", cursor_left, METH_VARARGS, "Moves console cursor left one step."},
   {"cursor_right", cursor_right, METH_VARARGS, "Moves console cursor right one step."},
   {"cursor_home", cursor_home, METH_VARARGS, "Moves console cursor to start of line."},
   {"cursor_end", cursor_end, METH_VARARGS, "Moves console cursor to end of line."},
   {"bind_char", bind_char_py, METH_VARARGS, "Binds a command to a single 'normal' key (mostly printable characters) i.e. gui.bind(" ", \"cells.step()\"). Note that these commands will only be processed if the console is inactive (toggle with ` or ~)"},
   {"bind_special", bind_special_py, METH_VARARGS, "Binds a command to a single 'special' key. i.e. gui.bind_special(101,\"gui.last_command()\")."},
   {NULL, NULL, 0, NULL}
};

static PyObject* bind_char_py(PyObject *self, PyObject *args)
{
  const char *command;
  char key ;
  if (!PyArg_ParseTuple(args, "cs", &key, &command)
         && !PyArg_ParseTuple(args, "bs", &key, &command))
       {
               gui->console << "Error: could not parse args for bind_char\n";
               return NULL;
       }

       bind_char(key, command);

       Py_INCREF(Py_None);
   return Py_None;
}

static PyObject* bind_special_py(PyObject *self, PyObject *args)
{
       const char *command;
       int key;
   	if (!PyArg_ParseTuple(args, "is", &key, &command))
       {
               gui->console << "Error: could not parse args for bind_char\n";
               return NULL;
       }
       bind_special(key, command);

       Py_INCREF(Py_None);
   return Py_None;
}


static PyObject* last_command(PyObject *self, PyObject *args)
{
       gui->console.last_command();
       Py_INCREF(Py_None);
   return Py_None;
}

static PyObject* next_command(PyObject *self, PyObject *args)
{
       gui->console.next_command();
       Py_INCREF(Py_None);
   return Py_None;
}

static PyObject* cursor_left(PyObject *self, PyObject *args)
{
       gui->console.cursor_left();
       Py_INCREF(Py_None);
   return Py_None;
}

static PyObject* cursor_home(PyObject *self, PyObject *args)
{
       gui->console.cursor_home();
       Py_INCREF(Py_None);
   return Py_None;
}

static PyObject* cursor_end(PyObject *self, PyObject *args)
{
       gui->console.cursor_end();
       Py_INCREF(Py_None);
   return Py_None;
}

static PyObject* cursor_right(PyObject *self, PyObject *args)
{
       gui->console.cursor_right();
       Py_INCREF(Py_None);
   return Py_None;
}


void init_console_module()
{
        Py_InitModule("console", console_methods);
        PyRun_SimpleString("import console\n");
	bool silent_bindings = true;
       	run_string("console.bind_special(101, \"console.last_command()\") #101 = up arrow key", silent_bindings);
	run_string("console.bind_special(103, \"console.next_command()\") #103 = down arrow", silent_bindings);
	run_string("console.bind_special(100, \"console.cursor_left()\")  #100 = left arrow key", silent_bindings);
	run_string("console.bind_special(102, \"console.cursor_right()\") #102 = right arrow key", silent_bindings);
	run_string("console.bind_special(106, \"console.cursor_home()\") #106 = home", silent_bindings);
	run_string("console.bind_special(107, \"console.cursor_end()\") #107 = end", silent_bindings);
}
