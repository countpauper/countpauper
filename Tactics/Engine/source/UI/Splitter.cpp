#include "UI/Splitter.h"
#include <GL/gl.h>

namespace Engine
{

Splitter::Splitter(Control& first, Control& second, bool horizontal, double split) :
    horizontal(horizontal),
    split(split)
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
        glScaled(split, 1.0, 1.0);
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
        glScaled(1.0, split, 1.0);
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
        if (pos.X() < split)
        {
            pos = Coordinate(pos.X() * split, pos.Y(), pos.Z());
            return children.front()->Click(pos);
        }
        else
        {
            pos = Coordinate(pos.X() * (1- split), pos.Y(), pos.Z());
            return children.back()->Click(pos);
        }
    }
    else
    {
        if (pos.Y() < split)
        {
            pos = Coordinate(pos.X(), pos.Y()*split, pos.Z());
            return children.front()->Click(pos);
        }
        else
        {
            pos = Coordinate(pos.X(), pos.Y()*(1-split), pos.Z());
            return children.back()->Click(pos);
        }
    }
}

}
