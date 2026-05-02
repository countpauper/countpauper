#include "UI/Bar.h"
#include <GL/gl.h>
#include <cmath>

namespace Engine
{

Bar::Bar(std::vector<std::reference_wrapper<Control>> controls, bool horizontal, double ratio) :
    horizontal(horizontal),
    ratio(ratio)
{
    for(auto& control : controls)
    {
        Add(control.get());
    }
}

void Bar::Render() const
{
    double step = 1.0f/ children.size();
    glPushMatrix();
    if (horizontal)
    {
        glScaled(step, 1.0, 1.0);
        for(auto child : children)
        {
            child->Render();
            glTranslated(1.0, 0.0, 0.0);
        }
    }
    else
    {
        glScaled(1.0, step, 1.0);
        for(auto child : children)
        {
            child->Render();
            glTranslated(0.0, 1.0, 0.0);
        }
    }
    glPopMatrix();
}

Control* Bar::Click(Coordinate pos)
{
    if (horizontal)
    {
        double index;
        double offset = std::modf(pos.X() * children.size(), &index);
        return children.at(unsigned(index))->Click(Coordinate(offset, pos.Y(), pos.Z()));
    }
    else
    {
        double index;
        double offset = std::modf(pos.Y() * children.size(), &index);
        return children.at(unsigned(index))->Click(Coordinate(pos.X(), offset, pos.Z()));
    }
}

}
