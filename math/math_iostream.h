#pragma once

#include <iostream>

#include "vector2.h"
#include "vector3.h"
#include "vector4.h"

#include "matrix3.h"
#include "matrix4.h"

template <typename scalar>
std::ostream& operator<<
(std::ostream& os, const _vector3<scalar >& vec)
{
    return os<< vec[0] << " " << vec[1] << " " << vec[2];
}

template <typename scalar>
std::istream& operator>>
(std::istream& is, _vector3<scalar >& vec)
{
    return is >> vec[0] >> vec[1] >> vec[2];
}

template <typename scalar>
std::ostream& operator<<
(std::ostream& os, const _vector2<scalar >& vec)
{
    return os<< vec[0] << " " << vec[1];
}

template <typename scalar>
std::istream& operator>>
(std::istream& is, _vector2<scalar >& vec)
{
    return is >> vec[0] >> vec[1];
}

template <typename scalar>
std::ostream& operator<<
(std::ostream& os, const _vector4<scalar >& vec)
{
    return os<< vec[0] << " " << vec[1] << " " << vec[2] << " " << vec[3];
}

template <typename scalar>
std::istream& operator>>
(std::istream& is, _vector4<scalar >& vec)
{
    return is >> vec[0] >> vec[1] >> vec[2] >> vec[3];
}

template <typename scalar>
std::ostream& operator<<
(std::ostream& os, const _matrix4<scalar >& m)
{
 	for(int i = 0; i < 4; i++)
	{
		os << '\t' << m.row(i) << '\n';
	}
	return os;
}

template <typename scalar>
std::ostream& operator<<
(std::ostream& os, const _matrix3<scalar >& m)
{
 	for(int i = 0; i < 3; i++)
	{
		os << '\t' << m.row(i) << '\n';
	}
	return os;
}
//TODO: write code for quaternion output
//TODO: write code for array_2d output
