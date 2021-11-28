#include "stdafx.h"
#include "from_string.h"

namespace Engine
{

#define _SILENCE_CXX17_CODECVT_HEADER_DEPRECATION_WARNING 1

template<>
std::string from_string(const std::wstring_view wstr)
{
    if (wstr.empty())
    {
        return "";
    }

    const auto size_needed = WideCharToMultiByte(CP_UTF8, 0, &wstr.at(0), (int)wstr.size(), nullptr, 0, nullptr, nullptr);
    if (size_needed <= 0)
    {
        throw std::runtime_error("WideCharToMultiByte() failed: " + std::to_string(size_needed));
    }

    std::string result(size_needed, 0);
    WideCharToMultiByte(CP_UTF8, 0, &wstr.at(0), (int)wstr.size(), &result.at(0), size_needed, nullptr, nullptr);
    return result;
}



const std::map<const std::wstring_view, bool> boolmap =
{
	{ L"false", false },
	{ L"true", true },
	{ L"no", false },
	{ L"yes", true },
	{ L"0", false },
	{ L"1", true }
};


template<> 
bool from_string(const std::wstring_view str)
{
	return from_string<bool>(Strip(str), boolmap);
}

}
