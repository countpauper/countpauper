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
    return result;
}
}