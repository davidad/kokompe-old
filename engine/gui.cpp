//
// GLSAMPLE.CPP
//  by Blaine Hodge
//

// Includes

#include <windows.h>
#include <gl/gl.h>
#include <gl/glu.h>
#include "gui.h"

int show_graphics_window(HINSTANCE hInstance, trimesh_t trimesh) {


	WNDCLASS wc;
	HWND hWnd;
	HDC hDC;
	HGLRC hRC;
	MSG msg;
	BOOL quit = FALSE;
	float theta = 0.0f;
	float xpos = 0.0f;
	GLuint dl;

	// register window class
	wc.style = CS_OWNDC;
	wc.lpfnWndProc = WndProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = hInstance;
	wc.hIcon = LoadIcon( NULL, IDI_APPLICATION );
	wc.hCursor = LoadCursor( NULL, IDC_ARROW );
	wc.hbrBackground = (HBRUSH)GetStockObject( BLACK_BRUSH );
	wc.lpszMenuName = NULL;
	wc.lpszClassName = TEXT("GLSample");
	RegisterClass( &wc );
	
	// create main window
	hWnd = CreateWindow( 
		TEXT("GLSample"), TEXT("OpenGL Sample"), 
		WS_CAPTION | WS_POPUPWINDOW | WS_VISIBLE,
		0, 0, 1024, 1024,
		NULL, NULL, hInstance, NULL );
	
	// enable OpenGL for the window
	EnableOpenGL( hWnd, &hDC, &hRC );
	


	// Configure OpenGL
    glEnable (GL_BLEND);
    glEnable (GL_DEPTH_TEST);
    glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glShadeModel (GL_SMOOTH);




	// Set the initial view
	double alt = 30;
	double azi = 30;
	double meters_per_pixel = 0.002;
	double ox = 0;
	double oy = 0;
    int first = 0;

	// program main loop
	while ( !quit )
	{
		
		// check for messages
		if ( PeekMessage( &msg, NULL, 0, 0, PM_REMOVE )  )
		{
			
			// handle or dispatch messages
			if ( msg.message == WM_QUIT ) 
			{
				quit = TRUE;
			} 
			else 
			{
				TranslateMessage( &msg );
				DispatchMessage( &msg );
			}
			
		} 
		else 
		{
			
			// OpenGL animation code goes here
		 azi +=1;
    
    // Clear the viewport
    glMatrixMode( GL_MODELVIEW );
    glClearColor (0.7, 0.7, 0.7, 1.0);
    glClearDepth(1.0);
    glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Draw a cube
    //draw_block(1.0,1.0,1.0);
	//if (first == 0) {
	//cout << "Creating display list.\n";
	//dl = glGenLists(1);
	//glNewList(dl, GL_COMPILE);
	//glEnable (0x809D);  arb_multisample appears to make no difference
	trimesh.drawgl();
	//glEndList();
	//cout << "Starting to draw.\n";
	//first = 1;
	//}
	



	//glCallList(dl);
	

	//trimesh.drawgl();

    set_view (alt, azi, meters_per_pixel, ox, oy);
			SwapBuffers( hDC );
		}
		
	}
	
	// shutdown OpenGL
	DisableOpenGL( hWnd, hDC, hRC );
	
	// destroy the window explicitly
	DestroyWindow( hWnd );
	
	return msg.wParam;
	
}

// Window Procedure

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	
	switch (message)
	{
		
	case WM_CREATE:
		return 0;
		
	case WM_CLOSE:
		PostQuitMessage( 0 );
		return 0;
		
	case WM_DESTROY:
		return 0;
		
	case WM_MOUSEMOVE:
		


		return 0;

	case WM_KEYDOWN:
		switch ( wParam )
		{
			
		case VK_ESCAPE:
			PostQuitMessage(0);
			return 0;
			
		}
		return 0;
	
	default:
		return DefWindowProc( hWnd, message, wParam, lParam );
			
	}
	
}

// Enable OpenGL

void EnableOpenGL(HWND hWnd, HDC * hDC, HGLRC * hRC)
{
	PIXELFORMATDESCRIPTOR pfd;
	int format;
	
	// get the device context (DC)
	*hDC = GetDC( hWnd );
	
	// set the pixel format for the DC
	ZeroMemory( &pfd, sizeof( pfd ) );
	pfd.nSize = sizeof( pfd );
	pfd.nVersion = 1;
	pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
	pfd.iPixelType = PFD_TYPE_RGBA;
	pfd.cColorBits = 24;
	pfd.cDepthBits = 16;
	pfd.iLayerType = PFD_MAIN_PLANE;
	format = ChoosePixelFormat( *hDC, &pfd );
	SetPixelFormat( *hDC, format, &pfd );
	
	// create and enable the render context (RC)
	*hRC = wglCreateContext( *hDC );
	wglMakeCurrent( *hDC, *hRC );
	
}

