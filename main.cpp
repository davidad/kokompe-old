#include <iostream>
#include <fstream>
#include "vvolume.h"


void load_stl(const char* filename)
{
// Binary STL file format is described in
// Automated Fabrication, by Marshall Burns, Ch.6
// and at http://www.ennex.com/~fabbers/StL.asp
	unsigned int percent_loaded = 0;
	unsigned int percent_displayed = 0;
	
	unsigned int size = 1000;
	unsigned int new_width = size;
	unsigned int new_height = size;
	matrix4 new_projection_matrix = matrix4::identity();
	//matrix4::set_to_transform(new_projection_matrix, vector3(500,500,500), 
//		quaternion::from_roll_pitch_yaw(vector3(0.0,3.14,-1.5)));
	matrix4::set_to_transform(new_projection_matrix, vector3(size / 2.0, size / 2.0, size / 2.0), 
		//quaternion(0,0,0,1));
		quaternion::from_roll_pitch_yaw(vector3(-0.5,2.5,-1.5)));
	vector3 new_ambient_light_color = vector3(1,1,1) * .4;
	vector3 new_light_direction = -vector3(0,1,0);
	vector3 new_light_source_color = vector3(1,1,1) * 1;
	

	vvolume view_a(new_width, new_height, new_projection_matrix, new_ambient_light_color,
			new_light_direction, new_light_source_color);
	
	vvolume view_b(new_width, new_height, new_projection_matrix, new_ambient_light_color,
			new_light_direction, new_light_source_color);
		
	std::fstream stl_file (filename, std::ios::binary | std::ios::in);
	char header[80];
    char dummy[1024];
	if (stl_file.fail()) {
		std::cout << "Error opening STL input file.\n";
		return;
	}
	else 
	{
		stl_file.read(header, 80);
		unsigned int num_triangle;
		stl_file.read((char*)&num_triangle, 4);
		unsigned short attr_byte_count;
		for(int i = 0; i < num_triangle; i++)
		{
			struct triangle
			{
				_vector3<float> normal;
				_vector3<float> vertex0;
				_vector3<float> vertex1;
				_vector3<float> vertex2;
			};
			triangle current_triangle;
			
			stl_file.read((char*)&current_triangle, sizeof(triangle));
			stl_file.read((char*)&attr_byte_count, sizeof(unsigned short));	
			if(attr_byte_count != 0)
			{
				stl_file.ignore(attr_byte_count);	
				std::cerr << "STL metadata was ignored. Bad things might happen." << std::endl;
			}
			
			vector3 v[3];
			for(int i = 0; i < 3; i++)
			{
				for(int j = 0; j < 3; j++)
				{
					v[i][j] = (&current_triangle.vertex0)[i][j];
				}
				v[i] = (v[i] - vector3(10,10,10)) * 1100;
			}
			vector3 normal;
			normal[0] = current_triangle.normal[0];
			normal[1] = current_triangle.normal[1];
			normal[2] = current_triangle.normal[2];
			
			
			percent_loaded = int((double(i) / double(num_triangle)) * 100);
			if(percent_loaded > percent_displayed)
			{
				std::cout << percent_loaded << "% done.\n";
				percent_displayed = percent_loaded;
			}
			
			
			view_a.draw_triangle(v, normal, vector3(1,1,1) * .6);
			view_b.draw_triangle(v, vector3(1,1,1) * .6);
		}

	}
		
	stl_file.close();
	
	view_a.write_to_disk("view_a.ppm");
	view_b.write_to_disk("view_b.ppm");
	
}

void simple_test()
{	
	unsigned int new_width = 1000;
	unsigned int new_height = 1000;
	matrix4 new_projection_matrix = matrix4::identity();
	vector3 new_ambient_light_color = vector3(1,1,1) * 0;
	vector3 new_light_direction = vector3(0,1,-1);
	vector3 new_light_source_color = vector3(1,1,1) * 1;
	
	vvolume test(new_width, new_height, new_projection_matrix, new_ambient_light_color,
			new_light_direction, new_light_source_color);
	
	vector3 tri[3];
	
	tri[0] = vector3(500,500,000);
	tri[1] = vector3(500,750,1000);
	tri[2] = vector3(750,500,1000);
	
	test.draw_triangle(tri, vector3(1,0,0));
	
	tri[0] = vector3(750,500,200);
	tri[1] = vector3(500,750,200);
	tri[2] = vector3(750,750,00);
	
	test.draw_triangle(tri, vector3(0,1,0));
	
	test.write_to_disk("test.ppm");
	
}

int main(int argc, char** argv)
{
	load_stl("stlfile2.stl");
	return 0;
}
