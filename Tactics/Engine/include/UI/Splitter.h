#pragma once
#include <string>
#include "UI/Control.h"
#include "Rendering/Color.h"

namespace Engine
{

class Splitter : public Controls
{
public:
    Splitter(Control& one, Control& two, double split = 0.5, bool horizontal=true);
    void Move(double split);
    void Render() const override;
    Control* Click(Coordinate pos) const override;
private:
    double split;
    bool horizontal;
};

}
