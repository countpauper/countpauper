#include "Engine/Defaults.h"
#include "Logic/Knowledge.h"
#include "Logic/Expression.h"
#include "Logic/Hypothesis.h"
#include "Logic/Predicate.h"
#include "Logic/Function.h"
#include "Logic/String.h"
#include "Logic/All.h"
#include "Logic/Element.h"
#include "Logic/Operator.h"
#include "Logic/Trace.h"
#include <cassert>
#include <sstream>

namespace Angel::Engine
{


Logic::String HelpList(const Logic::Knowledge& k)
{
    auto root = k.Root();
    if (root.empty())
        return Logic::String("The namespace is empty");

    std::stringstream ss; 
    for(const auto& know : root)
    {
        ss << know.Summary() << std::endl;
    }
    return Logic::String(ss.str());
}

Logic::Predicate MakeSignature(const Logic::Id& id, Logic::Hypothesis args)
{
    Logic::Variable var("args");
    Logic::List arglist = var.Substitute(args).Get<Logic::List>();
    return Logic::Predicate(id, std::move(arglist));
}

Logic::String HelpTopic(const Logic::Knowledge& k, const Logic::Id& id)
{
    auto matches = k.Matches(Logic::Predicate(id, {Logic::All("args")}));
    if (matches.empty())
    {
        throw std::runtime_error(std::format("Unknown topic {}", Logic::to_string(id)));
    }
    std::stringstream ss; 
    for(const auto& match: matches)
    {
        if (const auto* antecedent_args = match.GetIf<Logic::Association>())
        {
            auto predicate = MakeSignature(id, antecedent_args->Right().Get<Logic::Conjunction>());
            ss << predicate << "\t" << antecedent_args->Left().Documentation() << std::endl;
        }
        else 
        {
            ss << Logic::Predicate(id) << "\t" << match.Documentation() << std::endl;
        }
    }
    return Logic::String(ss.str());
}

Logic::Expression GetArg(const Logic::Hypothesis& args, const std::string_view name)
{
    Logic::Variable arg(name);
    auto value = arg.Substitute(args);
    if (value == arg)
    {
        return Logic::Expression();
    }
    else 
    {
        return value;
    }
}

Logic::Expression Help(const Logic::Knowledge& k, const Logic::Hypothesis& args, Logic::Trace& trace)
{
    Logic::Expression topic = GetArg(args, "topic");
    if (topic)
    {
        Logic::Id id = topic.Get<Logic::Id>();
        return HelpTopic(k, id);
    } 
    else 
    {
        return HelpList(k);
    }
}

Logic::Expression Delete(const Logic::Knowledge& k, const Logic::Hypothesis& args, Logic::Trace& trace)
{
    Logic::Expression target = GetArg(args, "id");
    if (target)
    {
        Logic::Knowledge& readAccess = k.Lock();
        return Logic::Integer(readAccess.Forget(target.Cast<Logic::Predicate>()));
    }
    else 
    {
        throw std::invalid_argument("Delete must provide an id");
    }
    return Logic::Integer(0);
}

Logic::Expression Print(const Logic::Knowledge& k, const Logic::Hypothesis& args, Logic::Trace& trace)
{
    Logic::Expression target = GetArg(args, "omething");
    std::cout << target.Infer(k, args, trace);
    return Logic::Boolean(true);
}


Logic::Expression Trace(const Logic::Knowledge& k, const Logic::Hypothesis& args, Logic::Trace& trace)
{
    trace.SetVerbosity(1);
    return Logic::Boolean(true);
}


void AddDefaults(Logic::Knowledge& knowledge)
{
    knowledge.Know(Logic::Association{Logic::Predicate("help"), Logic::Function(Help, 
        "List all functions.")});
    knowledge.Know(Logic::Association{Logic::Predicate("help", {Logic::Variable("topic")}), Logic::Function(Help, 
        "Describe the function matching the $topic.")});
    knowledge.Know(Logic::Association{Logic::Predicate("delete", {Logic::Variable("id")}), Logic::Function(Delete, 
        "Remove all the functions matching the $id.")});
    knowledge.Know(Logic::Association{Logic::Predicate("print", {Logic::Variable("omething")}), Logic::Function(Print, 
        "Write $omething to the standard output immediately.")});
    knowledge.Know(Logic::Association{Logic::Predicate("trace"), Logic::Function(Trace, 
        "Trace whenever any predicate is inferred with the arguments and hypothesis.")});        
    knowledge.Know(Logic::Association{Logic::Predicate("trace", {Logic::Variable("predicate")}), Logic::Function(Trace, 
        "Print whenever a matching predicate is inferred with the arguments and hypothesis.")});        
}

}