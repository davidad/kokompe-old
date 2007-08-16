#ifndef GUI
#define GUI

#include <windows.h>
#include <gl/gl.h>
#include "trimesh.h"

int show_graphics_window(HINSTANCE hInstance, trimesh_t trimesh);
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
void EnableOpenGL(HWND hWnd, HDC * hDC, HGLRC * hRC);
void DisableOpenGL(HWND hWnd, HDC hDC, HGLRC hRC);

static void
set_view (double alt, double azi, double meters_per_pixel, double ox, double oy);

void draw_block(double x, double y, double z);


#endif