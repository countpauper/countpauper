#pragma once

#include "Coordinate.h"

namespace Engine
{
    class Camera
    {
    public:
        Camera();

        void Render() const;
        void Move(const Coordinate& newPosition);
        void Zoom(float delta);
        void Drag(float dx, float dz);
        void FinishDrag(float dx, float dz);
    private:
        Coordinate position;
        float dragx, dragz;
        float fov;
        float zoom;
    };
}   // ::Engine
