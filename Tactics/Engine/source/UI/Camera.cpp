#include <GL/gl.h>
#include <GL/glut.h>

#include "UI/Camera.h"
#include "Geometry/Angles.h"
#include "Geometry/Coordinate.h"
#include "Geometry/Quaternion.h"
#include "Geometry/Vector.h"
#include "Geometry/Matrix.h"
#include "UI/Logging.h"
#include "UI/WindowMessages.h"
#include "UI/Application.h"
#include "Logging.h"
#include <iostream>
#include <cmath>
#include <cassert>

namespace Engine
{
    Camera::Camera() :
        projection(Matrix::Identity())
    {
    }

    static void PostRedraw()
    {
        Application::Get().bus.Post(Redraw());
    }

    void Camera::Move(const Coordinate& newPosition)
    {
        position = newPosition;
        PostRedraw();
    }
    void Camera::Move(const Vector& offset)
    {
        position += offset;
        PostRedraw();
    }

    void Camera::Zoom(double factor)
    {
        zoom *= factor;
        PostRedraw();
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
            Logging::Log<UiLogging, Logging::Debug>("Face (%.3f, %.3f, %.3f) yaw=%.1f, pitch=%.1f, distance %.3f",
                vector.x, vector.y, vector.z,
                Rad2Deg(yaw), Rad2Deg(pitch), vector.Length());
        }
        PostRedraw();
    }

    void Camera::Turn(const Vector& delta)
    {
        rotation += delta;
        PostRedraw();
    }

    Matrix Camera::Projection() const
    {
        return projection;
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
        projection = Matrix::Projection();  // TODO can just compute it
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
        projection = Matrix::Projection();  // TODO can just compute it
        glMatrixMode(GL_MODELVIEW);
    }

    FreeCamera::FreeCamera()
    {
        Application::Get().bus.Subscribe(*this, {
                MessageType<KeyPressed>(),
                MessageType<ScrollWheel>()
        });
    }

    void FreeCamera::OnMessage(const Message& message)
    {
        if (auto key = message.Cast<KeyPressed>())
        {
            static constexpr double rotationSpeed = 0.05;
            if (key->modifiers & GLUT_ACTIVE_SHIFT)
            {
                if (key->code == GLUT_KEY_UP)
                {
                    Turn({-rotationSpeed, 0, 0});
                }
                else if (key->code == GLUT_KEY_DOWN)
                {
                    Turn({+rotationSpeed, 0, 0});

                }
                else if (key->code == GLUT_KEY_LEFT)
                {
                    Turn(vertical * -rotationSpeed);
               }
                else if (key->code == GLUT_KEY_RIGHT)
                {
                    Turn(vertical * rotationSpeed);
                }
            }
            else
            {
                if (key->code == GLUT_KEY_UP)
                {
                    Vector dir = Vector(sin(rotation.z), cos(rotation.z), 0);
                    Move(dir);
                }
                else if (key->code == GLUT_KEY_DOWN)
                {
                    Vector dir = -Vector(sin(-rotation.z), cos(-rotation.z), 0);
                    Move(dir);
                }
                else if (key->code == GLUT_KEY_RIGHT)
                {
                    Vector dir = Vector(cos(-rotation.z), sin(-rotation.z), 0);
                    Move(dir);
                }
                else if (key->code == GLUT_KEY_LEFT)
                {
                    Vector dir = -Vector(cos(-rotation.z), sin(-rotation.z), 0);
                    Move(dir);
                }
                else if (key->code == GLUT_KEY_PAGE_UP)
                {
                    Move(vertical);
                }
                else if (key->code == GLUT_KEY_PAGE_DOWN)
                {
                    Move(-vertical);
                }
                else if (key->ascii == '+')
                {
                    Zoom(1.1);
                }
                else if (key->ascii == '-') // smiley
                {
                    Zoom(1.0 / 1.1);
                }
            }
        }
        else if (auto scroll = message.Cast<ScrollWheel>())
        {
            if (scroll->up)
            {
                Zoom(1.1);
            }
            else
            {
                Zoom(1.0/ 1.1);
            }
        }
    }


    TrackingCamera::TrackingCamera()
    {
        Application::Get().bus.Subscribe(*this, {
                MessageType<KeyPressed>(),
                MessageType<ScrollWheel>()
        });
    }

    void TrackingCamera::Track(const Coordinate& newTarget)
    {
        target = newTarget;
        Face(target);
    }

    void TrackingCamera::OnMessage(const Message& message)
    {
        if (auto key = message.Cast<KeyPressed>())
        {
            if (key->modifiers!=0)
                return;

            if (key->code == GLUT_KEY_UP)
            {
                Vector dir = Vector(sin(rotation.z), cos(rotation.z), 0);
                Move(dir);
                Face(target);
            }
            else if (key->code == GLUT_KEY_DOWN)
            {
                Vector dir = -Vector(sin(-rotation.z), cos(-rotation.z), 0);
                Move(dir);
                Face(target);
            }
            else if (key->code == GLUT_KEY_RIGHT)
            {
                Vector dir = Vector(cos(-rotation.z), sin(-rotation.z), 0);
                Move(dir);
                Face(target);
            }
            else if (key->code == GLUT_KEY_LEFT)
            {
                Vector dir = -Vector(cos(-rotation.z), sin(-rotation.z), 0);
                Move(dir);
                Face(target);
            }
            else if (key->code == GLUT_KEY_PAGE_UP)
            {
                Move(vertical);
                Face(target);
            }
            else if (key->code == GLUT_KEY_PAGE_DOWN)
            {
                Move(-vertical);
                Face(target);
            }
            else if (key->ascii == '+')
            {
                Zoom(1.1);
            }
            else if (key->ascii == '-') // smiley
            {
                Zoom(1.0 / 1.1);
            }
        }
        else if (auto scroll = message.Cast<ScrollWheel>())
        {
            if (scroll->up)
            {
                Zoom(1.1);
            }
            else
            {
                Zoom(1.0/ 1.1);
            }
        }
    }


}
