#include "Logic/Expression.h"
#include "Logic/Boolean.h"
#include "Logic/Integer.h"
#include "Logic/Id.h"
#include "Logic/Expression.h"
#include "Logic/Knowledge.h"
#include <sstream>
#include <cassert>

namespace Angel::Logic
{

Expression::Expression(const Expression& e) :
    ExpressionVariant(e)
{
}

Match Expression::Matches(const Expression& e, const Variables& subs) const 
{
    return std::visit(overloaded_visit{
        [&e, &subs, this]<IsElement T>(const T& element) -> Match {
            const auto* var = std::get_if<Variable>(&e);
            // TODO this could be Variable::Matches()
            if (var)
                return var->Matches(*this, subs);
            if (e==element)
                return IsMatch;            
            return NoMatch; 
        },
        [&e, &subs](const auto& obj) -> Match 
        {
            return obj.Matches(e, subs);
        }},   *this);    
}

Object Expression::Infer(const class Knowledge& knowledge, const Variables& vars) const
{
    return std::visit(overloaded_visit{
        []<IsElement T>(const T& element) -> Object {
            return element; 
        },
        [&knowledge, &vars](const auto& obj) -> Object {
            return obj.Infer(knowledge, vars);
        }
    }, *this);
}

bool Expression::operator<(const Expression& e) const
{
    std::hash<Expression> hasher; 
    return hasher(*this) < hasher(e);
}

std::ostream& operator<<(std::ostream& s, const Expression& e)
{
    std::visit([&s](const auto& obj)
    {
        s << obj;
    }, e);
    return s;
}

std::string to_string(const Expression& e)
{
    std::stringstream ss;
    ss << e;
    return ss.str();
}

template<>
const std::optional<Predicate> Expression::TryCast<Predicate>() const
{
    auto same = std::get_if<Predicate>(this);  // TODO really cast 
    if (same)
        return std::optional<Predicate>(*same);

    return std::visit(overloaded_visit{
        [](const Id& id) { return std::optional<Predicate>(Predicate(id)); },
        [](const auto&) { return std::optional<Predicate>(); }
    }, *this);
}

template<>
const std::optional<Integer> Expression::TryCast<Integer>() const
{
    auto same = std::get_if<Integer>(this);
    if (same)
        return std::optional<Integer>(*same);

    return std::visit(overloaded_visit{
        [](const Boolean& b) { return std::optional<Integer>(b?1:0); },
        [](const auto&) { return std::optional<Integer>(); }
    }, *this);
}


}


namespace std
{

size_t hash<Angel::Logic::Expression>::operator()(const Angel::Logic::Expression& e) const
{
    return std::visit([](const auto& obj) { return obj.Hash(); }, e);
}


}
