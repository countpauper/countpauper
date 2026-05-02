#pragma once
#include "UI/Control.h"
#include <vector>
#include <functional>
namespace Engine
{

/// @brief A control that renders its children side by side in one direction, using up the entire area in the other direction
class Bar : public Controls
{
public:
    Bar(std::vector<std::reference_wrapper<Control>> controls, bool horizontal=true, double ratio=1.0f);
    void Render() const override;
    Control* Click(Coordinate pos) override;
private:
    bool horizontal;
    double ratio;
};

}
