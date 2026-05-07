#pragma once
#include "Rendering/Color.h"
#include "Rendering/Font.h"

namespace Engine
{

struct Style
{
    RGBA outline = RGBA::transparent;
    float lineWidth = 1.0;
    RGBA background = RGBA::transparent;
    RGBA textColor = RGBA::white;
    Font font;
};

}
