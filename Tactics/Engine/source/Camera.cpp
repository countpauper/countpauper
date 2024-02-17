#include <GL/gl.h>
#include <GL/glut.h>

#include "Engine/Camera.h"
#include "Engine/Geometry.h"
#include "Engine/Coordinate.h"
#include "Engine/Quaternion.h"
#include "Engine/Vector.h"
#include "Engine/Matrix.h"
#include <iostream>
#include <cmath>
#include <cassert>

namespace Engine
{
    Camera::Camera() :
        position(Coordinate::origin),
        target(0, 0, 0),
        rotation(-45 , 0, 0),
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

    bool Camera::Key(std::uint16_t key, unsigned modifiers)
    {
        if (modifiers & GLUT_ACTIVE_SHIFT)
        {
            if (key == GLUT_KEY_UP)
            {
                Move(vertical);
                return true;
            }
            else if (key == GLUT_KEY_DOWN)
            {
                Move(-vertical);
                return true;
            }
            else if (key == GLUT_KEY_LEFT)
            {
                Move(Engine::Vector(-1, 0, 0));
                return true;
            }
            else if (key == GLUT_KEY_RIGHT)
            {
                Move(Engine::Vector(1, 0, 0));
                return true;
            }
            else if (key == int('+')<<8)
            {
                zoom *= 1.1f;
                return true;
            }
            else if (key == int('-')<<8) // smiley
            {
                zoom/= 1.1f;
                return true;
            }
        }
        else
        {
            if (key == GLUT_KEY_UP)
            {
                rotation.x += 5;
                return true;
            }
            else if (key == GLUT_KEY_DOWN)
            {
                rotation.x -= 5;
                return true;
            }
            else if (key == GLUT_KEY_LEFT)
            {
                rotation -= (vertical * 5.0);
                return true;
            }
            else if (key == GLUT_KEY_RIGHT)
            {
                rotation += (vertical * 5.0);
                return true;
            }
        }
        return false;
    }

    PerspectiveCamera::PerspectiveCamera() :
        fov(90)    // makes scale = 1
    {
    }

    void PerspectiveCamera::Render() const
    {
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        double scale = zoom * double(1.0 / tan(fov* 0.5f * PI / 180.0f));
        double n = 10.0;  // 1.0 wastes a lot of depth buffer
        double f = 100.0; // pretty much how much of the map is visible in grids
        // default glClearDepth(1);
        // default glDepthRange(0, 1);
        // default glDepthFunc(GL_LESS);

        // https://www.scratchapixel.com/lessons/3d-basic-rendering/perspective-and-orthographic-projection-matrix/building-basic-perspective-projection-matrix

        Matrix m = Matrix::Perspective(n, f) * Matrix::Scale(Vector(scale, scale, 1));
        m.Render();
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
        glMatrixMode(GL_MODELVIEW);
    }

    TopCamera::TopCamera()
    {
        zoom = 0.1f;
    }



    void TopCamera::Render() const
    {
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        glScaled(zoom, zoom, zoom);
        //glTranslatef(dragx - position.x, -position.y, dragz - position.z);
        glTranslated(dragx - target.x, -target.y, dragz - 1.0f);
        glDepthRange(0, 1);
        glClearDepth(1);
        glDepthFunc(GL_LESS);
        glRotated(-90, 1.0, 0.0, 0.0);
        glMatrixMode(GL_MODELVIEW);
    }

}
