#pragma once

#include "Coordinate.h"

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
        void Key(WPARAM key);
    protected:
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
