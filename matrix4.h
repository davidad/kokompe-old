#pragma once
#include "vector3.h"
#include "quaternion.h"

#define MY(ROW, COLUMN) vals[(ROW * 4) + COLUMN]
#define A(ROW, COLUMN) a.vals[(ROW * 4) + COLUMN]
#define B(ROW, COLUMN) b.vals[(ROW * 4) + COLUMN]
#define R(ROW, COLUMN) result.vals[(ROW * 4) + COLUMN]

template<typename scalar>
class _matrix4
{
	private:
		scalar vals[16];
		
	public:
		const scalar* val_array() {return vals;}
		scalar& operator[](int i){return vals[i];}
		const scalar& operator[](int i) const{return vals[i];}
		
		scalar& operator()(int i){return vals[i];}
		const scalar& operator()(int i) const{return vals[i];}
		
		scalar& operator()(int row, int column){return vals[(row << 2) + column];}
		const scalar& operator()(int row, int column) const{return vals[(row << 2) + column];}
		
		_matrix4()
		{}
				
		_matrix4(scalar initial_value)
		{
			for(int i = 0; i < 16; i++)
			{
				vals[i] = initial_value;
			}
		}
		
		_matrix4(const scalar* array)
		{
			for(int i = 0; i < 16; i++)
			{
				vals[i] = array[i];
			}
		}
		
		static _matrix4 identity()
		{
			_matrix4 m;
			for(int i = 0; i < 16; i++)
			{
				m[i] = (i % 5) ? 0.0 : 1.0;
			}
			return m;
		}
		
		_matrix4(_vector3<scalar> offset)
		{
			_matrix4<scalar>& m(*this);
			m = identity();
			m(0,3) = offset[0];
			m(1,3) = offset[1];
			m(2,3) = offset[2];
		}

		
		static void set_to_transform(_matrix4& m, const _vector3<scalar>& offset, const _quaternion<scalar>& q)
		{
			scalar aa = q[0] * q[0];
			scalar ab2 = q[0] * q[1] * 2;
			scalar ac2 = q[0] * q[2] * 2; 
			scalar ad2 = q[0] * q[3] * 2;
						
			scalar bb = q[1] * q[1];
			scalar bc2 = q[1] * q[2] * 2; 
			scalar bd2 = q[1] * q[3] * 2; 
			
			scalar cc = q[2] * q[2];
			scalar cd2 = q[2] * q[3] * 2; 
			
			scalar dd = q[3] * q[3]; 
		
			m(0,0) = aa + bb - cc - dd;
			
			m(0,1) = bc2 - ad2;
			m(0,2) = ac2 + bd2;
			m(0,3) = offset[0];
			
			m(1,0) = ad2 + bc2;
			m(1,1) = aa - bb + cc - dd;
			m(1,2) = cd2 - ab2;
			m(1,3) = offset[1];
			
			m(2,0) = bd2 - ac2;
			m(2,1) = ab2 + cd2;
			m(2,2) = aa - bb - cc + dd;
			m(2,3) = offset[2];
			
			m(3,0) = 0;
			m(3,1) = 0;
			m(3,2) = 0;
			m(3,3) = 1;		
			
		}

		_matrix4(_vector3<scalar> t, _quaternion<scalar> q)
		{
			set_to_transform(*this, t, q);
		}
		_matrix4(_quaternion<scalar> q)
		{
			set_to_transform(*this, _vector3<scalar>(0,0,0), q);
		}

		_matrix4& operator=(const _matrix4& other);
		
		_matrix4& operator=(const scalar& s)
		{
			for(int i = 0; i < 16; i++)
			{
				vals[i] = s;
			}
			return *this;
		}

		_matrix4& operator*=(const scalar& s)
		{
			for(int i = 0; i < 16; i++)
			{	
				vals[i] *= s;
			}
			return *this;
		}

		_vector3<scalar> operator*(const _vector3<scalar>& v) const
		{
			_vector3<scalar> result;
			result[0] = MY(0, 0) * v[0] + MY(0, 1) * v[1] + MY(0, 2) * v[2] + MY(0, 3);
			result[1] = MY(1, 0) * v[0] + MY(1, 1) * v[1] + MY(1, 2) * v[2] + MY(1, 3);
			result[2] = MY(2, 0) * v[0] + MY(2, 1) * v[1] + MY(2, 2) * v[2] + MY(2, 3);
			return result;
		}
		
