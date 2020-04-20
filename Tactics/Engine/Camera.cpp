#include "stdafx.h"
#include <gl/GL.h>
#include "Camera.h"
#include "Geometry.h"
#include <iostream>

namespace Engine
{
    Camera::Camera() :
        position(0, 0, 0),
        target(0, 0, 0),
        rotation(-30 ,180, 0),
        zoom(1.0f),
        dragx(0),
        dragz(0)
    {
    }

    void Camera::Move(const Coordinate& newPosition)
    {
        position = newPosition;
    }
    void Camera::Move(const Vector& offset)
    {
        position += offset;
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

    void Camera::Key(WPARAM key)
    {
        auto shift = GetKeyState(VK_SHIFT)&0x8000;
        if (shift == 0)
        {
            if (key == VK_UP)
            {
                Move(Engine::Vector(0, 1, 0));
            }
            else if (key == VK_DOWN)
            {
                Move(Engine::Vector(0, -1, 0));
            }
            else if (key == VK_LEFT)
            {
                Move(Engine::Vector(-1, 0, 0));
            }
            else if (key == VK_RIGHT)
            {
                Move(Engine::Vector(1, 0, 0));
            }
        }
        else
        {
            if (key == VK_UP)
            {
                rotation.x += 10;
            }
            else if (key == VK_DOWN)
            {
                rotation.x -= 10;
            }
            else if (key == VK_LEFT)
            {
                rotation.y -= 10;
            }
            else if (key == VK_RIGHT)
            {
                rotation.y += 10;
            }
        }
    }
    PerspectiveCamera::PerspectiveCamera() :
        fov(90)    // makes scale = 1
    {
    }


    void PerspectiveCamera::Render() const
    {
/*        glFrustum(0, 10 , 0, 10, 3, 10.0 );
        glDepthRange(0, 2);
        glTranslatef(dragx - position.x, -position.y, dragz - position.z);
        glRotated(rotation.y, 0, 1, 0);
        glRotated(rotation.z, 0, 0, 1);
        glRotated(rotation.x, 1, 0, 0);
        return;
*/

        float scale = float(1.0 / tan(fov* 0.5f * PI / 180.0f));
        //float n = 1.0;    
        //float f = 1.0+scale;
        float n = 1.0;
        float f = 10.0;

        glDepthRange(n, f);
        glClearDepth(f);
        if (n < f)
            glDepthFunc(GL_LESS);
        else
            glDepthFunc(GL_GREATER);
        glDepthRange(0, 1);
        glDepthFunc(GL_LESS);

        // https://www.scratchapixel.com/lessons/3d-basic-rendering/perspective-and-orthographic-projection-matrix/building-basic-perspective-projection-matrix

        GLfloat perspectiveMatrix[16] =
        {
            scale, 0,   0,              0,
            0, scale,   0,              0,
            0, 0,       -f / (f - n),   -1,
            0, 0,       -f*n / (f - n),  0
        };
        glMultMatrixf(perspectiveMatrix);
        glTranslatef(dragx-position.x, -position.y, dragz-position.z);
        glRotated(rotation.y, 0, 1, 0);
        glRotated(rotation.z, 0, 0, 1);
        glRotated(rotation.x, 1, 0, 0);
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
        glDepthRange(0, 1);
        glClearDepth(1);
        glDepthFunc(GL_LESS);
        glRotated(-90, 1.0, 0.0, 0.0);
    }

}
