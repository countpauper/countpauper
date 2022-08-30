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
#include "LogicParser.h"
#include "Parser/Errors.h"

namespace Angel
{
namespace Parser
{

    std::any Merge(const std::any& left, const std::any& right)
    {
        if (!right.has_value())
            return left;

        if (!left.has_value())
            return right;

        if (left.type() == typeid(std::vector<std::any>))
        {
            const auto& leftArray = std::any_cast<std::vector<std::any>>(left);
            std::vector<std::any> merged(leftArray);
            assert(right.type() == typeid(Logic::Object));  // merge two vectors not implemented yet
            merged.push_back(right);
            return merged;
        }
        else if (left.type() == typeid(Logic::Object))
        {
            if (right.type() == typeid(std::vector<std::any>))
            {
                auto rightArray= std::any_cast<std::vector<std::any>>(right);
                rightArray.insert(rightArray.begin(), left);
                return rightArray;
            }
            else
            {
                return std::vector<std::any>{left, right};
            }
        }
        else
        {
            assert(false); // What is it ?
            return left;
        }
    }


    std::any ParseId(const std::string_view value)
    {
        return Logic::id(value);
    }
    
    std::any ParseInteger(const std::string_view value)
    {
        return Logic::integer(value);
    }


    std::any ParseBoolean(const std::string_view value)
    {
        return Logic::boolean(value);
    }

    std::any ConstructBracedSequence(const std::any& tokens)
    {
        if (tokens.has_value())
        {
            auto naked_expression = std::any_cast<Logic::Object>(tokens);
            if (naked_expression.As<Logic::Sequence>())
            {   // on sequences, braces are optional since () is an empty sequence and (element) is a size 1 sequence
                return naked_expression;
            }
            else
            {    // if the naked expression is not a sequence or collection, then make it one (ie (1) = a sequence of 1
                return Logic::sequence(std::move(naked_expression));
            }
        }
        else
            return Logic::sequence();
    }

    std::any ConstructBracedExpression(const std::any& tokens)
    {
        if (!tokens.has_value())
            return Logic::sequence();

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

    std::any ConstructSequence(const std::any& tokens)
    {
        auto result = std::make_unique<Logic::Sequence>();
        if (!tokens.has_value())
        {
            // empty sequence
        }
        else if (tokens.type() == typeid(Logic::Object))
        {
            result->Add(std::any_cast<Logic::Object>(tokens));
        }
        else if (tokens.type() == typeid(std::vector<std::any>))
        {
            const auto& elements = std::any_cast<std::vector<std::any>>(tokens);
            for (const auto& e : elements)
            {
                result->Add(std::move(std::any_cast<Logic::Object>(e)));
            }
        }
        return Logic::Object(std::move(result));
    }

    std::any ConstructPredicate(const std::any& tokens)
    {
        if (!tokens.has_value())
            throw ParseException("Predicate can't be void");

        if (tokens.type() == typeid(Logic::Object))
        {
            auto id = std::any_cast<Logic::Object>(tokens);
            return Logic::predicate(*id.As<Logic::Id>());
        }
        else
        {
            const auto& elements = std::any_cast<std::vector<std::any>>(tokens);
            assert(elements.size() == 2);   // id and argument sequence
            auto e0 = std::any_cast<Logic::Object>(elements.at(0));
            auto pId = e0.As<Logic::Id>();
            auto args = std::any_cast<Logic::Object>(elements.at(1));
            if (auto pSeq = args.As<Logic::Sequence>())
            {
                return Logic::predicate(*pId, std::move(*pSeq));
            }
            else
            {
                auto seq = Logic::Sequence(std::move(args));
                return Logic::predicate(*pId, std::move(seq));
            }
        }
    }

    std::any ConstructKnowledge(const std::any& tokens)
    {
        Logic::Knowledge result;
        if (tokens.has_value())
        {
            if (tokens.type() == typeid(Logic::Object))
            {
                auto clause = std::any_cast<Logic::Object>(tokens);
                result.Know(std::move(clause));
            }
            else
            {
                auto clauses = std::any_cast<std::vector<std::any>>(tokens);
                for (auto& c : clauses)
                {
                    auto clause = std::any_cast<Logic::Object>(c);
                    result.Know(std::move(clause));

                }
            }
        }
        return result;
    }



class LogicParser : public BNF::Parser
{
    std::any ParseImpl(const std::string_view rule, const std::any& tokens, const std::string_view value) const
    {
        if (rule == "id")
        {
            return ParseId(value);
        }
        else if (rule == "integer")
        {
            return ParseInteger(value);
        }
        else if (rule == "boolean")
        {
            return ParseBoolean(value);
        }
        else if (rule == "braced sequence")
        {
            return ConstructBracedSequence(tokens);
        }
        else if (rule == "braced expression")
        {
            return ConstructBracedExpression(tokens);
        }
        else if (rule == "sequence")
        {
            return ConstructSequence(tokens);
        }
        else if (rule == "predicate")
        {
            return ConstructPredicate(tokens);
        }
        else if (rule == "knowledge")
        {
            return ConstructKnowledge(tokens);
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
/*
    std::any Parse(const std::string_view rule, const std::any& tokens, const std::string_view value) const override
    {
        std::string inType = tokens.type().name();
        if (tokens.type() == typeid(Logic::Object))
        {
            const auto& obj = std::any_cast<Logic::Object>(tokens);
            inType = obj->String();
        }
        auto result = ParseImpl(rule, tokens, value);
        std::string outType = result.type().name();
        if (result.type() == typeid(Logic::Object))
        {
            const auto& obj = std::any_cast<Logic::Object>(result);
            outType = obj->String();
        }
        return result;
    }
*/

};

Logic::Knowledge Parse(const std::string& text)
{
	Logic::Knowledge result;
    auto match = BNF::Parse(BNF::knowledge,  text.c_str());
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
    if (auto whiteMatch = Angel::Parser::BNF::Whitespace().Parse(allData.c_str()))
    {
        start = whiteMatch->remaining;
    }
    auto match = Parser::BNF::Parse(Angel::Parser::BNF::expression_, start);
    s.seekg(-std::streamoff(match.remaining.length()), std::ios_base::cur);

    if (match.tokens.has_value())
        o = std::any_cast<const Logic::Object&>(match.tokens);
    else
    {
        std::string_view parsedString = start.substr(0, start.length() - match.remaining.length());
        throw ParseException(std::string("No object parsed from '") + parsedString.data() + "'");
    }
    return s;
}
}
}

std::ostream& operator<<(std::ostream& s, const Angel::Logic::Object& o)
{
    s << o->String();
    return s;
}


