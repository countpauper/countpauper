#include "stdafx.h"
#include <sstream>
#include <optional>
#include <map>
#include <ctype.h>
#include "Parser.h"
#include "Parser/Syntax.h"
#include "Parser/Parser.h"
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


class LogicParser : public BNF::Parser
{

    std::any ParseImpl(const std::string_view rule, const std::any& tokens, const std::string_view value) const
    {

        /*if (rule == "braces")
        {
        if (interpretation.type() == typeid(Logic::Object))
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
        else*/ if (rule == "id")
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
            return tokens;
        }
        else if (rule == "comma sequence")
        {
            const auto& elements = std::any_cast<std::vector<std::any>>(tokens);
            assert(elements.size() == 2);
            if (elements[1].type() == typeid(std::vector<std::any>))
            {   // flattern recursive vector of anys (should all be comma sequences, assuming that's the only interprationat that returns those 
                // without turning them into Logic::Objects. If recursive summations, con and disjunctions start doing this it will be needed
                // to extend the interpration structure (ector of anys) with the rules applied to match the type 
                auto result = std::any_cast<std::vector<std::any>>(elements[1]);
                result.insert(result.begin(), elements[0]);
                return result;
            }
            else
            {
                return elements;
            }
        }
        else if (rule == "braced expression")
        {
            if (tokens.has_value())
            {
                auto naked_expression = std::any_cast<Logic::Object>(tokens);
                if (naked_expression.As<Logic::Sequence>())
                {   // on sequences, braces are optional since () is an empty sequence and (element) is a size 1 sequence
                    return naked_expression;
                }
                else if (naked_expression.As<Logic::Operator>())
                {   // operators use braces for precedence
                    return naked_expression;
                }
                else
                {    // if the naked expression is not a computation, sequence or collection, then make it one (ie (1) = a sequence of 1 
                    return Logic::sequence(std::move(naked_expression));
                }
            }
        }
        else if (rule == "sequence")
        {
            if (tokens.has_value())
            {
                const auto& elements = std::any_cast<std::vector<std::any>>(tokens);
                auto result = std::make_unique<Logic::Sequence>();
                for (const auto& e : elements)
                {
                    result->Add(std::move(std::any_cast<Logic::Object>(e)));
                }
                return Logic::Object(std::move(result));

            }
            else
                return Logic::sequence();   // empty sequence
        }
        else if (rule == "predicate")
        {
            if (tokens.has_value())
            {
                if (tokens.type() == typeid(Logic::Object))
                {
                    auto id = std::any_cast<Logic::Object>(tokens);
                    return Logic::predicate(*id.As<Logic::Id>());
                }
                else if (tokens.type() == typeid(std::vector<std::any>))
                {
                    const auto& elements = std::any_cast<std::vector<std::any>>(tokens);
                    assert(elements.size() == 2);   // id and argument sequence
                    auto id = std::any_cast<Logic::Object>(elements[0]).As<Logic::Id>();
                    auto args = *std::any_cast<Logic::Object>(elements[1]).As<Logic::Sequence>();

                    return Logic::predicate(*id, std::move(args));
                }
            }

        }
        else if (rule == "knowledge")
        {
            Logic::Knowledge result; 
            if (tokens.has_value())
            {
                auto clauses = std::any_cast<std::vector<std::any>>(tokens);
                for (auto& c : clauses)
                {
                    auto clause = std::any_cast<Logic::Object>(c);
                    result.Know(std::move(clause));

                }
            }
            return result;
        }
        if (tokens.type() == typeid(std::vector<std::any>))
        {
            return tokens;
        }
        return tokens;
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


    std::any Parse(const std::string_view rule, const std::any& tokens, const std::string_view value) const override
    {
        std::string_view inType = tokens.type().name();
        auto result = ParseImpl(rule, tokens, value);
        std::string outType = result.type().name();
        if (result.type() == typeid(Logic::Object))
        {
            const auto& obj = std::any_cast<Logic::Object>(result);
            outType = obj->String();
        }
        return result;
    }

};

LogicParser parser;

Logic::Knowledge Parse(const std::string& text)
{
	Logic::Knowledge result;
    auto match = BNF::Parse(BNF::knowledge, parser, text.c_str());
    return std::any_cast<Logic::Knowledge>(match.tokens);
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
    if (auto whiteMatch = Angel::Parser::BNF::Whitespace().Parse(allData.c_str(), parser))
    {
        start = whiteMatch->remaining;
    }
    auto match = Parser::BNF::Parse(Angel::Parser::BNF::expression_, parser, start);
    s.seekg(-std::streamoff(match.remaining.length()), std::ios_base::cur);

    o = std::any_cast<const Logic::Object&>(match.tokens);
    return s;
}
}
}

std::ostream& operator<<(std::ostream& s, const Angel::Logic::Object& o)
{
    s << o->String();
    return s;
}



