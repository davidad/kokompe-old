#include <GL/glut.h>
#include "math_gl.h"

void glNormal(const _vector3< double >& n)
{
	glNormal3d(n[0], n[1], n[2]);
}

void glVertex(const _vector3< double >& v)
{
	glVertex3d(v[0], v[1], v[2]);
}

void glTranslate(const _vector3< double >& t)
{
	glTranslated(t[0], t[1], t[2]);
}

void glVertex(const _vector2< double >& v)
{
	glVertex2d(v[0], v[1]);
}

void glTranslate(const _vector2< double >& t)
{
	glTranslated(t[0], t[1], 0);
}

void glColor(const _vector3< double >& c)
{
	glColor3d(c[0], c[1], c[2]);
}

void glColor(const _vector4< double >& c)
{
	glColor4d(c[0], c[1], c[2], c[3]);
}

void glRotate(const _vector3< double >& a)
{
	glRotated(a[0], 1.0, 0.0, 0.0); 
	glRotated(a[1], 0.0, 1.0, 0.0);
	glRotated(a[2], 0.0, 0.0, 1.0);
}

void glUnrotate(const _vector3< double >& a)
{
	glRotated(-a[2], 0.0, 0.0, 1.0);
	glRotated(-a[1], 0.0, 1.0, 0.0);
	glRotated(-a[0], 1.0, 0.0, 0.0); 
}

void glMultMatrix(const _matrix4< double >& m)
{
	glMultMatrixd(m.val_array());
}

void glLoadMatrix(const _matrix4< double >& m)
{
	glLoadMatrixd(m.val_array());
}

_matrix4< double > glGetModelviewMatrix()
{
	_matrix4< double > results;
	glGetDoublev(GL_MODELVIEW_MATRIX, results.val_array());
	return results;
}

_matrix4< double > glGetProjectionMatrix()
{
	_matrix4< double > results;
	glGetDoublev(GL_PROJECTION_MATRIX, results.val_array());
	return results;
}




