#pragma once
#include <string>
#include "Logic/Knowledge.h"
#include <ios>
#include "Errors.h"

namespace Angel
{
namespace Logic { class Object;  }
namespace Parser
{

Logic::Knowledge Parse(const std::string& text);
std::ostream& operator<<(std::ostream& s, const Logic::Object& o);
std::istream& operator>>(std::istream& s, Logic::Object& o);
}
}