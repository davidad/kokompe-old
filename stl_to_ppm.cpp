//include files for utility
#include <iostream>
#include <fstream>
#include "argtable2.h"
#include "vvolume.h"

//parameters that should be set before calling the core function
std::ostream* out(&std::cout);
std::istream* in(&std::cin);


vector3 new_ambient_light_color = vector3(1,1,1) * .2;
vector3 new_light_direction = -vector3(1,1,0);
vector3 new_light_source_color = vector3(1,1,1) * 2;
matrix4 new_projection_matrix;

unsigned int size = 1000;
unsigned int new_width = size;
unsigned int new_height = size;

//core function of utility
int stl_to_ppm();

//basic helper functions
int open_input_file(const char* filename);
int open_output_file(const char* filename);

void cleanup();

int open_input_file(const char* filename)
{
	static std::fstream in_file (filename, std::ios::binary | std::ios::in);
	if (in_file.fail()) {
		std::cerr << "Error opening input file.\n";
		return -1;
	}
	
	in = &in_file;
}

int open_output_file(const char* filename)
{
	static std::fstream out_file (filename, std::ios::binary | std::ios::out);
	if (out_file.fail()) {
		std::cerr << "Error opening output file.\n";
		return -1;
	}
	
	out = &out_file;
}

int main(int argc, char** argv)
{
	int error_code = 0;
	atexit(&cleanup);
    struct arg_dbl* roll = arg_dbl0("r", "roll", "<angle>","camera view roll");
    struct arg_dbl* pitch = arg_dbl0("p", "pitch", "<angle>","camera view pitch");
    struct arg_dbl* yaw = arg_dbl0("y", "yaw", "<angle>","camera view yaw");
    struct arg_dbl* org_x = arg_dbl0("x", "origin-x", "<coord>", "origin x");
    struct arg_dbl* org_y = arg_dbl0("y", "origin-y", "<coord>", "origin y");
    struct arg_dbl* org_z = arg_dbl0("z", "origin-z", "<coord>", "origin z");
    struct arg_int* width = arg_int0("w", "width", "<pixels>", "output image width");
    struct arg_int* height = arg_int0("w", "height", "<pixels>", "output image height");
    struct arg_dbl* scale = arg_dbl0("s", "scale", "<scalar>", "scale the object relative to the image frame");
    struct arg_end* end = arg_end(20);
    void *argtable[] = {roll,pitch,yaw,org_x,org_y,org_z,width,height,scale,end};
    roll->dval[0] = -.5;
    pitch->dval[0] = 2.5;
    yaw->dval[0] = -1.5;
    org_x->dval[0] = 10;
    org_y->dval[0] = 10;
    org_z->dval[0] = 10;
    width->ival[0] = 1000;
    height->ival[0] = 1000;
    scale->dval[0] = 500;
    int nerrors = arg_parse(argc, argv, argtable);
    if(nerrors==0) {
        
        /*
        error_code = open_input_file("in.stl");
        if(error_code) exit(error_code);

        error_code = open_output_file("out.ppm");
        if(error_code) exit(error_code);
        */
        vector3 center(width->ival[0] / 2.0, height->ival[0] / 2.0, 0);
        
        matrix4 move_to_origin = matrix4(-vector3(org_x->dval[0],org_y->dval[0],org_z->dval[0]));
        matrix4 move_to_center = matrix4(center);
        matrix4 rotate = matrix4(quaternion::from_roll_pitch_yaw(vector3(roll->dval[0], pitch->dval[0], yaw->dval[0])));
        matrix4 scale_matrix = matrix4::identity();
        scale_matrix *= scale->dval[0];
        scale_matrix(3,3) = 1; //fix homogeneous component
        
        //new_projection_matrix = move_to_center * rotate * scale_matrix * move_to_origin;
        new_projection_matrix =  move_to_center * rotate * scale_matrix * move_to_origin;
        
        error_code = stl_to_ppm();
        if(error_code) exit(error_code);
    } else {
        arg_print_errors(stderr,end,NULL);
    }

	return 0;
}



int stl_to_ppm()
{
// Binary STL file format is described in
// Automated Fabrication, by Marshall Burns, Ch.6
// and at http://www.ennex.com/~fabbers/StL.asp

	
	vvolume view(new_width, new_height, new_projection_matrix, new_ambient_light_color,
			new_light_direction, new_light_source_color);
	
	char header[80];
    char dummy[1024];

	(*in).read(header, 80);
	unsigned int num_triangle;
	(*in).read((char*)&num_triangle, 4);
	unsigned short attr_byte_count;
	for(unsigned int i = 0; i < num_triangle; i++)
	{
		struct triangle
		{
			_vector3<float> normal;
			_vector3<float> vertex0;
			_vector3<float> vertex1;
			_vector3<float> vertex2;
		};
		triangle current_triangle;
		
		(*in).read((char*)&current_triangle, sizeof(triangle));
		(*in).read((char*)&attr_byte_count, sizeof(unsigned short));	
		if(attr_byte_count != 0)
		{
			(*in).ignore(attr_byte_count);	
			std::cerr << "STL metadata was ignored. Bad things might happen." << std::endl;
		}
		
		vector3 v[3];
		for(int i = 0; i < 3; i++)
		{
			for(int j = 0; j < 3; j++)
			{
				v[i][j] = (&current_triangle.vertex0)[i][j];
			}
		}
		vector3 normal;
		normal[0] = current_triangle.normal[0];
		normal[1] = current_triangle.normal[1];
		normal[2] = current_triangle.normal[2];
		
		view.draw_triangle(v, normal, vector3(1,1,1) * .6);
		
	}
		
	(*out) << view;
	return 0;
}


void cleanup()
{
}
