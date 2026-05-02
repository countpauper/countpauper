#pragma once

#include <string_view>

namespace Engine
{
enum class Align
{
    bottom=-1,
    right=-1,
    center=0,
    left=1,
    top=1
};

void glText(std::string_view text, const class Font& font, Align horizontal = Align::left, Align vertical = Align::top);
}
