#pragma once
#include <cmath>
#include "vector3.h"

template<typename scalar>
class _quaternion
{
	private:
		scalar vals[4];
	
	public:
		const scalar& a() const {return vals[0];}
		const scalar& b() const {return vals[1];}
		const scalar& c() const {return vals[2];}
		const scalar& d() const {return vals[3];}

		_quaternion() {}
		_quaternion(const scalar* array)
		{
			vals[0] = array[0];
			vals[1] = array[1];
			vals[2] = array[2];
			vals[3] = array[3];
		}
		
		_quaternion(const _quaternion& base)
		{
			vals[0] = base.vals[0];
			vals[1] = base.vals[1];
			vals[2] = base.vals[2];
			vals[3] = base.vals[3];
		}
		
		_quaternion(scalar new_a, scalar new_b, scalar new_c, scalar new_d)
		{
			vals[0] = new_a;
			vals[1] = new_b;
			vals[2] = new_c;
			vals[3] = new_d;
		}
		
		_quaternion(scalar new_a, _vector3<scalar> v)
		{
			vals[0] = new_a;
			vals[1] = v[0];
			vals[2] = v[1];
			vals[3] = v[2];
		}	
	
		scalar* as_array(){return vals;}
		const scalar* as_array() const{return vals;}
		
		scalar& operator[](int index) {return vals[index];}
		const scalar& operator[](int index) const {return vals[index];}
		
		_quaternion& operator=(const _quaternion& q)//assignment
		{
			vals[0] = q[0];
			vals[1] = q[1];
			vals[2] = q[2];
			vals[3] = q[3];
			return (*this);
		}
		
		_quaternion& operator=(const scalar* array) //assignment
		{
			vals[0] = array[0];
			vals[1] = array[1];
			vals[2] = array[2];
			vals[3] = array[3];
			return (*this);
		}
		
		void set(const scalar& new_a, const scalar& new_b,
				const scalar& new_c,  const scalar& new_d)
		{
			vals[0] = new_a;
			vals[1] = new_b;
			vals[2] = new_c;
			vals[3] = new_d;
		}
		
		bool operator==(const _quaternion& q)
		{
			return (vals[0] == q[0]) &&
					(vals[1] == q[1]) &&
					(vals[2] == q[2]) &&
					(vals[3] == q[3]);
		}
		
		bool operator!=(const _quaternion& vec)
		{
			return !((*this) == vec);
		}
		

		_quaternion& operator+=(const _quaternion& val_to_add)
		{
			vals[0] += val_to_add[0];
			vals[1] += val_to_add[1];
			vals[2] += val_to_add[2];
			vals[3] += val_to_add[3];
			return *this;
		}
		
		_quaternion& operator-=(const _quaternion& val_to_subtract)
		{
			vals[0] -= val_to_subtract[0];
			vals[1] -= val_to_subtract[1];
			vals[2] -= val_to_subtract[2];
			vals[3] -= val_to_subtract[3];
			return *this;
		}
		
		_quaternion& operator*=(const scalar& scale)
		{
			vals[0] *= scale;
			vals[1] *= scale;
			vals[2] *= scale;
			vals[3] *= scale;
			return *this;
		}

		_quaternion operator* (const _quaternion& q) const
		{
			_quaternion c;
			c[0] = vals[0]*q[0] - vals[1]*q[1] - vals[2]*q[2] - vals[3]*q[3];
		    c[1] = vals[0]*q[1] + vals[1]*q[0] + vals[2]*q[3] - vals[3]*q[2];
    		c[2] = vals[0]*q[2] - vals[1]*q[3] + vals[2]*q[0] + vals[3]*q[1];
    		c[3] = vals[0]*q[3] + vals[1]*q[2] - vals[2]*q[1] + vals[3]*q[0];

			return c;
		}

		_quaternion& operator*= (const _quaternion& q)
		{
			(*this) = (*this) * q;
			return (*this);
		}
		
		_quaternion& operator/=(const scalar& scale)
		{
			vals[0] /= scale;
			vals[1] /= scale;
			vals[2] /= scale;
			vals[3] /= scale;
			return *this;
		}
		
		_quaternion& operator/=(const _quaternion& q)
		{
			scalar a = vals[0];
			scalar b = vals[1];
			scalar c = vals[2];
			scalar d = vals[3];
			
			scalar denominator = q[0]*q[0]+q[1]*q[1]+q[2]*q[2]+q[3]*q[3];
			
			vals[0] = (+a*q[0]+b*q[1]+c*q[2]+d*q[3])/denominator;    //(a*ar+b*br+c*cr+d*dr)/denominator;
			vals[1] = (-a*q[1]+b*q[0]-c*q[3]+d*q[2])/denominator;    //((ar*b-a*br)+(cr*d-c*dr))/denominator;
			vals[2] = (-a*q[2]+b*q[3]+c*q[0]-d*q[1])/denominator;    //((ar*c-a*cr)+(dr*b-d*br))/denominator;
			vals[3] = (-a*q[3]-b*q[2]+c*q[1]+d*q[0])/denominator;    //((ar*d-a*dr)+(br*c-b*cr))/denominator;
						
			return(*this);
		}
		
