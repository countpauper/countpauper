#pragma once
#include <vector>
#include "Engine/Coordinate.h"
#include "Engine/Mesh.h"

namespace Engine
{
class Prop
{
public:
    Prop(Mesh& mesh, Coordinate loc, Quaternion ori= Quaternion());
    void Render() const;
private:
    Mesh& mesh;
    Coordinate location;
    Quaternion orientation;
};

class Scene
{
public:
    Scene() = default;
    Prop& Add(Mesh& mesh, Coordinate loc, Quaternion ori=Quaternion());
    void Render() const;
private:
    std::vector<std::unique_ptr<Prop>> props;
};

}
