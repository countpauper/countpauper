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
        std::istream& operator>>(std::istream& s, Logic::Object& o);
    }
}
std::ostream& operator<<(std::ostream& s, const Angel::Logic::Object& o);