		_quaternion operator/(const _quaternion& q) const
		{
			return _quaternion(*this) / q;
		}
		
		_quaternion operator+(const _quaternion& val_to_add) const //addition
		{
			return _quaternion(*this) += val_to_add;
		}
		
		_quaternion operator-(const _quaternion& val_to_subtract) const //subtraction
		{
			return _quaternion(*this) -= val_to_subtract;
		}
		
		_quaternion operator*(const scalar& scale) const //scaling
		{
			return _quaternion(*this) *= scale;
		}
		
		_quaternion operator/(const scalar& scale) const //scaling
		{
			return _quaternion(*this) /= scale;
		}
		
		_quaternion operator-() const // negation
		{
			return _quaternion(-vals[0], -vals[1], -vals[2], -vals[3]);
		}
		
		_quaternion conjugate() const
		{
			return _quaternion(vals[0], -vals[1], -vals[2], -vals[3]);
		}
		
		_quaternion inverse() const
		{
			return conjugate() / ((*this) * conjugate());
		}
		
		scalar magnitude() const
		{
			return sqrt(vals[0] * vals[0] + vals[1] * vals[1] + vals[2] * vals[2] + vals[3] * vals[3]);
		}
		
		scalar magnitude_squared() const
		{
			return (vals[0] * vals[0] + vals[1] * vals[1] + vals[2] * vals[2] + vals[3] * vals[3]);
		}
		
		
		_quaternion unit() const
		{
			return _quaternion(*this) / magnitude();
		}
		
		static _quaternion from_angle_axis(const scalar& angle, const _vector3<scalar>& axis)
		{
			scalar t = sin(angle / 2) / axis.length();
			return _quaternion(cos(angle / 2), axis[0] * t, axis[1] * t, axis[2] * t);
		}

		static _quaternion from_roll_pitch_yaw(const _vector3<scalar>& rpy)
		{
		    scalar halfroll = rpy[0] / 2;
			scalar halfpitch = rpy[1] / 2;
			scalar halfyaw = rpy[2] / 2;
		
			scalar sin_r2 = sin( halfroll );
			scalar sin_p2 = sin( halfpitch );
			scalar sin_y2 = sin( halfyaw );
		
			scalar cos_r2 = cos( halfroll );
			scalar cos_p2 = cos( halfpitch );
			scalar cos_y2 = cos( halfyaw );
		
			_quaternion q;
			q[0] = cos_r2 * cos_p2 * cos_y2 + sin_r2 * sin_p2 * sin_y2;
			q[1] = sin_r2 * cos_p2 * cos_y2 - cos_r2 * sin_p2 * sin_y2;
			q[2] = cos_r2 * sin_p2 * cos_y2 + sin_r2 * cos_p2 * sin_y2;
			q[3] = cos_r2 * cos_p2 * sin_y2 - sin_r2 * sin_p2 * cos_y2;
			return q;
		}
		
		_vector3<scalar> to_roll_pitch_yaw()
		{
			scalar roll_a =     2 * (vals[0]*vals[1] + vals[2]*vals[3]);
			scalar roll_b = 1 - 2 * (vals[1]*vals[1] + vals[2]*vals[2]);

			scalar pitch_sin = 2 * (vals[0]*vals[2] - vals[3]*vals[1]);
			
			scalar yaw_a =     2 * (vals[0]*vals[3] + vals[1]*vals[2]);
			scalar yaw_b = 1 - 2 * (vals[2]*vals[2] + vals[3]*vals[3]);

			//roll = atan2( roll_a, roll_b );
			//pitch = asin( pitch_sin );
			//yaw = atan2( yaw_a, yaw_b );
			
			return _vector3<scalar>(atan2(roll_a, roll_b), asin(pitch_sin), atan2(yaw_a, yaw_b));
		}

		_vector3<scalar> rotate(const _vector3<scalar>& v) const
		{
			_quaternion b(0, v[0], v[1], v[2]);
			
			//assumes that *this is a unit vector
			b = (*this) * b * (*this).conjugate();
			
			//if *this is not a unit vector
			b /= magnitude_squared();
			
			return _vector3<scalar>(b[1], b[2], b[3]);
		}
				
};

template <typename scalar>
_quaternion<scalar>& normalize(_quaternion<scalar>& q)
{	
	q /= q.magnitude();	
	return q;
}

template <typename scalar>
_quaternion<scalar> operator/(const scalar& s, const _quaternion<scalar>& q)
{
	return s * q.inverse();
}

template <typename scalar>
_quaternion<scalar> operator*(const scalar& s, const _quaternion<scalar>& q)
{
	return q * s;
}
