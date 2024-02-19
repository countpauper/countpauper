#pragma once
#include <cstdint>
#include "Engine/Object.h"
#include "Engine/Coordinate.h"
#include "Engine/Quaternion.h"
#include "Engine/Camera.h"
#include "Engine/Range.h"

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
