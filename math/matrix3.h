#pragma once
#include "vector3.h"
#include "vector4.h"
#include "quaternion.h"

#define MY(ROW, COLUMN) vals[ROW + (COLUMN * 3)]
#define A(ROW, COLUMN) a.vals[ROW + (COLUMN * 3)]
#define B(ROW, COLUMN) b.vals[ROW + (COLUMN * 3)]
#define R(ROW, COLUMN) result.vals[ROW + (COLUMN * 3)]

template<typename scalar>
class _matrix3
{
	private:
		scalar vals[9];
		
	public:
		const scalar* val_array() const {return vals;}
		scalar& operator[](int i){return vals[i];}
		const scalar& operator[](int i) const{return vals[i];}
		
		scalar& operator()(int i){return vals[i];}
		const scalar& operator()(int i) const{return vals[i];}
		
		scalar& operator()(int row, int column){return vals[(column * 3) + row];}
		const scalar& operator()(int row, int column) const{return vals[(column * 3) + row];}
		
		_vector3< scalar >& column(int i) {return _vector3< scalar >::cast(&vals[i * 3]);}
		const _vector3< scalar >& column(int i) const{return _vector3< scalar >::cast(&vals[i * 3]);}

		const _vector3< scalar > row(int i) const
		{
			return _vector3< scalar >(vals[i], vals[i + 3], vals[i + 6]);
		}

		_matrix3()
		{}
				
		_matrix3(scalar initial_value)
		{
			for(int i = 0; i < 9; i++)
			{
				vals[i] = initial_value;
			}
		}
		
		_matrix3(const scalar* array)
		{
			for(int i = 0; i < 9; i++)
			{
				vals[i] = array[i];
			}
		}
		
		static _matrix3 identity()
		{
			_matrix3 m;
			for(int i = 0; i < 9; i++)
			{
				m[i] = (i % 4) ? 0.0 : 1.0;
			}
			return m;
		}
		
		_matrix3(_vector2< scalar > offset)
		{
			_matrix3< scalar >& m(*this);
			m = identity();
			m(0,2) = offset[0];
			m(1,2) = offset[1];
		}
		
		_matrix3& operator=(const _matrix3& other)
		{
			for(int i = 0; i < 9; i++)
			{
				vals[i] = other[i];
			}
			return *this;
		}
		
		_matrix3& operator=(const scalar& s)
		{
			for(int i = 0; i < 9; i++)
			{
				vals[i] = s;
			}
			return *this;
		}

		_matrix3& operator*=(const scalar& s)
		{
			for(int i = 0; i < 9; i++)
			{	
				vals[i] *= s;
			}
			return *this;
		}

		_vector2< scalar > operator*(const _vector2< scalar >& v) const
		{
			_vector2< scalar > result;
			result[0] = MY(0, 0) * v[0] + MY(0, 1) * v[1] + MY(0, 2);
			result[1] = MY(1, 0) * v[0] + MY(1, 1) * v[1] + MY(1, 2);
			return result;
		}

		_vector3< scalar > operator*(const _vector3< scalar >& v) const
		{
			return column(0) * v[0] + column(1) * v[1] + column(2) * v[2];
		}

		_matrix3< scalar > operator*(const _matrix3< scalar >& b) const
		{
			//const _matrix3< scalar >& a(*this);
			_matrix3< scalar > result;
			for(int i = 0; i < 3; i++)
			{
				for(int j = 0; j < 3; j++)
				{
					R(i,j) = MY(i, 0)*B(0, j) + MY(i, 1)*B(1, j) + MY(i, 2)*B(2, j);
				}
			}
			return result;
		}

		_matrix3< scalar > operator+(const _matrix3< scalar >& b) const
		{
			_matrix3< scalar > result;
			for(int i = 0; i < 3; i++)
			{
				for(int j = 0; j < 3; j++)
				{
					R(i,j) = MY(i, j) + B(i, j);
				}
			}
			return result;
		}
		
		_matrix3< scalar > operator+(const scalar& s) const
		{
			_matrix3< scalar > result;
			for(int i = 0; i < 3; i++)
			{
				for(int j = 0; j < 3; j++)
				{
					R(i,j) = MY(i, j) + s;
				}
			}
			return result;
		}

		_matrix3< scalar > operator-(const _matrix3< scalar >& b) const
		{
			_matrix3< scalar > result;
			for(int i = 0; i < 3; i++)
			{
				for(int j = 0; j < 3; j++)
				{
					R(i,j) = MY(i, j) - B(i, j);
				}
			}
			return result;
		}
		
		_matrix3< scalar > operator-(const scalar& s) const
		{
			_matrix3< scalar > result;
			for(int i = 0; i < 3; i++)
			{
				for(int j = 0; j < 3; j++)
				{
					R(i,j) = MY(i, j) - s;
				}
			}
			return result;
		}

		_vector2< scalar > get_translation() const
		{
			return _vector2< scalar >(MY(0, 2), MY(1,2));
		}
		
		_matrix3< scalar > inverse() const;
		_matrix3< scalar > transpose() const;
		scalar determinant() const;
};

template<typename scalar>
_matrix3< scalar > _matrix3< scalar >::inverse() const
{
	_matrix3 result;
	R(0, 0) = MY(2,2)*MY(1,1) - MY(2,1)*MY(1,2);
	R(0, 1) = MY(2,1)*MY(0,2) - MY(2,2)*MY(0,1);
	R(0, 2) = MY(1,2)*MY(0,1) - MY(1,1)*MY(0,2);
	R(1, 0) = MY(2,0)*MY(1,2) - MY(2,2)*MY(1,0);
	R(1, 1) = MY(2,2)*MY(0,0) - MY(2,0)*MY(0,2);
	R(1, 2) = MY(1,0)*MY(0,2) - MY(1,2)*MY(0,0);
	R(2, 0) = MY(2,1)*MY(1,0) - MY(2,0)*MY(1,1);
	R(2, 1) = MY(2,0)*MY(0,1) - MY(2,1)*MY(0,0);
	R(2, 2) = MY(1,1)*MY(0,0) - MY(1,0)*MY(0,1);
	result *= (1/determinant());
	return result;
}

template<typename scalar>
_matrix3< scalar > _matrix3< scalar >::transpose() const
{
	_matrix3 result;
	for(int i = 0; i < 3; i++)
	{
		for(int j = 0; j < 3; j++)
		{
			R(i,j) = MY(j, i);
		}
	}

	return result;
}
		
template<typename scalar>
scalar _matrix3< scalar >::determinant() const
{
	scalar value;
			
	value = MY(0,0) * (MY(2,2) * MY(1,1) - MY(2,1) * MY(1,2))
		  + MY(1,0) * (MY(2,1) * MY(0,2) - MY(2,2) * MY(0,1))
		  + MY(2,0) * (MY(1,2) * MY(0,1) - MY(1,1) * MY(0,2));
	return value;

}	

#undef MY
#undef A
#undef B
#undef R
