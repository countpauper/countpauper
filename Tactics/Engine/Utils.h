#pragma once

#include <string>
#include <vector>
#include <set>

namespace Engine
{
    std::vector<std::wstring> Split(const std::wstring& str, wchar_t delimiter);
    std::vector<std::string> Split(const std::string& str, char delimiter);
    std::set<std::string> SplitSet(const std::string& str, char delimiter);

    extern const wchar_t* whitespace;
}