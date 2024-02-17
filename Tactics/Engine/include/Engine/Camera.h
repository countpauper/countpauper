#pragma once

#include "Coordinate.h"
#include "Vector.h"

namespace Engine
{
    class Camera
    {
    public:
        Camera();

        virtual void Render() const = 0;
        void Move(const Coordinate& newPosition);
        void Move(const Vector& offset);
        void Face(const Coordinate& newPosition);
        void Zoom(double factor);
        void Drag(double dx, double dz);
        void FinishDrag(double dx, double dz);
        // Key is ascii if >=32 or GLUT_KEY_X if <32
        // modifiers is a bit mask of GLUT_ACTIVE_
        // returns true if handled
        bool Key(unsigned char key, unsigned modifiers);
    protected:
        Vector vertical{0, 0, 1};
        Coordinate position;
        Coordinate target;
        Coordinate rotation;
        double dragx, dragz;
        double zoom;
    };


    class PerspectiveCamera : public Camera
    {
    public:
        PerspectiveCamera();
        void Render() const override;
        float fov;
    };

    class TopCamera : public Camera
    {
    public:
        TopCamera();
        void Render() const override;
    };

}   // ::Engine
