#pragma once

#include "Engine/Rendering/Color.h"
#include "Engine/Rendering/IRendition.h"
#include "Engine/Geometry/Coordinate.h"

namespace Engine
{
    class Light : public IRendition
    {
    public:
        Light();
        void Render() const override;
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
