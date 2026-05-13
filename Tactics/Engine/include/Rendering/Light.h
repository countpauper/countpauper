#pragma once

#include "Rendering/Color.h"
#include "Rendering/IRendition.h"
#include "Geometry/Coordinate.h"

namespace Engine
{
    class Light : public IRendition
    {
    public:
        Light();
        void On();
        void Render() const override;
        void Off();
        void Move(const Coordinate& newPosition);
    private:

        unsigned id;

        float power;            // base power. 0.0 if off
        float directional;      // how parallel are the rays. 0.0 (sun) is totally parallel
        Coordinate position;
        float diffusePower;
        RGBA diffuse;
        float ambientPower;
        RGBA ambient;
        float specularPower;
        RGBA specular;
    };



}   // ::Engine
