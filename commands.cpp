#include </usr/include/python2.4/Python.h>

#include <GL/glut.h>
#include <string>

#include "commands.h"
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
