#pragma once
#include <vector>
#include <memory>
#include "Engine/Coordinate.h"
#include "Engine/Quaternion.h"
#include "Engine/Camera.h"

namespace Engine
{
class Mesh;

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
    Camera& GetCamera();
private:
    PerspectiveCamera camera;
    // TODO: lights
    std::vector<std::unique_ptr<Prop>> props;
};

}
