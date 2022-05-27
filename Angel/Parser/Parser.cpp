#include "stdafx.h"
#include <sstream>
#include <optional>
#include <map>
#include <ctype.h>
#include "Parser.h"
#include "Parser/Syntax.h"
#include "Parser/Interpreter.h"
#include "Logic/Predicate.h"
#include "Logic/Boolean.h"
#include "Logic/Integer.h"
#include "Logic/Sequence.h"
#include "Logic/Conjunction.h"
#include "Logic/Set.h"
#include "Logic/Clause.h"
#include "Logic/Object.h"

namespace Angel
{
namespace Parser
{

class LogicInterpreter : public BNF::Interpreter
{
    std::any Interpret(const std::string_view rule, const std::any& interpretation, const std::string_view value) const override
    {
        if (rule == "knowledge")
        {
            Logic::Knowledge knowledge;
            if (interpretation.type() == typeid(const std::vector<Logic::Clause>&))
            {
                const std::vector<Logic::Expression>& expressions = std::any_cast<const std::vector<Logic::Expression>&>(interpretation);
                for (auto& e : expressions)
                {
                    knowledge.Know(Logic::Object(e.Copy()));
                }
            }
            return knowledge;
        }
        else if (rule == "id")
        {
            return Logic::id(value);
        }
        else if (rule == "integer")
        {
            return Logic::integer(value);
        }
        else if (rule == "boolean")
        {
            return Logic::boolean(value);
        }
        else if (rule == "element")
        {
            return interpretation;
        }

        return interpretation;
    }

    std::any Merge(const std::any& left, const std::any& right) const
    {
        return left;    // what? we looped, make a sequence of them? which type? we just moved the sequence/set parsing problem arround the long way 
        // perhaps just allow vectors of expressions, but interpret should really tell us what is interpreted, also for sequence/disjunction and which index perhaps too 
    }
};

LogicInterpreter interpreter;

Logic::Knowledge Parse(const std::string& text)
{
	Logic::Knowledge result;
    auto match = BNF::Parse(BNF::knowledge, interpreter, text.c_str());
    return std::any_cast<Logic::Knowledge>(match.interpretation);
}

std::ostream& operator<<(std::ostream& s, const Logic::Object& o)
{
    assert(false);  // TODO: all expressions shoudl be serializable
    return s;
}

std::istream& operator>>(std::istream& s, Logic::Object& o)
{
    // TODO: allow bnf to parse from stream and reset streams on failure? 
    std::string allData;
    while (!s.eof())
    {
        char buffer[1024];
        auto readChars = s.readsome(buffer, sizeof(buffer));
        if (!readChars)
            break;
        allData.append(buffer, static_cast<size_t>(readChars));
    }

    // skip inital whitespace
    std::string_view start = allData;
    if (auto whiteMatch = BNF::Whitespace().Parse(allData.c_str(), interpreter))
    {
        start = whiteMatch->remaining;
    }
    auto match = BNF::Parse(BNF::expression, interpreter, start);
    s.seekg(-std::streamoff(match.remaining.length()), std::ios_base::cur);
 
    o = std::any_cast<const Logic::Object&>(match.interpretation);
    return s;
}

}
}
