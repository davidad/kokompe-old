#pragma once
#include <set>

#include "../math/vector2.h"
#include "control_object.h"
#include "../engine/math_string_gl_texture.h"

class design
{
    public:
		design();
		~design();

		//render an overlay of control points, relationships, and the evaluated string
		void render(_vector2<double> upper_left, _vector2<double> lower_right);

		//selects the closest control object within "radius" object units of the given point
		//object_id select(_vector2<double> point, double radius) const;

		//gets the center of a control object
		//_vector2<double> get_center(object_id obj) const;
		//void move(object_id obj, _vector2<double> new_center)

		std::string get_math_string();

		control_object* check_hit(_vector2<double> p, double r);
		void move(control_object* selected_object, _vector2<double> end_position);
 
		void add_control_point(_vector2<double> p);
    private:
		math_string_gl_texture evaluated_string;

		std::set<control_object*> my_controls;
		//std::set<relationship*> my_relationships;
		//std::set<derived_point*> ...;
		//std::set<shape*> my_shapes;
		//vector2
};






