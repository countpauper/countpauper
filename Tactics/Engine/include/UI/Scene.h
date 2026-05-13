#pragma once
#include <vector>
#include <memory>
#include <cstdint>
#include "UI/Scenery.h"
#include "UI/Camera.h"
#include "Rendering/Light.h"

namespace Engine
{
class Mesh;

class Scene
{
public:
    Scene();
    Scenery& Add(Scenery& prop);
    void Render() const;
    Camera& GetCamera();
    std::pair<Scenery*, std::uint32_t> Select(Coordinate c) const;
    std::pair<Scenery*, std::uint32_t> Hit(const Line& line) const;
private:
    TrackingCamera camera;
    Light sun;
    std::vector<Scenery*> scenery;
};

}
