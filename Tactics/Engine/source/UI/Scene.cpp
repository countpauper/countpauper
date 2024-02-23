#include "UI/Scene.h"
#include "Geometry/Mesh.h"
#include <GL/gl.h>
#include "Geometry/Matrix.h"

namespace Engine
{

Scenery& Scene::Add(Scenery& prop)
{
    props.emplace_back(&prop);
    return *props.back();
}

Camera& Scene::GetCamera()
{
    return camera;
}

std::pair<Scenery*, uint32_t> Scene::Select(Coordinate c) const
{
    Line ray(c - Vector(0,0,1), c + Vector(0,0,1));
    ray *= Matrix::Projection().Inverse();
    return Hit(ray);
}

std::pair<Scenery*, std::uint32_t> Scene::Hit(const Line& line) const
{
    double nearest = std::numeric_limits<float>::max();
    Scenery* result = nullptr;
    std::uint32_t resultName = 0;

    for(const auto& prop : props)
    {
        auto [distance, name] = prop->Intersection(line);
        if (distance< nearest)
        {
            nearest = distance;
            result = prop;
            resultName = name;
        }
    }
    return std::make_pair(result, resultName);
}

void Scene::Render() const
{
    camera.Render();
    for(const auto& prop: props)
    {
        prop->Render();
    }
}

}