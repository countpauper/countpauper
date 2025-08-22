#pragma once 
#include <cstddef>
#include <type_traits>

namespace eul
{

template<typename T>
std::size_t raw_type_id()
{
    static std::byte id;
    return (std::size_t)(&id);
};

template<typename T>
std::size_t type_id()
{
    using strippedT = std::remove_cv_t<std::remove_reference_t<T>>;
    return raw_type_id<strippedT>();
};

template<typename T>
std::size_t type_id(const T&)
{
    return type_id<T>();
};

}
