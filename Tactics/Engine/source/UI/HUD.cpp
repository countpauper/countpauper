#include "UI/HUD.h"
#include "Rendering/Text.h"
#include "Geometry/Vector.h"
#include <GL/gl.h>

namespace Engine
{

Control::Control(std::string_view name) :
    name(name)
{
}

Control* Control::FindControl(std::string_view path)
{
    if (path==name)
        return this;
    else
        return nullptr;
}

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

Control* Controls::FindControl(std::string_view path)
{
    auto remainingPath = path;
    if (!name.empty()) {
        if (path.substr(0, name.size()) == name &&
            path.size() >= name.size()+1 &&
            path[name.size()] == pathSeparator)
        {
            auto remainingPath = path.substr(path.size()+1);
        }
        else
            return nullptr;
    }

    for(auto control: *this)
    {
        if (auto found = control->FindControl(remainingPath))
            return found;
    }
    return nullptr;
}

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

Label::Label(std::string_view name, std::string_view text) :
    Control(name),
    text(text)
{
}

void Label::SetText(std::string_view newText)
{
    text=newText;
}

void Label::Render() const
{
    if (!shown)
        return;
    glText(text.c_str());
}

}
