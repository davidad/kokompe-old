// Arcball camera by Eugene Hsu
// Based on 6.839 sample code for rotation code.
// Extended to handle translation (MIDDLE) and scale (RIGHT)

// -*-c++-*-
#pragma once
#include "../math/math.h"


class Camera
{
public:

    Camera();
    
    typedef enum { NONE, LEFT, MIDDLE, RIGHT } Button;

    // You must call all of the Set*() functions before you use this!
    // I didn't put it into the constructor because it's inconvenient
    // to initialize stuff in my opengl application.
    
    void SetDimensions(int w, int h);
    void SetViewport(int x, int y, int w, int h);
    void SetPerspective(double fovy);

    // Call from whatever UI toolkit
    void MouseClick(Button button, int x, int y);
    void MouseDrag(int x, int y);
    void MouseRelease(int x, int y);

    // Apply viewport, perspective, and modeling
    // use these instead of 
    void ApplyViewport() const;
    void ApplyPerspective() const;
    void ApplyModelview() const;

    // Set for relevant vars
    void SetCenter(const vector3 &center);
    void SetRotation(const matrix4 &rotation);
    void SetDistance(const double distance);

    // Get for relevant vars
    vector3 GetCenter() const { return mCurrentCenter; }
    matrix4 GetRotation() const { return mCurrentRot; }
    double GetDistance() const { return mCurrentDistance; }
    
private:

    // States 
    int     mDimensions[2];
    int     mStartClick[2];
    Button  mButtonState;

    // For rotation
    matrix4   mStartRot;
    matrix4   mCurrentRot;

    // For translation
    double  mPerspective[2];
    int     mViewport[4];
    vector3   mStartCenter;
    vector3   mCurrentCenter;

    // For zoom
    double  mStartDistance;
    double  mCurrentDistance;

    void ArcBallRotation(int x, int y);
    void PlaneTranslation(int x, int y);
    void DistanceZoom(int x, int y);
};
