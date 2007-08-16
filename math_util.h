#pragma once

#include "vector2.h"
#include "vector3.h"
#include "matrix4.h"
#include <ostream>

typedef double scalar;
typedef _vector2<scalar> vector2;
typedef _vector3<scalar> vector3;
typedef _matrix4<scalar> matrix4;
typedef _quaternion<scalar> quaternion;


template<typename T >
std::ostream& operator<<(std::ostream& os, const _vector3<T >& v)
{
	os << '[' << v[0] << ", " << v[1] << ", " << v[2] << ']';
	return os;
}
