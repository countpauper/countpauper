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


const std::map<const std::wstring, bool> boolmap =
{
	{ L"false", false },
	{ L"true", true },
	{ L"no", false },
	{ L"yes", true },
	{ L"0", false },
	{ L"1", true }
};


template<> 
bool from_string(const std::wstring& str)
{
	return from_string<bool>(Strip(str), boolmap);
}

}
