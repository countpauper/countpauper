#pragma once
#include <string_view>

namespace Engine
{

class Object
{
public:
    virtual std::string_view Name() const = 0;
};

}
