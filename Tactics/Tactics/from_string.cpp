#include "stdafx.h"
#include "from_string.h"

namespace Engine
{

template<>
std::wstring from_string(const std::wstring& str)
{
    return str;
}

template<>
std::string from_string(const std::wstring &str)
{
    return std::string(str.begin(), str.end());
}

}
