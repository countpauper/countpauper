#include "UI/HUD.h"
#include "Geometry/Vector.h"
#include <GL/gl.h>
#include "Utility/GLutil.h"

namespace Engine
{

HUD::HUD() :
    Controls("")
{
    parent = this;    // root is its own parent
    projection = Matrix::Identity();
    projection *= Matrix::Scale({2.0, -2.0, 1.0});  // make positive y go down and XY 0...1
    projection.SetTranslation({-1.0, 1.0, -1.0}); // top left and front of Z-buffer
}

void HUD::Render() const
{
    // TODO: this doesn't clip at all, which will be weird eventually
    // In that case an AABB should be used for the window, multiplied with a matrix for each control
    // and applied as a glviewport before actually rendering. This requires some sort of helper and state to
    // generalize this and separate it from actual rendering
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    projection.Render();
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    glTemporary blend(GL_BLEND, true);
    Controls::Render();
}

Control* HUD::Click(Coordinate pos) const
{
    return Controls::Click(projection.Inverse() * pos);
}


}
