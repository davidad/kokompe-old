#include "kokompe.h"

#include <iostream>
#include <GL/glut.h>

#include "color.h"
#include "commands.h"

static int mouse_button;
static int mouse_state;

void _reshape(int new_width, int new_height);
void _idle();
void _render();
void _keyboard (unsigned char key, int x, int y);
void _special (int key, int x, int y);
void _mouse(int button, int state, int x, int y);
void _mouse_motion(int x, int y);
	
kokompe* active_kokompe;
				

void kokompe::render_math_string(std::string instring)
{
	eval_geometry(instring, trimesh);
}

//core functions
void kokompe::run()
{
	glutMainLoop ();
}

void kokompe::init(int argc, char** argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode (GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
	glutCreateWindow ("Kokompe - Viewer (Prototype)");

	glutDisplayFunc(_render);
	glutReshapeFunc(_reshape);

	glutKeyboardFunc (_keyboard);
	glutSpecialFunc(_special);

	glutMouseFunc(_mouse);
	glutIdleFunc (_idle);
	glutMotionFunc(_mouse_motion);

    camera.SetPerspective(50.0);

	camera.SetCenter(vector3(0,0,0));
	camera.SetRotation(matrix4::identity());
    camera.SetDistance(2.0);

	background_color = steel_blue;
	active_kokompe = this;

	init_py_commands();

}

void kokompe::render()
{
	double time = double(glutGet(GLUT_ELAPSED_TIME)) / 1000.0;
	static unsigned int elapsed_frames = 0;
	elapsed_frames++;

	glClearColor(background_color[0], background_color[1], background_color[2], 1.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);	// Clear Screen And Depth Buffer
    glEnable(GL_DEPTH_TEST);   // Depth testing must be turned on
    glEnable(GL_LIGHTING);     // Enable lighting calculations
    glEnable(GL_LIGHT0);       // Turn on light #0.

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	camera.ApplyPerspective();


	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();


    // Set light properties
    GLfloat Lt0diff[] = {1.0,1.0,1.0,1.0};
    GLfloat Lt0pos[] = {1.0f,1.0f,5.0f,1.0};
    glLightfv(GL_LIGHT0, GL_DIFFUSE, Lt0diff);
    glLightfv(GL_LIGHT0, GL_POSITION, Lt0pos);


	camera.ApplyModelview();
	
    // Set material properties of object
    // These colors are RGBA.  Keep the last value at 1.0.
	GLfloat diffColor[4] = {.7,.7,.7,1.0};
    GLfloat specColor[] = {1.0, 1.0, 1.0, 1.0};
    GLfloat shininess[] = {100.0};

    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, diffColor);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, specColor);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, shininess);
  
	trimesh.drawgl();

	console.render();

	glutSwapBuffers();
}

void kokompe::handle_key(char key)
{
	process_key(key);
}

void kokompe::special(int key)
{
	process_special(key);
}

void kokompe::reshape(int new_width, int new_height)
{
	console.set_dimensions(new_width, new_height);
	camera.SetDimensions(new_width, new_height);
	camera.SetViewport(0,0,new_width,new_height);
	camera.ApplyViewport();
	glutPostRedisplay();
}
void kokompe::mouse(int button, int state, int x, int y)
{
	mouse_button = button;
	mouse_state = state;
	if(!state)
	{
		if(button == GLUT_LEFT_BUTTON) camera.MouseClick(camera.LEFT,x,y);	
		else if(button == GLUT_MIDDLE_BUTTON) camera.MouseClick(camera.MIDDLE,x,y);	
		else if(button == GLUT_RIGHT_BUTTON) camera.MouseClick(camera.RIGHT,x,y);	
	}
	else
	{
		camera.MouseRelease(x,y);
	}

	glutPostRedisplay();
}

void kokompe::mouse_motion(int x, int y)
{
	camera.MouseDrag(x, y);
	glutPostRedisplay();
}

void _render()
{
	active_kokompe->render();
}

void _keyboard (unsigned char key, int x, int y)
{
	active_kokompe->handle_key(key);
}

void _special (int key, int x, int y)
{
	active_kokompe->special(key);
}


void _reshape(int new_width, int new_height)
{
	active_kokompe->reshape(new_width, new_height);    
}

void _idle()
{

}

void _mouse(int button, int state, int x, int y)
{
	active_kokompe->mouse(button, state, x, y);
}
void _mouse_motion(int x, int y)
{
	active_kokompe->mouse_motion(x, y);
	glutPostRedisplay();
}
	
int main(int argc, char** argv)
{
	kokompe the_app;
	the_app.init(argc, argv);
	the_app.run();
}
