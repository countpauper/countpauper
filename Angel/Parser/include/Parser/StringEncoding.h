#pragma once
#include <string>

namespace Angel::Parser
{
    std::wstring from_utf8(const std::string_view& utf8);
    std::string to_utf8(const std::wstring_view& ws);
}
