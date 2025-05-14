#pragma once
#include <stdexcept>
#include <format>

namespace Angel::Logic
{


class CastException : public std::runtime_error
{
public:
    CastException(const std::type_info& from, const std::type_info& to) :
        std::runtime_error(std::format("Can not Cast from {} to {} ", from.name(), to.name()))
    {
    }
    template<class _From>
    CastException(const std::type_info& to) :
        CastException(typeid(_From), to)
    {
    }
};


}
