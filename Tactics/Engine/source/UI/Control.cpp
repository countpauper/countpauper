#include "UI/Control.h"

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
    for(auto child: children)
    {
        child->Render();
    }
}


void Control::Show()
{
    shown = true;
}

void Control::Hide()
{
    shown = false;
}
void Control::Enable()
{
    enabled = true;
}

void Control::Disable()
{
    enabled = false;
}

bool Control::IsShown() const
{
    return shown && parent &&
        (parent == this || parent->IsShown());
}

bool Control::IsEnabled() const
{
    return enabled && parent &&
        (parent==this || parent->IsEnabled()); // TODO recurse;
}



void Controls::Add(Control& control)
{
    if (control.parent)
        throw std::runtime_error("Control reparented");
    children.push_back(&control);
    control.parent = this;
}

Control* Controls::Click(Coordinate pos) const
{
    if (!enabled || !shown)
        return nullptr;

    for(auto child: children)
    {
        auto hit = child->Click(pos);
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
            remainingPath = path.substr(name.size()+1);
        }
        else
            return nullptr;
    }

    for(auto child: children)
    {
        if (auto found = child->FindControl(remainingPath))
            return found;
    }
    return nullptr;
}


}
