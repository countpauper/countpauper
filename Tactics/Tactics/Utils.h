#pragma once

#include <string>
#include <vector>

namespace Engine
{
    std::vector<std::wstring> Split(const std::wstring& str, wchar_t delimiter);
}