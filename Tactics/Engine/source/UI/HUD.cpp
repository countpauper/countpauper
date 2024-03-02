#include "UI/HUD.h"
#include "Rendering/Text.h"
#include "Geometry/Vector.h"
#include <GL/gl.h>

namespace Engine
{


void Controls::Render() const
{
    if (!shown)
        return;
    for(auto control: *this)
    {
        control->Render();
    }
}

Control* Controls::Click(Coordinate pos) const
{
    if (!enabled || !shown)
        return nullptr;

    for(auto control: *this)
    {
        auto hit = control->Click(pos);
        return hit;
    }
    return nullptr;
}

HUD::HUD()
{
    projection = Matrix::Identity();
    projection *= Matrix::Scale({2.0, -2.0, 1.0});
    projection.SetTranslation({-1.0, 1.0, -1.0});
}

void HUD::Render() const
{
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    projection.Render();
    //glScaled(2.0, -2.0, 1.0);       // make positive y go down
    //glTranslated(-0.5, -0.5, -1.0); // top left and front of z-buffer
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    Controls::Render();
}

Control* HUD::Click(Coordinate pos) const
{
    return Controls::Click(projection.Inverse() * pos);
}

Label::Label(std::string_view text) :
    text(text)
{
}

void Label::SetText(std::string_view text)
{
    text=text;
}

void Label::Render() const
{
    if (!shown)
        return;
    glText(text.c_str());
}

}
