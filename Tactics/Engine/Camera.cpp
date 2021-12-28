#include "stdafx.h"
#include <gl/GL.h>
#include "Camera.h"
#include "Geometry.h"
#include "Coordinate.h"
#include "Quaternion.h"
#include "Vector.h"
#include <iostream>
#include "Matrix.h"

namespace Engine
{
    Camera::Camera() :
        position(Coordinate::origin),
        target(0, 0, 0),
        rotation(0 ,180, 0),
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
    void Camera::Zoom(double factor)
    {
        zoom *= factor;
    }
    void Camera::Drag(double dx, double dz)
    {
        dragx = dx;
        dragz = dz;
    }
    void Camera::FinishDrag(double dx, double dz)
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
            else if (key == VK_ADD)
            {
                zoom *= 1.1f;
            }
            else if (key == VK_SUBTRACT)
            {
                zoom/= 1.1f;
            }
        }
        else
        {
            if (key == VK_UP)
            {
                rotation.x += 5;
            }
            else if (key == VK_DOWN)
            {
                rotation.x -= 5;
            }
            else if (key == VK_LEFT)
            {
                rotation.y -= 5;
            }
            else if (key == VK_RIGHT)
            {
                rotation.y += 5;
            }
        }
    }
    PerspectiveCamera::PerspectiveCamera() :
        fov(90)    // makes scale = 1
    {
    }


    void PerspectiveCamera::Render() const
    {
        GLdouble equation[4];
        glGetClipPlane(GL_CLIP_PLANE1, equation);
        double scale = zoom * double(1.0 / tan(fov* 0.5f * PI / 180.0f));
        double n = 10.0;  // 1.0 wastes a lot of depth buffer
        double f = 100.0; // pretty much how much of the map is visible in grids
        // default glClearDepth(1);
        // default glDepthRange(0, 1);
        // default glDepthFunc(GL_LESS);

        // https://www.scratchapixel.com/lessons/3d-basic-rendering/perspective-and-orthographic-projection-matrix/building-basic-perspective-projection-matrix
        
        Matrix m = Matrix::Perspective(n, f) * Matrix::Scale(Vector(scale, scale, 1));
        glMultMatrixd(&m.data());
/** /
        // ortho projection
        glScaled(0.1*zoom, 0.1*zoom, 0.01*zoom);
/**/
        glTranslated(dragx-position.x, -position.y, dragz-position.z-n);
        glRotated(rotation.x, 1, 0, 0);
        glRotated(rotation.y, 0, 1, 0);
        glRotated(rotation.z, 0, 0, 1);
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
        glScaled(zoom, zoom, zoom);
        //glTranslatef(dragx - position.x, -position.y, dragz - position.z);
        glTranslated(dragx - target.x, -target.y, dragz - 1.0f);
        glDepthRange(0, 1);
        glClearDepth(1);
        glDepthFunc(GL_LESS);
        glRotated(-90, 1.0, 0.0, 0.0);
    }

}