		_matrix4<scalar> operator*(const _matrix4<scalar>& b) const
		{
			//const _matrix4<scalar>& a(*this);
			_matrix4<scalar> result;
			for(int i = 0; i < 4; i++)
			{
				for(int j = 0; j < 4; j++)
				{
					R(i,j) = MY(i, 0)*B(0, j) + MY(i, 1)*B(1, j) + MY(i, 2)*B(2, j) + MY	(i, 3)*B(3, j);
				}
			}
			return result;
		}

		_matrix4<scalar> operator+(const _matrix4<scalar>& b) const
		{
			//const _matrix4<scalar>& a(*this);
			_matrix4<scalar> result;
			for(int i = 0; i < 4; i++)
			{
				for(int j = 0; j < 4; j++)
				{
					R(i,j) = MY(i, j) + B(i, j);
				}
			}
			return result;
		}
		
		_matrix4<scalar> operator+(const scalar& s) const
		{
			//const _matrix4<scalar>& a(*this);
			_matrix4<scalar> result;
			for(int i = 0; i < 4; i++)
			{
				for(int j = 0; j < 4; j++)
				{
					R(i,j) = MY(i, j) + s;
				}
			}
			return result;
		}

		_matrix4<scalar> operator-(const _matrix4<scalar>& b) const
		{
			//const _matrix4<scalar>& a(*this);
			_matrix4<scalar> result;
			for(int i = 0; i < 4; i++)
			{
				for(int j = 0; j < 4; j++)
				{
					R(i,j) = MY(i, j) - B(i, j);
				}
			}
			return result;
		}
		
		_matrix4<scalar> operator-(const scalar& s) const
		{
			//const _matrix4<scalar>& a(*this);
			_matrix4<scalar> result;
			for(int i = 0; i < 4; i++)
			{
				for(int j = 0; j < 4; j++)
				{
					R(i,j) = MY(i, j) - s;
				}
			}
			return result;
		}

		_vector3<scalar> get_translation() const
		{
			return _vector3<scalar>(MY(0, 3), MY(1,3), MY(2,3));
		}
		
		void copy_rotation_to(scalar* r_matrix) const
		{
			for(int i = 0; i < 3; i++)
			{
				for(int j = 0; j < 3; j++)
				{
					r_matrix[i + j * 3] = MY(i, j);
				}
			}
		}

		void copy_rotation_from(const scalar* r_matrix)
		{
			for(int i = 0; i < 3; i++)
			{
				for(int j = 0; j < 3; j++)
				{
					MY(i, j) = r_matrix[i + j * 3];
				}
			}
		}

		_matrix4<scalar> inverse() const;
		_matrix4<scalar> transpose() const;
		scalar determinant() const;
};

