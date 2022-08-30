#pragma once
#include <string>
#include "Logic/Knowledge.h"
#include <ios>
#include <any>
#include "Errors.h"

namespace Angel
{
    namespace Logic { class Object;  }
    namespace Parser
    {
        Logic::Knowledge Parse(const std::string& text);
        std::istream& operator>>(std::istream& s, Logic::Object& o);

        // Syntax.h functions
        std::any Merge(const std::any& a, const std::any& b);

        std::any ParseId(const std::string_view value);
        std::any ParseInteger(const std::string_view value);
        std::any ParseBoolean(const std::string_view value);

        std::any ConstructBracedSequence(const std::any& tokens);
        std::any ConstructBracedExpression(const std::any& tokens);
        std::any ConstructSequence(const std::any& tokens);
        std::any ConstructPredicate(const std::any& tokens);
        std::any ConstructKnowledge(const std::any& tokens);

    }
}
std::ostream& operator<<(std::ostream& s, const Angel::Logic::Object& o);
