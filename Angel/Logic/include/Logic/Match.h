#pragma once 

#include <map>
#include <optional>
#include "Object.h"

namespace Angel::Logic 
{
    
using Variables = std::map<std::string, Object>;

using Match = std::optional<Variables>;
static const Match IsMatch=Match(Variables());
static const Match NoMatch=Match();

using Matches=std::map<const class Clause*, Variables>;

}