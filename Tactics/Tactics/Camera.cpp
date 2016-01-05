#include "stdafx.h"
#include <gl/GL.h>
#include "Camera.h"

namespace Engine
{
    Camera::Camera() :
        fov(90),    // makes scale = 1
        position(0,0,0),
        zoom(1.0f),
        dragx(0),
        dragz(0)
    {
    }

    void Camera::Move(const Coordinate& newPosition)
    {
        position = newPosition;
    }
    void Camera::Zoom(float delta)
    {
        zoom = max(0.0f, zoom + delta);
    }
    void Camera::Render() const
    {
        float scale = float(1.0 / tan(fov* 0.5f * M_PI / 180.0f));
        float n = 10;
        float f = 0;
        GLfloat perspectiveMatrix[16] =
        {
            scale, 0, 0, 0,
            0, scale, 0, 0,
            0, 0, f / (f - n), 1,
            0, 0, f*n / (f - n), 0
        };
        glMultMatrixf(perspectiveMatrix);
        glTranslatef(dragx-position.x, -position.y, dragz-position.z);
        // TODO: instead of zoom, have a view angle. a target position, move backwards and forwards
        //glScalef(zoom, zoom, zoom);
    }
    void Camera::Drag(float dx, float dz)
    {
        dragx = dx;
        dragz = dz;
    }
    void Camera::FinishDrag(float dx, float dz)
    {
        position.x -= dx;
        position.z -= dz;
        dragx = 0;
        dragz = 0;
    }

}
