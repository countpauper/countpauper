#include "Engine/Defaults.h"
#include "Logic/Knowledge.h"
#include "Logic/Expression.h"
#include "Logic/Match.h"
#include "Logic/Predicate.h"
#include "Logic/Function.h"
#include "Logic/String.h"
#include "Logic/VariantUtils.h"
#include "Logic/Element.h"
#include "Logic/Operator.h"
#include <cassert>
#include <sstream>

namespace Angel::Engine
{

std::string Summary(const Logic::Expression& e)
{
    return std::visit(Logic::overloaded_visit{
         [&e]<Logic::IsElement T>(const T& element) -> std::string 
         {
            return Logic::to_string(e);
        },
        [](const Logic::Function& fn)
        {
            return std::string(fn.ShortDescription());
        },
        []<Logic::IsOperation Op>(const Op& operation) -> std::string 
        {
            return std::format("{} with {} terms", operation.ope.Description(), operation.size()); 
        },
        // TODO (before collection) all operators if they have an ope
        []<Logic::IsCollection C>(const C& collection) -> std::string 
        {
            return std::format("{} {} collection {} items", 
                C::unique?"unique":"non-unique",
                C::ordered?"ordered":"unordered", 
                collection.size()); 
        },
        [](const Logic::Association& a) -> std::string
        {
            return std::format("{} : {}", Summary(a.Left()), Summary(a.Right()));
        },
        [&e](const Logic::Predicate& p) -> std::string
        {
            return Logic::to_string(e);
        },
        [&e](const Logic::Variable& v) -> std::string
        {
            return std::format("${}", v.Name());
        },
        [&e](const auto& obj) 
        {
            return std::string("TODO Description for ")+Logic::to_string(e);
        }
    }, e);
}


std::string Description(const Logic::Expression& e)
{
    return std::visit(Logic::overloaded_visit{
        [](const Logic::Function& fn)
        {
            return std::string(fn.Documentation());
        },
        [&e](const auto& obj) 
        {
            return Logic::to_string(e);
        }
    }, e);
}

Logic::Expression Help(const Logic::Knowledge& k, const Logic::Variables& vars)
{
    Logic::Variable var("topic");
    auto topic = var.Infer(k, vars);
    if (topic == var)
    {
        auto root = k.Root();
        if (root.empty())
            return Logic::String("The namespace is empty");

        std::stringstream ss; 
        for(const auto& know : root)
        {
            ss << Summary(know) << std::endl;
        }
        return Logic::String(ss.str());
    } 
    else 
    {
        Logic::Predicate predicate = topic.Cast<Logic::Predicate>();
        auto matches = k.Matches(predicate);
        if (matches.empty()) 
        {
            throw std::runtime_error(std::format("Unknown topic {}", Logic::to_string(topic)));
        }
        std::stringstream ss; 
        for(const auto& match: matches)
        {
            ss << Description(match) << std::endl;
        }
        return Logic::String(ss.str());
    }
}

Logic::Expression Delete(const Logic::Knowledge& k, const Logic::Variables& var)
{
    // TODO: implement, also how if knowledge is constant. 
    // this is likely one of the view functions that will change the knowledge besides : 
    // import and parse(<coude> )
    return Logic::Boolean(false);
}

void AddDefaults(Logic::Knowledge& knowledge)
{
    knowledge.Know(Logic::Association{Logic::Predicate("help"), Logic::Function(Help, 
        "You're looking at it.")});
    knowledge.Know(Logic::Association{Logic::Predicate("help", {Logic::Variable("topic")}), Logic::Function(Help, 
        "Describe the function matching the $topic.")});
    knowledge.Know(Logic::Association{Logic::Predicate("delete", {Logic::Variable("predicate")}), Logic::Function(Delete, 
        "Remove all the functions matching the $predicate.")});
}

}