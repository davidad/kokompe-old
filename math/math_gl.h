#pragma once
#include "math.h"

void glVertex(const _vector2< double >& v);
void glTranslate(const _vector2< double >& t);

void glNormal(const _vector3< double >& n);
void glVertex(const _vector3< double >& v);
void glTranslate(const _vector3< double >& t);

void glColor(const _vector3< double >& c);
void glColor(const _vector4< double >& c);

void glRotate(const _vector3< double >& a);
void glUnrotate(const _vector3< double >& a);

void glMultMatrix(const _matrix4< double >& m);
void glLoadMatrix(const _matrix4< double >& m);

