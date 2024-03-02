#include "UI/HUD.h"
#include "Geometry/Vector.h"
#include <GL/gl.h>

namespace Engine
{

HUD::HUD() :
    Controls("")
{
    projection = Matrix::Identity();
    projection *= Matrix::Scale({2.0, -2.0, 1.0});  // make positive y go down and XY 0...1
    projection.SetTranslation({-1.0, 1.0, -1.0}); // top left and front of Z-buffer
}

void HUD::Render() const
{
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    projection.Render();
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    Controls::Render();
}

Control* HUD::Click(Coordinate pos) const
{
    return Controls::Click(projection.Inverse() * pos);
}


}
