#pragma once
#include <cmath>

template<typename scalar>
class _vector2
{
	private:
		scalar vals[2];
		
	public:
		const scalar& x() const {return vals[0];}
		const scalar& y() const {return vals[1];}
		
		_vector2() {}
			
		_vector2(const scalar* array)
		{
			vals[0] = array[0];
			vals[1] = array[1];
		}
		
		_vector2(const _vector2& base)
		{
			vals[0] = base.vals[0];
			vals[1] = base.vals[1];
		}
		_vector2(scalar new_x, scalar new_y)
		{
			vals[0] = new_x;
			vals[1] = new_y;
		}	
	
		scalar* as_array(){return vals;}
		const scalar* as_array() const{return vals;}
		
		scalar& operator[](int index) {return vals[index];}
		const scalar& operator[](int index) const {return vals[index];}
		
		_vector2& operator=(const _vector2& vec)//assignment
		{
			vals[0] = vec[0];
			vals[1] = vec[1];
			return *this;
		}
		
		_vector2& operator=(const scalar* array) //assignment
		{
			vals[0] = array[0];
			vals[1] = array[1];
			return *this;
		}
		
		void set(const scalar& new_x, const scalar& new_y)
		{
			vals[0] = new_x;
			vals[1] = new_y;
		}
		
		bool operator==(const _vector2& vec)
		{
			return (vals[0] == vec[0]) &&
					(vals[1] == vec[1]);
		}
		
		bool operator!=(const _vector2& vec)
		{
			return !((*this) == vec);
		}
		
		bool operator<(const _vector2& vec) const
		{
			if(vals[0] != vec[0])
			{
				return vals[0] < vec[0];
			}
			else
			{
				return vals[1] < vec[1];
			}
		}
		
		bool operator<=(const _vector2& vec) const
		{
			return ((*this) < vec) || ((*this) == vec);
		}

		bool operator>(const _vector2& vec) const
		{
			return !((*this) <= vec);
		}
		
		bool operator>=(const _vector2& vec) const
		{
			return !((*this) < vec);
		}

		_vector2& operator+=(const _vector2& val_to_add)
		{
			vals[0] += val_to_add[0];
			vals[1] += val_to_add[1];
			return *this;
		}
		
		_vector2& operator-=(const _vector2& val_to_subtract)
		{
			vals[0] -= val_to_subtract[0];
			vals[1] -= val_to_subtract[1];
			return *this;
		}
		
		_vector2& operator*=(const scalar& scale)
		{
			vals[0] *= scale;
			vals[1] *= scale;
			return *this;
		}

		_vector2& operator/=(const scalar& scale)
		{
			vals[0] /= scale;
			vals[1] /= scale;
			return *this;
		}
		
		scalar operator^(const _vector2& other) //cross product
		{
			return vals[0]*other[1] - vals[1]*other[0];
		}
		
		
		_vector2 operator+(const _vector2& val_to_add) const //addition
		{
			return _vector2(*this) += val_to_add;
		}
		
		_vector2 operator-(const _vector2& val_to_subtract) const //subtraction
		{
			return _vector2(*this) -= val_to_subtract;
		}
		
		_vector2 operator*(const scalar& scale) const //scaling
		{
			return _vector2(*this) *= scale;
		}
		
		_vector2 operator/(const scalar& scale) const //scaling
		{
			return _vector2(*this) /= scale;
		}
		
		scalar operator*(const _vector2& other) const //dot product
		{
			return (vals[0] * other[0]) + (vals[1] * other[1]);
		}

		_vector2 operator-() const // negation
		{
			return _vector2(-vals[0], -vals[1]);
		}
		
		scalar dot(const _vector2& v) const
		{
			return *this * v;
		}
		
		scalar cross(const _vector2& v) const
		{
			return *this ^ v;
		}
		
		scalar length_squared() const
		{
			return *this * *this;
		}
		scalar length() const
		{
			return sqrt(length_squared());
		}
		
		_vector2 unit() const
		{
			return _vector2(*this) / length();
		}
		
		
		
};


template<typename scalar>
_vector2<scalar> polar_vector2(scalar angle, scalar length)
{
	return _vector2<scalar>(sin(angle) * length, cos(angle) * length);
}

template<typename scalar>
_vector2<scalar>& normalize(_vector2<scalar>& v)
{
	v /= v.length();
	return v;
}

/*
#ifdef _GLIBCXX_OSTREAM

template <typename scalar>
std::ostream& operator<<
(std::ostream& os, const _vector2<scalar>& vec)
{
    return os<< "<" << vec.x() << ", " << vec.y() << ">";
}

#endif
*/
