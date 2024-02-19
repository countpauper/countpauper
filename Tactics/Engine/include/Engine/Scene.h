#pragma once
#include <vector>
#include <memory>
#include <cstdint>
#include "Engine/Prop.h"
#include "Engine/Camera.h"

namespace Engine
{
class Mesh;

class Scene
{
public:
    Scene() = default;
    Prop& Add(Prop&& prop);
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
