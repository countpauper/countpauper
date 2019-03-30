#pragma once

#include <string>
#include <sstream>
#include <map>
#include <iterator>
#include "Utils.h"

namespace Engine
{
template<typename T>
T from_string(const std::wstring& str)
{
    T result;
    std::wstringstream stream(str);
    stream >> result;
    if (stream.fail())
        throw std::runtime_error("Failed to parse value from string");
    return result;
}
template<> std::wstring from_string(const std::wstring& str);
template<> std::string from_string(const std::wstring &str);
template<> bool from_string(const std::wstring& str);

template <typename T>
std::vector<T> from_strings(const std::wstring& str, wchar_t delimiter)
{
    auto strs = Split(str, delimiter);
    std::vector<T> o(strs.size());
    std::transform(strs.begin(), strs.end(), o.begin(), [](const std::wstring& str)
    {
        return Engine::from_string<T>(str);
    });
    return o;
}

template<typename T>
T from_string(const std::wstring& str, const std::map<const std::wstring, T>& mapping)
{
    return mapping.at(str);
}

template <typename T>
std::set<T> from_strings(const std::wstring& str, wchar_t delimiter, const std::map<const std::wstring, T>& mapping)
{
    auto strs = Split(str, delimiter);
    std::set<T> o;
    std::transform(strs.begin(), strs.end(), std::inserter(o,o.begin()), [&mapping](const std::wstring& str)
    {
        return Engine::from_string<T>(str,mapping);
    });
    return o;
}



}