template<typename scalar>
_matrix4<scalar> _matrix4<scalar>::inverse() const
{
	_matrix4 result;
	R(0, 0) = MY(1, 2)*MY(2, 3)*MY(3, 1) - MY(1, 3)*MY(2, 2)*MY(3, 1) + MY(1, 3)*MY(2, 1)*MY(3, 2) - MY(1, 1)*MY(2, 3)*MY(3, 2) - MY(1, 2)*MY(2, 1)*MY(3, 3) + MY(1, 1)*MY(2, 2)*MY(3, 3);
	R(0, 1) = MY(0, 3)*MY(2, 2)*MY(3, 1) - MY(0, 2)*MY(2, 3)*MY(3, 1) - MY(0, 3)*MY(2, 1)*MY(3, 2) + MY(0, 1)*MY(2, 3)*MY(3, 2) + MY(0, 2)*MY(2, 1)*MY(3, 3) - MY(0, 1)*MY(2, 2)*MY(3, 3);
	R(0, 2) = MY(0, 2)*MY(1, 3)*MY(3, 1) - MY(0, 3)*MY(1, 2)*MY(3, 1) + MY(0, 3)*MY(1, 1)*MY(3, 2) - MY(0, 1)*MY(1, 3)*MY(3, 2) - MY(0, 2)*MY(1, 1)*MY(3, 3) + MY(0, 1)*MY(1, 2)*MY(3, 3);
	R(0, 3) = MY(0, 3)*MY(1, 2)*MY(2, 1) - MY(0, 2)*MY(1, 3)*MY(2, 1) - MY(0, 3)*MY(1, 1)*MY(2, 2) + MY(0, 1)*MY(1, 3)*MY(2, 2) + MY(0, 2)*MY(1, 1)*MY(2, 3) - MY(0, 1)*MY(1, 2)*MY(2, 3);
	R(1, 0) = MY(1, 3)*MY(2, 2)*MY(3, 0) - MY(1, 2)*MY(2, 3)*MY(3, 0) - MY(1, 3)*MY(2, 0)*MY(3, 2) + MY(1, 0)*MY(2, 3)*MY(3, 2) + MY(1, 2)*MY(2, 0)*MY(3, 3) - MY(1, 0)*MY(2, 2)*MY(3, 3);
	R(1, 1) = MY(0, 2)*MY(2, 3)*MY(3, 0) - MY(0, 3)*MY(2, 2)*MY(3, 0) + MY(0, 3)*MY(2, 0)*MY(3, 2) - MY(0, 0)*MY(2, 3)*MY(3, 2) - MY(0, 2)*MY(2, 0)*MY(3, 3) + MY(0, 0)*MY(2, 2)*MY(3, 3);
	R(1, 2) = MY(0, 3)*MY(1, 2)*MY(3, 0) - MY(0, 2)*MY(1, 3)*MY(3, 0) - MY(0, 3)*MY(1, 0)*MY(3, 2) + MY(0, 0)*MY(1, 3)*MY(3, 2) + MY(0, 2)*MY(1, 0)*MY(3, 3) - MY(0, 0)*MY(1, 2)*MY(3, 3);
	R(1, 3) = MY(0, 2)*MY(1, 3)*MY(2, 0) - MY(0, 3)*MY(1, 2)*MY(2, 0) + MY(0, 3)*MY(1, 0)*MY(2, 2) - MY(0, 0)*MY(1, 3)*MY(2, 2) - MY(0, 2)*MY(1, 0)*MY(2, 3) + MY(0, 0)*MY(1, 2)*MY(2, 3);
	R(2, 0) = MY(1, 1)*MY(2, 3)*MY(3, 0) - MY(1, 3)*MY(2, 1)*MY(3, 0) + MY(1, 3)*MY(2, 0)*MY(3, 1) - MY(1, 0)*MY(2, 3)*MY(3, 1) - MY(1, 1)*MY(2, 0)*MY(3, 3) + MY(1, 0)*MY(2, 1)*MY(3, 3);
	R(2, 1) = MY(0, 3)*MY(2, 1)*MY(3, 0) - MY(0, 1)*MY(2, 3)*MY(3, 0) - MY(0, 3)*MY(2, 0)*MY(3, 1) + MY(0, 0)*MY(2, 3)*MY(3, 1) + MY(0, 1)*MY(2, 0)*MY(3, 3) - MY(0, 0)*MY(2, 1)*MY(3, 3);
	R(2, 2) = MY(0, 1)*MY(1, 3)*MY(3, 0) - MY(0, 3)*MY(1, 1)*MY(3, 0) + MY(0, 3)*MY(1, 0)*MY(3, 1) - MY(0, 0)*MY(1, 3)*MY(3, 1) - MY(0, 1)*MY(1, 0)*MY(3, 3) + MY(0, 0)*MY(1, 1)*MY(3, 3);
	R(2, 3) = MY(0, 3)*MY(1, 1)*MY(2, 0) - MY(0, 1)*MY(1, 3)*MY(2, 0) - MY(0, 3)*MY(1, 0)*MY(2, 1) + MY(0, 0)*MY(1, 3)*MY(2, 1) + MY(0, 1)*MY(1, 0)*MY(2, 3) - MY(0, 0)*MY(1, 1)*MY(2, 3);
	R(3, 0) = MY(1, 2)*MY(2, 1)*MY(3, 0) - MY(1, 1)*MY(2, 2)*MY(3, 0) - MY(1, 2)*MY(2, 0)*MY(3, 1) + MY(1, 0)*MY(2, 2)*MY(3, 1) + MY(1, 1)*MY(2, 0)*MY(3, 2) - MY(1, 0)*MY(2, 1)*MY(3, 2);
	R(3, 1) = MY(0, 1)*MY(2, 2)*MY(3, 0) - MY(0, 2)*MY(2, 1)*MY(3, 0) + MY(0, 2)*MY(2, 0)*MY(3, 1) - MY(0, 0)*MY(2, 2)*MY(3, 1) - MY(0, 1)*MY(2, 0)*MY(3, 2) + MY(0, 0)*MY(2, 1)*MY(3, 2);
	R(3, 2) = MY(0, 2)*MY(1, 1)*MY(3, 0) - MY(0, 1)*MY(1, 2)*MY(3, 0) - MY(0, 2)*MY(1, 0)*MY(3, 1) + MY(0, 0)*MY(1, 2)*MY(3, 1) + MY(0, 1)*MY(1, 0)*MY(3, 2) - MY(0, 0)*MY(1, 1)*MY(3, 2);
	R(3, 3) = MY(0, 1)*MY(1, 2)*MY(2, 0) - MY(0, 2)*MY(1, 1)*MY(2, 0) + MY(0, 2)*MY(1, 0)*MY(2, 1) - MY(0, 0)*MY(1, 2)*MY(2, 1) - MY(0, 1)*MY(1, 0)*MY(2, 2) + MY(0, 0)*MY(1, 1)*MY(2, 2);
	result *= (1/determinant());
	return result;
}

