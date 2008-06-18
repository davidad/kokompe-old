#pragma once
#include "../math/vector2.h"

/*
//types of control objects:
point (object space locations)
offset (vector relative to specific point)
direction (normalized vector)
radius (scalar)
*/

class control_object
{
	public:

		virtual void render_static() = 0; //basic rendering function
		virtual void render_hover() = 0; //rendering for when mouse is over the object
		virtual void render_drag(_vector2<double> mouse_position) = 0;

		virtual bool check_hit(_vector2<double> position, double radius) = 0;
		//virtual void start_drag(_vector2<double> p) = 0;
		//virtual void end_drag(_vector2<double> p) = 0;
			
		
		virtual _vector2<double> get_state() = 0;
		virtual void set_state(_vector2<double> new_param) = 0;
	private:

};

class point_control
	: public control_object
{
	public:
		point_control(_vector2<double> initial_position) : my_position(initial_position) {}

		bool check_hit(_vector2<double> position, double radius);

		void render_static(); //basic rendering function
		void render_hover(); //rendering for when mouse is over the object
		void render_drag(_vector2<double> mouse_position);
		_vector2<double> get_state();		
		void set_state(_vector2<double> new_param);
	private:
		_vector2<double> my_position;
	
};
