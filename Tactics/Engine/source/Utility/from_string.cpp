#include "Engine/Utility/from_string.h"

namespace Engine
{

#define _SILENCE_CXX17_CODECVT_HEADER_DEPRECATION_WARNING 1

template<>
std::string from_string(std::string_view str)
{
	return std::string(str.data(), str.size());
}

const std::map<std::string_view, bool> boolmap =
{
	{ "false", false },
	{ "true", true },
	{ "no", false },
	{ "yes", true },
	{ "0", false },
	{ "1", true }
};


template<>
bool from_string(std::string_view str)
{
	return from_string<bool>(Strip(str), boolmap);
}

}
