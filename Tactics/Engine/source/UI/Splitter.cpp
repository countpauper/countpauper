#include "UI/Splitter.h"
#include <GL/gl.h>

namespace Engine
{

Splitter::Splitter(Control& first, Control& second, double split, bool horizontal) :
    split(split),
    horizontal(horizontal)
{
    Add(first);
    Add(second);
}
void Splitter::Move(double split)
{
    split = split;
}

void Splitter::Render() const
{
    if (horizontal)
    {
        glPushMatrix();
        glScaled(split, 1.0, 0.0);
        children.front()->Render();
        glPopMatrix();

        glPushMatrix();
        glTranslated(split, 0.0, 0.0);
        glScaled(1.0-split, 1.0, 1.0);
        children.back()->Render();
        glPopMatrix();
    }
    else
    {
        glPushMatrix();
        glScaled(1.0, split, 0.0);
        children.front()->Render();
        glPopMatrix();

        glPushMatrix();
        glTranslated(0.0, split, 0.0);
        glScaled(1.0, 1.0-split, 1.0);
        children.back()->Render();
        glPopMatrix();
    }
}

Control* Splitter::Click(Coordinate pos) const
{
    if (horizontal)
    {
        if (pos.x<split)
        {
            pos.x *= split;
            return children.front()->Click(pos);
        }
        else
        {
            pos.x *= 1-split;
            return children.back()->Click(pos);
        }
    }
    else
    {
        if (pos.y < split)
        {
            pos.y *= split;
            return children.front()->Click(pos);
        }
        else
        {
            pos.y *= 1-split;
            return children.back()->Click(pos);
        }
    }
}

}
