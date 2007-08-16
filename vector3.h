#pragma once
#include <cmath>

template<typename scalar>
class _vector3
{
	private:
		scalar vals[3];
		
	public:
		scalar& x() {return vals[0];}
		scalar& y() {return vals[1];}
		scalar& z() {return vals[2];}

		const scalar& x() const {return vals[0];}
		const scalar& y() const {return vals[1];}
		const scalar& z() const {return vals[2];}

		_vector3() {}
			
		_vector3(const scalar* array)
		{
			vals[0] = array[0];
			vals[1] = array[1];
			vals[2] = array[2];
		}
		
		_vector3(const _vector3& base)
		{
			vals[0] = base.vals[0];
			vals[1] = base.vals[1];
			vals[2] = base.vals[2];
		}
		_vector3(scalar new_x, scalar new_y, scalar new_z)
		{
			vals[0] = new_x;
			vals[1] = new_y;
			vals[2] = new_z;
		}	
	
		scalar* as_array(){return vals;}
		const scalar* as_array() const{return vals;}
		
		scalar& operator[](int index) {return vals[index];}
		const scalar& operator[](int index) const {return vals[index];}
		
		_vector3& operator=(const _vector3& vec)//assignment
		{
			vals[0] = vec[0];
			vals[1] = vec[1];
			vals[2] = vec[2];
			return *this;
		}
		
		_vector3& operator=(const scalar* array) //assignment
		{
			vals[0] = array[0];
			vals[1] = array[1];
			vals[2] = array[2];
			return *this;
		}
		
		void set(const scalar& new_x, const scalar& new_y, const scalar& new_z)
		{
			vals[0] = new_x;
			vals[1] = new_y;
			vals[2] = new_z;
		}
		
		bool operator==(const _vector3& vec)
		{
			return (vals[0] == vec[0]) &&
					(vals[1] == vec[1]) &&
					(vals[2] == vec[2]);
		}
		
		bool operator!=(const _vector3& vec)
		{
			return !((*this) == vec);
		}
		
		bool operator<(const _vector3& vec)
		{
			return length_squared() < vec.length_squared();
		}
		
		bool operator<=(const _vector3& vec)
		{
			return length_squared() <= vec.length_squared();
		}

		bool operator>(const _vector3& vec)
		{
			return length_squared() > vec.length_squared();
		}
		
		bool operator>=(const _vector3& vec)
		{
			return length_squared() >= vec.length_squared();
		}

		_vector3& operator+=(const _vector3& val_to_add)
		{
			vals[0] += val_to_add[0];
			vals[1] += val_to_add[1];
			vals[2] += val_to_add[2];
			return *this;
		}
		
		_vector3& operator-=(const _vector3& val_to_subtract)
		{
			vals[0] -= val_to_subtract[0];
			vals[1] -= val_to_subtract[1];
			vals[2] -= val_to_subtract[2];
			return *this;
		}
		
		_vector3& operator*=(const scalar& scale)
		{
			vals[0] *= scale;
			vals[1] *= scale;
			vals[2] *= scale;
			return *this;
		}

		_vector3& operator/=(const scalar& scale)
		{
			vals[0] /= scale;
			vals[1] /= scale;
			vals[2] /= scale;
			return *this;
		}
		
		_vector3& operator^=(const _vector3& other) //cross product
		{
			scalar temp0 = vals[1]*other[2] - vals[2]*other[1];
			scalar temp1 = vals[2]*other[0] - vals[0]*other[2];
  			vals[2] = vals[0]*other[1] - vals[1]*other[0];
			vals[0] = temp0;
			vals[1] = temp1;
			
			return *this;
		}
		
		
		_vector3 operator+(const _vector3& val_to_add) const //addition
		{
			return _vector3(*this) += val_to_add;
		}
		
		_vector3 operator-(const _vector3& val_to_subtract) const //subtraction
		{
			return _vector3(*this) -= val_to_subtract;
		}
		
		_vector3 operator*(const scalar& scale) const //scaling
		{
			return _vector3(*this) *= scale;
		}
		
		_vector3 operator/(const scalar& scale) const //scaling
		{
			return _vector3(*this) /= scale;
		}
		
		scalar operator*(const _vector3& other) const //dot product
		{
			return (vals[0] * other[0]) + (vals[1] * other[1]) + (vals[2] * other[2]);
		}

		_vector3 operator^(const _vector3& other) const //cross product
		{
			return _vector3(*this) ^= other;
		}
		
		_vector3 operator-() const // negation
		{
			return _vector3(-vals[0], -vals[1], -vals[2]);
		}
		
		scalar dot(const _vector3& v) const
		{
			return *this * v;
		}
		
		_vector3 cross(const _vector3& v) const
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
		
		_vector3 unit() const
		{
			return _vector3(*this) / length();
		}
		
};

template<typename scalar>
_vector3<scalar>& normalize(_vector3<scalar>& v)
{
	v /= v.length();
	return v;
}

