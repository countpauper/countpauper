#include <GL/gl.h>
#include <GL/glut.h>

#include "UI/Camera.h"
#include "Geometry/Angles.h"
#include "Geometry/Coordinate.h"
#include "Geometry/Quaternion.h"
#include "Geometry/Vector.h"
#include "Geometry/Matrix.h"
#include "UI/Debug.h"
#include <iostream>
#include <cmath>
#include <cassert>

namespace Engine
{
    Camera::Camera() =default;

    void Camera::Move(const Coordinate& newPosition)
    {
        position = newPosition;
    }
    void Camera::Move(const Vector& offset)
    {
        position += offset;
    }

    void Camera::Zoom(double factor)
    {
        zoom *= factor;
    }

    void Camera::Face(Coordinate target)
    {
        auto vector = target - position;
        if (vector)
        {
            auto [yaw, pitch] = FaceYawPitch(vector.Normal(), vertical.z == 1);
            rotation.x = pitch;
            if (vertical.z)
            {
                rotation.z = yaw;
            }
            else
            {
                assert(!vertical.x);
                rotation.y = yaw;
            }
            Debug::Log<true>("Face (%.3f, %.3f, %.3f) yaw=%.1f, pitch=%.1f, distance %.3f",
                vector.x, vector.y, vector.z,
                Rad2Deg(yaw), Rad2Deg(pitch), vector.Length());
        }
    }

    PerspectiveCamera::PerspectiveCamera() :
        fov(90)    // makes scale = 1
    {
    }

    void PerspectiveCamera::Render() const
    {
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        double scale = zoom * double(1.0 / tan(Deg2Rad(fov* 0.5f)));
        double n = 10.0;  // 1.0 wastes a lot of depth buffer
        double f = 100.0; // pretty much how much of the map is visible in grids
        // default glClearDepth(1);
        // default glDepthRange(0, 1);
        // default glDepthFunc(GL_LESS);

        // https://www.scratchapixel.com/lessons/3d-basic-rendering/perspective-and-orthographic-projection-matrix/building-basic-perspective-projection-matrix

        Matrix m = Matrix::Perspective(n, f) * Matrix::Scale(Vector(scale, scale, 1));
        m.Render();

        // Not sure why but everything (order, direction=sign) needs to be reversed from the position
        // if the camera man. I suppose because of the camera moves left, the world seems to move right?
        glRotated(Rad2Deg(rotation.x), 1, 0, 0);
        glRotated(Rad2Deg(rotation.y), 0, 1, 0);
        glRotated(Rad2Deg(rotation.z), 0, 0, 1);
        glTranslated(-position.x, -position.y, -position.z);

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
        glTranslated(-position.x, -position.y, - 1.0f);
        glDepthRange(0, 1);
        glClearDepth(1);
        glDepthFunc(GL_LESS);
        glRotated(-90, 1.0, 0.0, 0.0);
        glMatrixMode(GL_MODELVIEW);
    }

    bool FreeCamera::Key(std::uint16_t key, unsigned modifiers)
    {
        static constexpr double rotationSpeed = 0.05;
        if (modifiers & GLUT_ACTIVE_SHIFT)
        {
            if (key == GLUT_KEY_UP)
            {
                rotation.x -= rotationSpeed;
                return true;
            }
            else if (key == GLUT_KEY_DOWN)
            {
                rotation.x += rotationSpeed;
                return true;
            }
            else if (key == GLUT_KEY_LEFT)
            {
                rotation -= (vertical * rotationSpeed);
                return true;
            }
            else if (key == GLUT_KEY_RIGHT)
            {
                rotation += (vertical * rotationSpeed);
                return true;
            }
        }
        else
        {
            if (key == GLUT_KEY_UP)
            {
                Vector dir = Vector(sin(rotation.z), cos(rotation.z), 0);
                Move(dir);
                return true;
            }
            else if (key == GLUT_KEY_DOWN)
            {
                Vector dir = -Vector(sin(-rotation.z), cos(-rotation.z), 0);
                Move(dir);
                return true;
            }
            else if (key == GLUT_KEY_RIGHT)
            {
                Vector dir = Vector(cos(-rotation.z), sin(-rotation.z), 0);
                Move(dir);
                return true;
            }
            else if (key == GLUT_KEY_LEFT)
            {
                Vector dir = -Vector(cos(-rotation.z), sin(-rotation.z), 0);
                Move(dir);
                return true;
            }
            else if (key == GLUT_KEY_PAGE_UP)
            {
                Move(vertical);
                return true;
            }
            else if (key == GLUT_KEY_PAGE_DOWN)
            {
                Move(-vertical);
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
        return false;
    }

    void TrackingCamera::Track(const Coordinate& newTarget)
    {
        target = newTarget;
        Face(target);
    }

    bool TrackingCamera::Key(std::uint16_t key, unsigned modifiers)
    {

        if (modifiers!=0)
            return false;

        if (key == GLUT_KEY_UP)
        {
            Vector dir = Vector(sin(rotation.z), cos(rotation.z), 0);
            Move(dir);
            Face(target);
            return true;
        }
        else if (key == GLUT_KEY_DOWN)
        {
            Vector dir = -Vector(sin(-rotation.z), cos(-rotation.z), 0);
            Move(dir);
            Face(target);
            return true;
        }
        else if (key == GLUT_KEY_RIGHT)
        {
            Vector dir = Vector(cos(-rotation.z), sin(-rotation.z), 0);
            Move(dir);
            Face(target);
            return true;
        }
        else if (key == GLUT_KEY_LEFT)
        {
            Vector dir = -Vector(cos(-rotation.z), sin(-rotation.z), 0);
            Move(dir);
            Face(target);
            return true;
        }
        else if (key == GLUT_KEY_PAGE_UP)
        {
            Move(vertical);
            Face(target);
            return true;
        }
        else if (key == GLUT_KEY_PAGE_DOWN)
        {
            Move(-vertical);
            Face(target);
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

        return false;
    }


}
