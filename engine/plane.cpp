#include "plane.h"
#include "vector.h"


// Construct a plane.
plane_t::plane_t() {
	n.set(1.0f, 0.0f, 0.0f);
	d = 0.0f;
}

// Construct a plane from a point on the plane and the normal vector
// to the plane. 
void plane_t::set(vector_t point, vector_t normal) {
	n = normal;
	d = dot(point, normal);
}

// Construct a line.
line_t::line_t() {
	v.set(1.0f, 0.0f, 0.0f);
	p.set(0.0f, 0.0f, 0.0f);
}

// Construct a line from a point on the line and a vector
// parallel to the line
void line_t::set(vector_t point, vector_t line_vector) {
	v = line_vector;
	p = point;


}

// Find the intersection point of three mutually non-parallel planes

// (Right now this function does no checking to see if the planes are not
// parallel -- this should be done at a higher level)

vector_t plane_t::three_plane_intersection(plane_t &p1, plane_t &p2, plane_t &p3) {
	// (See http://local.wasp.uwa.edu.au/~pboure/geometry/3planes/)
	vector_t t1 = mul((cross(p2.n,p3.n)), p1.d);
	vector_t t2 = mul((cross(p3.n,p1.n)), p2.d);
	vector_t t3 = mul((cross(p1.n,p2.n)), p3.d);
	float denom = dot(p1.n, cross(p2.n,p3.n));
	
	vector_t intersection = mul(add(add(t1,t2),t3), 1/denom);
	return(intersection);
}

// Finds the line at the intersectioin of two non-parallel planes.  (Right now
// this function does not check to insure that the planes are not parallel.)

line_t plane_t::two_plane_intersection(plane_t &p1, plane_t &p2) {
	// See http://local.wasp.uwa.edu.au/~pboure/geometry/planeplane/)
	line_t line;

	// vector along line --- cross product of plane normals
	line.v = cross(p1.n,p2.n);

	// point on line
	float determinant = dot(p1.n,p1.n)*dot(p2.n,p2.n) - dot(p1.n,p2.n)*dot(p1.n,p2.n);
	float c1 = (p1.d*dot(p2.n,p2.n) - p2.d*dot(p1.n,p2.n))/determinant;
	float c2 = (p2.d*dot(p1.n,p1.n) - p1.d*dot(p1.n,p2.n))/determinant;
	line.p = add(mul(p1.n,c1), mul(p2.n,c2));

	return(line);
}

int line_t::line_box_intersection(line_t &line, vector_t box_lower, vector_t box_upper, vector_t *inside_point) {
	float t, x, y, z;
	int i_count = 0;
	vector_t intersection[6];    // this is maybe inefficient...can only be 2 intersections
								// but want to avoid wierd crashes for math errors

	// Determines if a line intersects the interior volume of a rectangular box,
	// and if so, returns a point inside the box on the line.

	// For each of the six faces of the box, test to see if the box intersects the face
	
	// LOWER X FACE
	t = (box_lower.x - line.p.x) / line.v.x;  // line parameter t on the face
	x = box_lower.x;
	y = line.p.y + t*line.v.y;
	z = line.p.z + t*line.v.z;
	if ((y > box_lower.y) && (y < box_upper.y) &&
		(z > box_lower.z) && (z < box_upper.z)) 
			intersection[i_count++].set(x,y,z);  // line intersects this face!

	// UPPER X FACE
	t = (box_upper.x - line.p.x) / line.v.x;  // line parameter t on the face
	x = box_upper.x;
	y = line.p.y + t*line.v.y;
	z = line.p.z + t*line.v.z;
	if ((y > box_lower.y) && (y < box_upper.y) &&
		(z > box_lower.z) && (z < box_upper.z)) 
			intersection[i_count++].set(x,y,z);  // line intersects this face!

	// LOWER Y FACE
	t = (box_lower.y - line.p.y) / line.v.y;  // line parameter t on the face
	x = line.p.x + t*line.v.x;
	y = box_lower.y;
	z = line.p.z + t*line.v.z;
	if ((x > box_lower.x) && (x < box_upper.x) &&
		(z > box_lower.z) && (z < box_upper.z)) 
			intersection[i_count++].set(x,y,z);  // line intersects this face!

	// UPPER Y FACE
	t = (box_upper.y - line.p.y) / line.v.y;  // line parameter t on the face
	x = line.p.x + t*line.v.x;
	y = box_upper.y; 
	z = line.p.z + t*line.v.z;
	if ((x > box_lower.x) && (x < box_upper.x) &&
		(z > box_lower.z) && (z < box_upper.z)) 
			intersection[i_count++].set(x,y,z);  // line intersects this face!

	// LOWER Z FACE
	t = (box_lower.z - line.p.z) / line.v.z;  // line parameter t on the face
	x = line.p.x + t*line.v.x;
	y = line.p.y + t*line.v.y;
	z = box_lower.z;
	if ((x > box_lower.x) && (x < box_upper.x) &&
		(y > box_lower.y) && (y < box_upper.y)) 
			intersection[i_count++].set(x,y,z);  // line intersects this face!

	// UPPER Z FACE
	t = (box_upper.z - line.p.z) / line.v.z;  // line parameter t on the face
	x = line.p.x + t*line.v.x;
	y = line.p.y + t*line.v.y;
	z = box_upper.z;
	if ((x > box_lower.x) && (x < box_upper.x) &&
		(y > box_lower.y) && (y < box_upper.y)) 
			intersection[i_count++].set(x,y,z);  // line intersects this face!

	
	if (i_count == 0) {
		return(0);
	}
	else {
		if (i_count == 2) {
			// box does intersect!
			*inside_point = midpoint(intersection[0], intersection[1]);
			return(1);
		}
		else {
			cerr << "Line intersects box interior in " << i_count << " places, a mathematical impossibility." << endl;
			return(0);
		}
	}
}


// Finds the nearest point to point on line line, returns in *near_point.
int line_t::nearest_point_on_line(line_t &line, vector_t point, vector_t *near_point) {
  vector_t nv = normalize(line.v);       // Compute normalized line vector (points direction from line.p to desired point A)
  vector_t lp_p = sub(point, line.p);        // Compute vector from line.p to point)
  vector_t pv = mul(nv, dot(lp_p, nv));  // get vector from line origin P to pt. A 
  *near_point = add(line.p, pv);
  return(0);
}

