#include <GL/glut.h>
#include <iostream>
using namespace std;

#include "camera.h"

#ifndef M_PI
#define M_PI 3.141592653589793238462643383279502
#endif

Camera::Camera()
{
	matrix4 test;
	mStartRot = mStartRot.identity();
    mCurrentRot = mCurrentRot.identity();
}

void Camera::SetDimensions(int w, int h)
{
    mDimensions[0] = w;
    mDimensions[1] = h;
}

void Camera::SetPerspective(double fovy)
{
    mPerspective[0] = fovy;
}

void Camera::SetViewport(int x, int y, int w, int h)
{
    mViewport[0] = x;
    mViewport[1] = y;
    mViewport[2] = w;
    mViewport[3] = h;
    mPerspective[1] = double(w)/h;
}

void Camera::SetCenter(const vector3 &center)
{
    mStartCenter = mCurrentCenter = center;
}

void Camera::SetRotation(const matrix4 &rotation)
{
    mStartRot = mCurrentRot = rotation;
}

void Camera::SetDistance(const double distance)
{
    mStartDistance = mCurrentDistance = distance;
}

void Camera::MouseClick(Button button, int x, int y)
{
	std::cout << "camera::MouseClick" << std::endl;
    mStartClick[0] = x;
    mStartClick[1] = y;

    mButtonState = button;
    switch (button)
    {
    case LEFT:
        mCurrentRot = mStartRot;
        break;
    case MIDDLE:
        mCurrentCenter = mStartCenter;
        break;
    case RIGHT:
        mCurrentDistance = mStartDistance;
        break;        
    default:
        break;
    }
}

void Camera::MouseDrag(int x, int y)
{
    switch (mButtonState)
    {
    case LEFT:
        ArcBallRotation(x,y);
        break;
    case MIDDLE:
        PlaneTranslation(x,y);
        break;
    case RIGHT:
        DistanceZoom(x,y);
        break;
    default:
        break;
    }
}


void Camera::MouseRelease(int x, int y)
{
    mStartRot = mCurrentRot;
    mStartCenter = mCurrentCenter;
    mStartDistance = mCurrentDistance;
    
    mButtonState = NONE;
}


void Camera::ArcBallRotation(int x, int y)
{
	cout << "Rotating" << endl;
    double sx, sy, sz, ex, ey, ez;
    double scale;
    double sl, el;
    double dotprod;
    
    // find vectors from center of window
    sx = mStartClick[0] - (mDimensions[0]  / 2);
    sy = mStartClick[1] - (mDimensions[1] / 2);
    ex = x -   (mDimensions[0] / 2);
    ey = y -   (mDimensions[1] / 2);
    
    // invert y coordinates (raster versus device coordinates)
    sy = -sy;
    ey = -ey;
    
    // scale by inverse of size of window and magical sqrt2 factor
    if (mDimensions[0] > mDimensions[1]) {
        scale = (double) mDimensions[1];
    } else {
        scale = (double) mDimensions[0];
    }

    scale = 1.0/scale;
    
    sx *= scale;
    sy *= scale;
    ex *= scale;
    ey *= scale;

    // project points to unit circle
    sl = hypot(sx, sy);
    el = hypot(ex, ey);
    
    if (sl > 1.0) {
        sx /= sl;
        sy /= sl;
        sl = 1.0;
    }
    if (el > 1.0) {
        ex /= el;
        ey /= el;
        el = 1.0;
    }
    
    // project up to unit sphere - find Z coordinate
    sz = sqrt(1.0 - sl * sl);
    ez = sqrt(1.0 - el * el);
    
    // rotate (sx,sy,sz) into (ex,ey,ez)
    
    // compute angle from dot-product of unit vectors (and double
    // it).  compute axis from cross product.
    
	vector3 s(sx, sy, sz);
	vector3 e(ex, ey, ez);
	dotprod = s * e;

    if (dotprod != 1)
    {		
		vector3 axis = (s ^ e).unit();
		scalar theta = -2.0 * acos(dotprod);
		quaternion q = quaternion::from_angle_axis(theta, axis);
		mCurrentRot = matrix4(q);
		mCurrentRot = mStartRot * mCurrentRot;
    }
    else
    {
        mCurrentRot = mStartRot;
    }


}

void Camera::PlaneTranslation(int x, int y)
{
    // map window x,y into viewport x,y

    // start
    int sx = mStartClick[0] - mViewport[0];
    int sy = mStartClick[1] - mViewport[1];

    // current
    int cx = x - mViewport[0];
    int cy = y - mViewport[1];


    // compute "distance" of image plane (wrt projection matrix)
    double d = double(mViewport[3])/2.0 / tan(mPerspective[0]*M_PI / 180.0 / 2.0);

    // compute up plane intersect of clickpoint (wrt fovy)
    double su = -sy + mViewport[3]/2.0;
    double cu = -cy + mViewport[3]/2.0;

    // compute right plane intersect of clickpoint (ASSUMED FOVY is 1)
    double sr = (sx - mViewport[2]/2.0);
    double cr = (cx - mViewport[2]/2.0);

    vector2 move(cr-sr, cu-su);

    // this maps move
    move *= -mCurrentDistance/d;

	vector3 u(mCurrentRot(0, 0),mCurrentRot(1, 0),mCurrentRot(2, 0));
	vector3 v(mCurrentRot(0, 1),mCurrentRot(1, 1),mCurrentRot(2, 1));

    mCurrentCenter = mStartCenter + u * move[0] + v * move[1];

}

void Camera::ApplyViewport() const
{
    glViewport(mViewport[0],mViewport[1],mViewport[2],mViewport[3]);
}

void Camera::ApplyPerspective() const
{
    gluPerspective(mPerspective[0], mPerspective[1], 0.1, 1000.0);   
}


void Camera::ApplyModelview() const
{
    // back up distance
    gluLookAt(0,0,mCurrentDistance,
              0,0,0,
              0.0, 1.0, 0.0);

    // rotate object
    glMultMatrixd(mCurrentRot.val_array());

    //translate object to center
    glTranslated(-mCurrentCenter[0],-mCurrentCenter[1],-mCurrentCenter[2]);    
}

void Camera::DistanceZoom(int x, int y)
{
    int sy = mStartClick[1] - mViewport[1];
    int cy = y - mViewport[1];

    double delta = double(cy-sy)/mViewport[3];

    // exponential zoom factor
    mCurrentDistance = mStartDistance * exp(delta);  
}

