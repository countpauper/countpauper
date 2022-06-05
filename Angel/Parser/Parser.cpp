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

// TODO: generalize to all collections, including disjunctions, products, summations
void RecursiveCollect(Logic::Sequence& seq, const std::any& interpretation)
{
    if (!interpretation.has_value())
        return;
    if (interpretation.type() == typeid(Logic::Object))
    {
        seq.Add(std::any_cast<Logic::Object>(interpretation));
        return;
    }
    if (interpretation.type() == typeid(std::vector<std::any>))
    {
        const auto& elements = std::any_cast<std::vector<std::any>>(interpretation);
        for (const auto&e : elements)
        {
            RecursiveCollect(seq, e);
        }
        return;
    }
    assert(false); // then what is it
}

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
        if (rule == "braces")
        {
            if (interpretation.type() == typeid(const std::vector<Logic::Object>&))
            {
                const auto& obj = std::any_cast<Logic::Object>(interpretation);
                if (obj.As<Logic::Collection>())
                {
                    return obj;
                }
                else
                {
                    return Logic::sequence(Logic::Object(obj));
                }
            }
            else
            {

            }
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
        else if (rule == "sequence")
        {
            auto result = std::make_unique<Logic::Sequence>();
            RecursiveCollect(*result, interpretation);
            return Logic::Object(std::move(result));
        }

        return interpretation;
    }

    std::any Merge(const std::any& left, const std::any& right) const
    {
        if (!right.has_value())
            return left;

        if (left.has_value())
        {
            if (left.type() == typeid(std::vector<std::any>))
            {
                const auto& leftArray = std::any_cast<std::vector<std::any>>(left);
                std::vector<std::any> merged(leftArray);
                merged.push_back(right);
                return merged;
            }
            else
            {
                assert(false); // what? merge two normal expressions into a vector? Under which circumstances?  
                return left;
            }
        }
        else 
        {
            return std::vector<std::any>{right};
        }
                
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
