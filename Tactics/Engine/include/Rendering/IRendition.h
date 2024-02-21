#pragma once

namespace Engine
{

    class IRendition
    {
    public:
        virtual void Render() const = 0;
    };

}