#pragma once
#include <string>
#include "UI/Label.h"

namespace Engine
{

class Button : public Label
{
public:
    Button(std::string_view name, std::string_view text);
    Control* Click(Coordinate pos) override;
private:
};

}
