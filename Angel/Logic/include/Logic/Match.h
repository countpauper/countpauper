#pragma once 

#include "Logic/Node.h"
#include <map>
#include <optional>
#include <string>

namespace Angel::Logic 
{
    
using Variables = std::map<std::string, Node>;

using Match = std::optional<Variables>;
static const Match IsMatch=Match(Variables());
static const Match NoMatch=Match();

}