#include "Engine/UI/Scene.h"
#include "Engine/Geometry/Mesh.h"
#include <GL/gl.h>
#include "Engine/Geometry/Matrix.h"

#undef GetObjectA // Thanks windows.h

namespace Engine
{

Prop::Prop(Object* object, Mesh& mesh, Coordinate loc, Quaternion ori) :
    object(object),
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

Object* Prop::operator->()
{
    return object;
}

std::pair<double, std::uint32_t> Prop::Intersection(const Line& line) const
{
    auto localLine = orientation.Conjugate() * (line - Vector(location));
    return mesh.NamedIntersection(localLine);
}

}
