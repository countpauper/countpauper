#include "Engine/Scene.h"
#include "Engine/Mesh.h"
#include <GL/gl.h>
#include "Engine/Matrix.h"

namespace Engine
{

Prop::Prop(std::string_view name, Mesh& mesh, Coordinate loc, Quaternion ori) :
    name(name),
    mesh(mesh),
    location(loc),
    orientation(ori)
{

}
void Prop::Render() const
{
    glPushMatrix();
    location.Render();
    orientation.Render();
    mesh.Render();
    glPopMatrix();
}

std::string_view Prop::Name() const
{
    return std::string_view(name.data(), name.size());
}

std::pair<double, std::uint32_t> Prop::Intersection(const Line& line) const
{
    auto localLine = orientation.Conjugate() * (line - Vector(location));
    return mesh.Intersection(localLine);
}

Prop& Scene::Add(std::string_view name, Mesh& mesh, Coordinate loc, Quaternion ori)
{
    props.emplace_back(std::make_unique<Prop>(name, mesh, loc, ori));
    return *props.back();
}

Camera& Scene::GetCamera()
{
    return camera;
}

std::pair<Prop*, uint32_t> Scene::Select(Coordinate c) const
{
    Line ray(c - Vector(0,0,1), c + Vector(0,0,1));
    ray *= Matrix::Projection().Inverse();
    return Hit(ray);
}

std::pair<Prop*, std::uint32_t> Scene::Hit(const Line& line) const
{
    double nearest = std::numeric_limits<float>::max();
    Prop* result = nullptr;
    std::uint32_t resultName = 0;

    for(const auto& prop : props)
    {
        auto [distance, name] = prop->Intersection(line);
        if (distance< nearest)
        {
            nearest = distance;
            result = prop.get();
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
