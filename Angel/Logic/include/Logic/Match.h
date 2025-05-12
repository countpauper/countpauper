#pragma once 

#include <map>
#include <optional>
#include <string>

namespace Angel::Logic 
{
    
class Object;

using Variables = std::map<std::string, Object>;

using Match = std::optional<Variables>;
static const Match IsMatch=Match(Variables());
static const Match NoMatch=Match();

}