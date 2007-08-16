#pragma once

#include <ostream>

#include "vector2.h"
#include "vector3.h"

template <typename scalar>
std::ostream& operator<<
(std::ostream& os, const _vector3<scalar >& vec)
{
    return os<< "<" << vec.x() << ", " << vec.y() << ", " << vec.z() << ">";
}

template <typename scalar>
std::ostream& operator<<
(std::ostream& os, const _vector2<scalar >& vec)
{
    return os<< "<" << vec.x() << ", " << vec.y() << ">";
}

//TODO: get code for matrix output
//TODO: get code for quaternion output
//TODO: get code for array_2d output
