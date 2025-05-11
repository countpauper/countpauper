#pragma once
#include <stdexcept>

namespace Angel::Logic
{

template<class _From>
class CastException : public std::runtime_error
{
public:
    CastException(const std::type_info& to) :
        std::runtime_error((std::string("Can not Cast from ") + typeid(_From).name() + " to " + to.name()))
    {
    }
};

}
