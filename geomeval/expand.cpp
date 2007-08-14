#include "expand.h"
#include "array_2d.h"
// +-----> +x
// | 
// |
// V
//+y

//this is essentially an offset to the closest point
class edist
{
	public:
		edist(){}
		edist(int new_dx, int new_dy) : dx(new_dx), dy(new_dy){}
		int dx, dy;
		bool operator<(const edist& o) const{return (dx * dx + dy * dy < o.dx * o.dx + o.dy * o.dy);}
		bool operator<=(const double& o) const{return (dx * dx + dy * dy <= o);}
		void operator=(const edist& o) {dx = o.dx; dy = o.dy;}
		void choose(const edist& o)
		{
			if(o < (*this))
			{
				(*this) = o;
			}
		}
		edist above() { return edist(dx, dy - 1);} //get edist that would be from the cell above this one
		edist below() { return edist(dx, dy + 1);}
		edist left() { return edist(dx - 1, dy);}
		edist right() { return edist(dx + 1, dy);}
};	

void expand_dist_map(array_2d<edist>& dists)
{
	const unsigned int width(dists.width);
	const unsigned int height(dists.height);
	const unsigned int size(dists.size);
	
	//12
	//34
	
	//1234 pass
	for(int i = 0; i < size; i++)
	{
		if((i % width)) dists[i].choose(dists[i - 1].right()); //left->right
		if(i > width) dists[i].choose(dists[i - width].below()); //above->below
	}

	//4321 pass
	for(int i = size - 1; i >= 0; i--)
	{
		if((i % width) + 1 < width) dists[i].choose(dists[i + 1].left()); //right->left
		if((i + width) < size) dists[i].choose(dists[i + width].above()); //below->above
	}
	
	//2143 pass
	for(int x = width - 1; x >= 0; x--)
	for(int y = 0; y < height; y++)
	{
		int i = x + y * width;
		if((i % width) + 1 < width) dists[i].choose(dists[i + 1].left()); //right->left
		if(i > width) dists[i].choose(dists[i - width].below()); //above->below
	}
	
	
	//3412 pass
	for(int x = 0; x < width; x++)
	for(int y = height - 1; y >= 0; y--)
	{
		int i = x + y * width;
		if((i % width)) dists[i].choose(dists[i - 1].right()); //left->right
		if((i + width) < size) dists[i].choose(dists[i + width].above()); //below->above
	}
	
}

//set all points to true that would be in circle of radius r from a point that was originally true
void expand_object(array_2d<unsigned char >& obj, double r)
{
	const unsigned int width(obj.width);
	const unsigned int height(obj.height);
	const unsigned int size(obj.size);
	
	array_2d<edist > dists(width, height);
	edist max_dist = edist(-(width + height), -(width + height));
	edist zero = edist(0,0);
	
	for(unsigned int i = 0; i < size; i++)
	{
		dists[i] = (obj[i] ? zero : max_dist);
	}
		
	expand_dist_map(dists);
	
	double r_squared = r * r;
	
	for(unsigned int i = 0; i < size; i++)
	{
		obj[i] = dists[i] <= r_squared;
	}

}
