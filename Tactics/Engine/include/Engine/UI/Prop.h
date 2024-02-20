#pragma once
#include <cstdint>
#include "Engine/UI/Object.h"
#include "Engine/Geometry/Coordinate.h"
#include "Engine/Geometry/Quaternion.h"
#include "Engine/UI/Camera.h"
#include "Engine/Utility/Range.h"

#undef GetObjectA // Thanks windows.h
namespace Engine
{
class Mesh;

class Prop
{
public:
    Prop(Object* object, Mesh& mesh, Coordinate loc, Quaternion ori= Quaternion::Identity());
    void Render() const;
    std::pair<double, std::uint32_t> Intersection(const Line& line) const;
    Object* operator->();
private:
    Object* object;
    Mesh& mesh;
    Coordinate location;
    Quaternion orientation;
};


}
