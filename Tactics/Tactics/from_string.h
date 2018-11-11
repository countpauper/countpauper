#pragma once

#include <string>
#include <sstream>

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

}