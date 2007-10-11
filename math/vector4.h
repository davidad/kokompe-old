#pragma once
#include <cmath>

template<typename scalar>
class _vector4
{
	private:
		scalar vals[4];
		
	public:
		scalar& x() {return vals[0];}
		scalar& y() {return vals[1];}
		scalar& z() {return vals[2];}
		scalar& w() {return vals[3];}

		const scalar& x() const {return vals[0];}
		const scalar& y() const {return vals[1];}
		const scalar& z() const {return vals[2];}
		const scalar& w() const {return vals[2];}

		_vector4() {}
			
		_vector4(scalar const* array)
		{
			vals[0] = array[0];
			vals[1] = array[1];
			vals[2] = array[2];
			vals[3] = array[3];
		}
		
		_vector4(const _vector4& base)
		{
			vals[0] = base.vals[0];
			vals[1] = base.vals[1];
			vals[2] = base.vals[2];
			vals[3] = base.vals[3];
		}
		_vector4(scalar new_x, scalar new_y, scalar new_z, scalar new_w)
		{
			vals[0] = new_x;
			vals[1] = new_y;
			vals[2] = new_z;
			vals[3] = new_w;
		}	
	
		scalar* as_array(){return vals;}
		const scalar* as_array() const{return vals;}
		
		operator scalar*() {return vals;}
		operator scalar const*() const {return vals;}

		scalar& operator[](int index) {return vals[index];}
		const scalar& operator[](int index) const {return vals[index];}
		
		_vector4& operator=(const _vector4& vec)//assignment
		{
			vals[0] = vec[0];
			vals[1] = vec[1];
			vals[2] = vec[2];
			vals[3] = vec[3];
			return *this;
		}
		
		_vector4& operator=(const _vector3< scalar >& vec)//assignment
		{
			vals[0] = vec[0];
			vals[1] = vec[1];
			vals[2] = vec[2];
			vals[3] = 0;
			return *this;
		}
		
		_vector4& operator=(const scalar* array) //assignment
		{
			vals[0] = array[0];
			vals[1] = array[1];
			vals[2] = array[2];
			vals[3] = array[3];
			return *this;
		}
		
		void set(const scalar& new_x, const scalar& new_y, const scalar& new_z, const scalar& new_w)
		{
			vals[0] = new_x;
			vals[1] = new_y;
			vals[2] = new_z;
			vals[3] = new_w;
		}
		
		bool operator==(const _vector4& vec) const
		{
			return (vals[0] == vec[0]) &&
					(vals[1] == vec[1]) &&
					(vals[2] == vec[2]) &&
					(vals[3] == vec[3]);
		}
		
		bool operator!=(const _vector4& vec) const
		{
			return !((*this) == vec);
		}
		
		bool operator<(const _vector4& vec) const
		{
			if(vals[0] != vec[0]) return vals[0] < vec[0];
			if(vals[1] != vec[1]) return vals[1] < vec[1];
			if(vals[2] != vec[2]) return vals[2] < vec[2];
			if(vals[3] != vec[3]) return vals[3] < vec[3];
			return false;
		}
		
		bool operator<=(const _vector4& vec) const
		{
			return (*this) < vec || (*this) == vec;
		}

		bool operator>(const _vector4& vec) const
		{
			return (*this) < vec || (*this) == vec;
		}
		
		bool operator>=(const _vector4& vec) const
		{
			return !((*this) < vec);
		}


		_vector4& operator+=(const _vector4& val_to_add)
		{
			vals[0] += val_to_add[0];
			vals[1] += val_to_add[1];
			vals[2] += val_to_add[2];
			vals[3] += val_to_add[3];
			return *this;
		}
		
		_vector4& operator-=(const _vector4& val_to_subtract)
		{
			vals[0] -= val_to_subtract[0];
			vals[1] -= val_to_subtract[1];
			vals[2] -= val_to_subtract[2];
			vals[3] -= val_to_subtract[3];
			return *this;
		}
		
		_vector4& operator*=(const scalar& scale)
		{
			vals[0] *= scale;
			vals[1] *= scale;
			vals[2] *= scale;
			vals[3] *= scale;
			return *this;
		}

		_vector4& operator/=(const scalar& scale)
		{
			vals[0] /= scale;
			vals[1] /= scale;
			vals[2] /= scale;
			vals[3] /= scale;
			return *this;
		}
		
		_vector4& operator^=(const _vector4& other) //cross product
		{
			scalar temp0 = vals[1]*other[2] - vals[2]*other[1];
			scalar temp1 = vals[2]*other[0] - vals[0]*other[2];
  			vals[2] = vals[0]*other[1] - vals[1]*other[0];
			vals[0] = temp0;
			vals[1] = temp1;
			
			return *this;
		}
		
		
		_vector4 operator+(const _vector4& val_to_add) const //addition
		{
			return _vector4(*this) += val_to_add;
		}
		
		_vector4 operator-(const _vector4& val_to_subtract) const //subtraction
		{
			return _vector4(*this) -= val_to_subtract;
		}
		
		_vector4 operator*(const scalar& scale) const //scaling
		{
			return _vector4(*this) *= scale;
		}
		
		_vector4 operator/(const scalar& scale) const //scaling
		{
			return _vector4(*this) /= scale;
		}
		
		scalar operator*(const _vector4& other) const //dot product
		{
			return (vals[0] * other[0]) +
				   (vals[1] * other[1]) +
				   (vals[2] * other[2]) +
				   (vals[3] * other[3]);
		}

		_vector4 operator^(const _vector4& other) const //cross product
		{
			return _vector4(*this) ^= other;
		}
		
		_vector4 operator-() const // negation
		{
			return _vector4(-vals[0], -vals[1], -vals[2], -vals[3]);
		}
		
		scalar dot(const _vector4& v) const
		{
			return *this * v;
		}
		
		_vector4 cross(const _vector4& v) const
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
		
		_vector4 unit() const
		{
			return _vector4(*this) / length();
		}
		
		static _vector4< scalar >& cast(scalar* array)
		{
			return *((_vector4*)(array));
		}

		static _vector4< scalar > const& cast(scalar const* array)
		{
			return *((_vector4 const*)(array));
		}

};

template<typename scalar>
_vector4< scalar >& normalize(_vector4< scalar >& v)
{
	v /= v.length();
	return v;
}

