#pragma once

#include <string>
#include <sstream>
#include <map>
#include <set>
#include <iterator>
#include <algorithm>
#include "Utils.h"

namespace Engine
{

template<typename T>
T from_string(const std::string_view str)
{
    T result;
    std::stringstream stream(std::string(str.data()));
    stream >> result;
    if (stream.fail())
        throw std::runtime_error("Failed to parse value from string");
    return result;
}

template<> std::string from_string(const std::string_view str);
template<> bool from_string(const std::string_view str);

template <typename T>
std::vector<T> from_strings(const std::string_view str, char delimiter)
{
    auto strs = Split(str, delimiter);
    std::vector<T> o(strs.size());
    std::transform(strs.begin(), strs.end(), o.begin(), [](std::string_view str)
    {
        return Engine::from_string<T>(str);
    });
    return o;
}

template<typename T>
T from_string(const std::string_view str, const std::map<const std::string_view, T>& mapping)
{
    return mapping.at(str);
}

template <typename T>
std::set<T> from_strings(const std::string_view str, wchar_t delimiter, const std::map<const std::string_view, T>& mapping)
{
    auto strs = Split(str, delimiter);
    std::set<T> o;
    std::transform(strs.begin(), strs.end(), std::inserter(o,o.begin()), [&mapping](std::string_view str)
    {
        return Engine::from_string<T>(str,mapping);
    });
    return o;
}



}