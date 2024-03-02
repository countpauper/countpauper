#pragma once
#include "UI/Control.h"
#include "Geometry/Matrix.h"

namespace Engine
{

class HUD : public Controls
{
public:
    HUD();
    void Render() const override;
    Control* Click(Coordinate pos) const override;
private:
    Matrix projection;
};

}