template<typename scalar>
_matrix4<scalar> _matrix4<scalar>::transpose() const
{
	_matrix4 result;
	for(int i = 0; i < 4; i++)
	{
		for(int j = 0; j < 4; j++)
		{
			R(i,j) = MY(j, i);
		}
	}

	return result;
}
		
template<typename scalar>
scalar _matrix4<scalar>::determinant() const
{
	scalar value;
			
	value =
	MY(0, 3) * MY(1, 2) * MY(2, 1) * MY(3, 0)-MY(0, 2) * MY(1, 3) * MY(2, 1) * MY(3, 0)-MY(0, 3) * MY(1, 1) * MY(2, 2) * MY(3, 0)+MY(0, 1) * MY(1, 3) * MY(2, 2) * MY(3, 0)+
	MY(0, 2) * MY(1, 1) * MY(2, 3) * MY(3, 0)-MY(0, 1) * MY(1, 2) * MY(2, 3) * MY(3, 0)-MY(0, 3) * MY(1, 2) * MY(2, 0) * MY(3, 1)+MY(0, 2) * MY(1, 3) * MY(2, 0) * MY(3, 1)+
	MY(0, 3) * MY(1, 0) * MY(2, 2) * MY(3, 1)-MY(0, 0) * MY(1, 3) * MY(2, 2) * MY(3, 1)-MY(0, 2) * MY(1, 0) * MY(2, 3) * MY(3, 1)+MY(0, 0) * MY(1, 2) * MY(2, 3) * MY(3, 1)+
	MY(0, 3) * MY(1, 1) * MY(2, 0) * MY(3, 2)-MY(0, 1) * MY(1, 3) * MY(2, 0) * MY(3, 2)-MY(0, 3) * MY(1, 0) * MY(2, 1) * MY(3, 2)+MY(0, 0) * MY(1, 3) * MY(2, 1) * MY(3, 2)+
	MY(0, 1) * MY(1, 0) * MY(2, 3) * MY(3, 2)-MY(0, 0) * MY(1, 1) * MY(2, 3) * MY(3, 2)-MY(0, 2) * MY(1, 1) * MY(2, 0) * MY(3, 3)+MY(0, 1) * MY(1, 2) * MY(2, 0) * MY(3, 3)+
	MY(0, 2) * MY(1, 0) * MY(2, 1) * MY(3, 3)-MY(0, 0) * MY(1, 2) * MY(2, 1) * MY(3, 3)-MY(0, 1) * MY(1, 0) * MY(2, 2) * MY(3, 3)+MY(0, 0) * MY(1, 1) * MY(2, 2) * MY(3, 3);
	return value;

}	

template<typename scalar>
inline _matrix4<scalar>& _matrix4<scalar>::operator=(const _matrix4<scalar>& other)
{
	for(int i = 0; i < 16; i++)
	{
		vals[i] = other[i];
	}
	return *this;
}

#undef MY
#undef A
#undef B
#undef R
