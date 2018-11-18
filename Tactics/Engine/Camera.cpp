#include "stdafx.h"
#include <gl/GL.h>
#include "Camera.h"
#include "Geometry.h"

namespace Engine
{
    Camera::Camera() :
        position(0, 0, 0),
        target(0, 0, 0),
        zoom(1.0f),
        dragx(0),
        dragz(0)
    {
    }

    void Camera::Move(const Coordinate& newPosition)
    {
        position = newPosition;
    }
    void Camera::Face(const Coordinate& newTarget)
    {
        target = newTarget;
    }
    void Camera::Zoom(float delta)
    {
        zoom = std::max(0.0f, zoom + delta);
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

    PerspectiveCamera::PerspectiveCamera() :
        fov(90)    // makes scale = 1
    {
    }


    void PerspectiveCamera::Render() const
    {
        float scale = float(1.0 / tan(fov* 0.5f * PI / 180.0f));
        float n = scale;
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
        /* Facing doesn't work right
        auto vector = target - position;
        if (vector)
        {
            double xAngle = atan2(vector.y, vector.z);
            auto yVector = vector;
            yVector.x = 0;
            double yAngle = atan2(vector.x, yVector.Length());
            glRotated(Engine::Rad2Deg(yAngle), 0, 1, 0);
            glRotated(Engine::Rad2Deg(xAngle), 1, 0, 0);
        }

        */
    }

    TopCamera::TopCamera()
    {
        zoom = 0.1f;
    }

    void TopCamera::Render() const
    {
        glScalef(zoom, zoom, zoom);
        //glTranslatef(dragx - position.x, -position.y, dragz - position.z);
        glTranslatef(dragx - target.x, -target.y, dragz - 1.0f);
        glRotated(-90, 1.0, 0.0, 0.0);
    }

}
