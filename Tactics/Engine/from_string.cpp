#include "from_string.h"

namespace Engine
{

#define _SILENCE_CXX17_CODECVT_HEADER_DEPRECATION_WARNING 1


const std::map<const std::string_view, bool> boolmap =
{
	{ "false", false },
	{ "true", true },
	{ "no", false },
	{ "yes", true },
	{ "0", false },
	{ "1", true }
};


}
