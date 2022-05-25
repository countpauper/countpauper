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

Logic::Knowledge Parse(const std::wstring& text);
std::wostream& operator<<(std::wostream& s, const Logic::Object& o);
std::wistream& operator>>(std::wistream& s, Logic::Object& o);
}
}