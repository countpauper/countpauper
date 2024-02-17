#pragma once
#include <vector>
#include <memory>
#include <cstdint>
#include "Engine/Coordinate.h"
#include "Engine/Quaternion.h"
#include "Engine/Camera.h"
#include "Engine/Range.h"

namespace Engine
{
class Mesh;
class Line;

class Prop
{
public:
    Prop(std::string_view name, Mesh& mesh, Coordinate loc, Quaternion ori= Quaternion::Identity());
    void Render() const;
    std::pair<double, std::uint32_t> Intersection(const Line& line) const;
    std::string_view Name() const;
private:
    std::string name;
    Mesh& mesh;
    Coordinate location;
    Quaternion orientation;
};

class Scene
{
public:
    Scene() = default;
    Prop& Add(std::string_view name, Mesh& mesh, Coordinate loc, Quaternion ori=Quaternion::Identity());
    void Render() const;
    Camera& GetCamera();
    std::pair<Prop*, std::uint32_t> Select(Coordinate c) const;
    std::pair<Prop*, std::uint32_t> Hit(const Line& line) const;
private:
    PerspectiveCamera camera;
    // TODO: lights
    std::vector<std::unique_ptr<Prop>> props;
};

}
