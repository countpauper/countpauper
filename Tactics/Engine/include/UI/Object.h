#pragma once
#include <string_view>
#include "Geometry/Coordinate.h"
#include "Geometry/Quaternion.h"

namespace Engine
{

class Object
{
public:
    virtual std::string_view Name() const = 0;
    virtual void Render() const = 0;
    virtual Coordinate GetCoordinate() const = 0;
    virtual Quaternion GetOrientation() const = 0;
};

}
