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
        void Face(const Coordinate& newPosition);
        void Zoom(float delta);
        void Drag(float dx, float dz);
        void FinishDrag(float dx, float dz);
    protected:
        Coordinate position;
        Coordinate target;
        float dragx, dragz;
        float zoom;
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