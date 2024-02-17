#include "Engine/Scene.h"
#include <GL/gl.h>

namespace Engine
{

Prop::Prop(Mesh& mesh, Coordinate loc, Quaternion ori) :
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

Prop& Scene::Add(Mesh& mesh, Coordinate loc, Quaternion ori)
{
    props.emplace_back(std::make_unique<Prop>(mesh, loc, ori));
    return *props.back();
}

void Scene::Render() const
{
    for(const auto& prop: props)
    {
        prop->Render();
    }
}

}
