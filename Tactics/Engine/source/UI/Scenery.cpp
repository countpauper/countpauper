#include "UI/Scene.h"
#include "Geometry/Mesh.h"
#include <GL/gl.h>
#include "Geometry/Matrix.h"
#include "Rendering/Text.h"
namespace Engine
{

Scenery::Scenery(Mesh& mesh) :
    mesh(mesh)
{

}
void Scenery::Render() const
{
    glPushMatrix();
    GetCoordinate().Render();
    GetOrientation().Render();
    mesh.Render();
    glPopMatrix();
}

Coordinate Scenery::GetCoordinate() const
{
    return Coordinate::origin;
}

Quaternion Scenery::GetOrientation() const
{
    return Quaternion::Identity;
}

std::pair<double, std::uint32_t> Scenery::Intersection(const Line& line) const
{
    auto localLine = GetOrientation().Conjugate() * (line - Vector(GetCoordinate()));
    return mesh.NamedIntersection(localLine);
}

}
