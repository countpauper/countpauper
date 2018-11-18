#pragma once

#include "Color.h"
#include "Coordinate.h"

namespace Engine
{
    class Light
    {
    public:
        Light();
        void Render() const;
        void On();
        void Off();
        void Move(const Coordinate& newPosition);
    private:
        bool on;
        Coordinate position;
        float diffusePower;
        RGBA diffuse;
        float ambientPower;
        RGBA ambient;
        float specularPower;
        RGBA specular;
    };

}   // ::Engine