// Disable OpenGL

void DisableOpenGL(HWND hWnd, HDC hDC, HGLRC hRC)
{
	wglMakeCurrent( NULL, NULL );
	wglDeleteContext( hRC );
	ReleaseDC( hWnd, hDC );
}


static void
set_view (double alt, double azi, double meters_per_pixel, double ox, double oy)
{
  
    int t1;
    unsigned int t, win_w, win_h, win_max;
    double eye_dist;

	win_w = 1024;  // currently hardcoded
	win_h = 1024;
    double draw_yaw = 0;

    glViewport (0, 0, win_w, win_h);

    glMatrixMode (GL_PROJECTION);
    glLoadIdentity ();

	if (win_w > win_h)
		win_max = win_w;
	else
		win_max = win_h;


    eye_dist = win_max * meters_per_pixel;
    glFrustum (win_w * -meters_per_pixel / 2.0 / 100,
            win_w * meters_per_pixel / 2.0 / 100,
            win_h * -meters_per_pixel / 2.0 / 100,
            win_h * meters_per_pixel / 2.0 / 100,
            eye_dist / 100, eye_dist * 100);

    glMatrixMode (GL_MODELVIEW);
    glLoadIdentity ();

	// Set up lighting here so that the object appears to rotate,
	// rather than the viewer's viewpoint
	GLfloat light_position[] = { 10.0, 10.0, 10.0, 1.0 };
	GLfloat lm_ambient[] = { 0.5, 0.5, 0.5, 1.0 };
	glLightfv(GL_LIGHT0, GL_POSITION, light_position);
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, lm_ambient);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glEnable(GL_DEPTH_TEST);
	glShadeModel (GL_SMOOTH);


    glTranslated (0, 0, -eye_dist);
    glRotated (alt, -1.0, 0.0, 0.0);
    glRotated (azi + draw_yaw, 0.0, 0.0, 1.0);
    glTranslated (-ox, -oy, 0);
}






void draw_block(double x, double y, double z) {
  x /= 2.0;
  y /= 2.0;
  z /= 2.0;

glBegin(GL_QUADS);                              // Draw a cube with quads
                glColor3f(0.0f,1.0f,0.0f);                                        // Color Blue
                glVertex3f( x, y,-z);                                   // Top Right Of The Quad (Top)
                glVertex3f(-x, y,-z);                                   // Top Left Of The Quad (Top)
                glVertex3f(-x, y, z);                                   // Bottom Left Of The Quad (Top)
                glVertex3f( x, y, z);                                   // Bottom Right Of The Quad (Top)
                glColor3f(1.0f,0.5f,0.0f);                                        // Color Orange
                glVertex3f( x,-y, z);                                   // Top Right Of The Quad (Bottom)
                glVertex3f(-x,-y, z);                                   // Top Left Of The Quad (Bottom)
                glVertex3f(-x,-y,-z);                                   // Bottom Left Of The Quad (Bottom)
                glVertex3f( x,-y,-z);                                   // Bottom Right Of The Quad (Bottom)
                glColor3f(1.0f,0.0f,0.0f);                                        // Color Red
                glVertex3f( x, y, z);                                   // Top Right Of The Quad (Front)
                glVertex3f(-x, y, z);                                   // Top Left Of The Quad (Front)
                glVertex3f(-x,-y, z);                                   // Bottom Left Of The Quad (Front)
                glVertex3f( x,-y, z);                                   // Bottom Right Of The Quad (Front)
                glColor3f(1.0f,1.0f,0.0f);                                        // Color Yellow
                glVertex3f( x,-y,-z);                                   // Top Right Of The Quad (Back)
                glVertex3f(-x,-y,-z);                                   // Top Left Of The Quad (Back)
                glVertex3f(-x, y,-z);                                   // Bottom Left Of The Quad (Back)
                glVertex3f( x, y,-z);                                   // Bottom Right Of The Quad (Back)
                glColor3f(0.0f,0.0f,1.0f);                                        // Color Blue
                glVertex3f(-x, y, z);                                   // Top Right Of The Quad (Left)
                glVertex3f(-x,y,-z);                                    // Top Left Of The Quad (Left)
                glVertex3f(-x,-y,-z);                                   // Bottom Left Of The Quad (Left)
                glVertex3f(-x,-y, z);                                   // Bottom Right Of The Quad (Left)
                glColor3f(1.0f,0.0f,1.0f);     
// Color Violet
                glVertex3f( x,y,-z);                                    // Top Right Of The Quad (Right)
                glVertex3f( x,y, z);                                    // Top Left Of The Quad (Right)
                glVertex3f( x,-y, z);                                   // Bottom Left Of The Quad (Right)
                glVertex3f( x,-y,-z);                                   // Bottom Right Of The Quad (Right)
        glEnd();
}



