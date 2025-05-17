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


template <typename... Ts>
ExpressionVariant make_operation(const std::string_view ope, Collection&& operands) 
{
    if (ope.empty())
    {
        assert(operands.size()==1);
        return operands.front();        
    }
    Logic::Expression result = Logic::Boolean(false);
    bool found = false;
    
    ((!found && (found = (Ts::ope == ope)) 
        ? (result.emplace<Ts>(std::move(operands)), true) 
        : false), ...);
    
    if (!found) throw std::invalid_argument(std::format("Invalid operator {}", ope));
    return result;
}


Expression::Expression(const std::string_view ope, Collection&& operands) : 
    ExpressionVariant(make_operation<Summation, Disjunction, Conjunction>(ope, std::move(operands)))
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

}


namespace std
{

size_t hash<Angel::Logic::Expression>::operator()(const Angel::Logic::Expression& e) const
{
    return std::visit([](const auto& obj) { return obj.Hash(); }, e);
}


}
