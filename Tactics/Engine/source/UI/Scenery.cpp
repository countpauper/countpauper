#include "UI/Scene.h"
#include "Geometry/Mesh.h"
#include <GL/gl.h>
#include "Geometry/Matrix.h"

namespace Engine
{

Scenery::Scenery(Mesh& mesh) :
    mesh(mesh)
{

}
void Scenery::Render() const
{
    glPushMatrix();
    GetLocation().Render();
    GetOrientation().Render();
    mesh.Render();
    glPopMatrix();
}

Coordinate Scenery::GetLocation() const
{
    return Coordinate::origin;
}

Quaternion Scenery::GetOrientation() const
{
    return Quaternion::Identity();
}

std::pair<double, std::uint32_t> Scenery::Intersection(const Line& line) const
{
    auto localLine = GetOrientation().Conjugate() * (line - Vector(GetLocation()));
    return mesh.NamedIntersection(localLine);
}

}
