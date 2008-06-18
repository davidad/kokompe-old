#include "application.h"
#include <GL/glut.h>
#include <iostream>



application::application(int argc, char** argv)
{
	time_to_shutdown = false;
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
    glutInitWindowSize(512, 512);
}

application::~application()
{
	std::cout << "Destrying application" << std::endl;
	for(std::set<view*>::iterator i = open_views.begin(); i != open_views.end(); i++) {
		(*i)->close();
		delete *i;
	}

	for(std::set<design*>::iterator i = open_designs.begin(); i != open_designs.end(); i++) {
		delete *i;
	}
}

void application::initiate_shutdown()
{
	time_to_shutdown = true;
}

void application::run()
{
	//load a default view (possibly reload old open views in the future)
	new_view();

	//start the main event loop
	//if using freeglut:	
	//while(!time_to_shutdown)
	//{
	//	glutMainLoopEvent();
	//}
	glutMainLoop();

}


design* application::new_design()
{
	design* new_d = new design();
	open_designs.insert(new_d);
	return new_d;
}

void application::new_view()
{	
	design* new_d = new_design();
	new_view(*new_d);
}

void application::new_view(design& d)
{
	view* new_v = new view(d, *this);
	open_views.insert(new_v);
}

