#pragma once

#ifndef M_PI
#define M_PI 3.1415926535897932384626433832795028 
#endif

#include "vector2.h"
#include "vector3.h"
#include "vector4.h"
#include "quaternion.h"
#include "matrix3.h"
#include "matrix4.h"
#include "math_iostream.h"
#include "math_gl.h"

typedef double scalar;
typedef _vector2< scalar > vector2;
typedef _vector3< scalar > vector3;
typedef _vector4< scalar > vector4;
typedef _quaternion< scalar > quaternion;
typedef _matrix3< scalar > matrix3;
typedef _matrix4< scalar > matrix4;

