#include "UI/Scene.h"
#include "Geometry/Mesh.h"
#include <GL/gl.h>
#include "Geometry/Matrix.h"

namespace Engine
{

Scenery& Scene::Add(Scenery& prop)
{
    scenery.emplace_back(&prop);
    return *scenery.back();
}

Camera& Scene::GetCamera()
{
    return camera;
}

std::pair<Scenery*, uint32_t> Scene::Select(Coordinate c) const
{
    Line ray(c - Vector(0,0,1), c + Vector(0,0,1));
    ray *= camera.Projection().Inverse();
    return Hit(ray);
}

std::pair<Scenery*, std::uint32_t> Scene::Hit(const Line& line) const
{
    double nearest = std::numeric_limits<float>::max();
    Scenery* result = nullptr;
    std::uint32_t resultName = 0;

    for(const auto& obj : scenery)
    {
        auto [distance, name] = obj->Intersection(line);
        if (distance< nearest)
        {
            nearest = distance;
            result = obj;
            resultName = name;
        }
    }
    return std::make_pair(result, resultName);
}

void Scene::Render() const
{
    camera.Render();
    for(const auto& obj: scenery)
    {
        obj->Render();
    }
}

}
