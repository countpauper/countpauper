#pragma once
#include "UI/Control.h"

namespace Engine
{

/// @brief A control with two children that splits its area in two and renders each child in on each side
class Splitter : public Controls
{
public:
    Splitter(Control& one, Control& two, bool horizontal=true, double split = 0.5);
    void Move(double split);
    void Render() const override;
    Control* Click(Coordinate pos) override;
private:
    bool horizontal;
    double split;
};

}
