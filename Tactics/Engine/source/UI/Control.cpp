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
            remainingPath = path.substr(name.size()+1);
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


}
