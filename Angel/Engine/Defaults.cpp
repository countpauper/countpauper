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

std::string Description(const Logic::Expression& e)
{
    return std::visit(Logic::overloaded_visit{
         [&e]<Logic::IsElement T>(const T& element) -> std::string 
         {
            return std::string("= ") + Logic::to_string(e);
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
        [&e](const Logic::Predicate& p) -> std::string
        {
            return std::format("if {}", Logic::to_string(e));
        },
        [&e](const auto& obj) 
        {
            return std::string("TODO Description for ")+Logic::to_string(e);
        }
    }, e);
}

Logic::Object Help(const Logic::Knowledge& k, const Logic::Variables& vars)
{
    if (!vars.empty())
        return Logic::Boolean(false);   // help($F) not yet implemented, todo: should throw Angel exception
    auto root = k.Root();
    if (root.empty())
        return Logic::String("The namespace is empty");

    std::stringstream ss; 
    for(const auto& know : root)
    {
        ss << know.first << " " << Description(know.second) << std::endl;
    }
    return Logic::String(ss.str());
}

Logic::Object Delete(const Logic::Knowledge& k, const Logic::Variables& var)
{
    // TODO: implement, also how if knowledge is constant. 
    // this is likely one of the view functions that will change the knowledge besides : 
    // import and parse(<coude> )
    return Logic::Boolean(false);
}

void AddDefaults(Logic::Knowledge& knowledge)
{
    knowledge.Know(Logic::Predicate("help"), Logic::Function(Help, 
        "List contents of the root namespace."));
    knowledge.Know(Logic::Predicate("help", {Logic::Variable("topic")}), Logic::Function(Help, 
        "Describe the function matching the $topic."));
    knowledge.Know(Logic::Predicate("delete", {Logic::Variable("predicate")}), Logic::Function(Delete, 
        "Remove all the functions matching the $predicate."));
}

}