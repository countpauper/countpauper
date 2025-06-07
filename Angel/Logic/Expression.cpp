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
ExpressionVariant make_unary_operation(const Operator ope, Expression&& operand) 
{
    if (!ope)
        return std::move(operand);
        
    Logic::Expression result = Logic::Boolean(false);
    bool found = false;
    
    ((!found && (found = (Ts::ope == ope)) 
        ? (result.emplace<Ts>(std::move(operand)), true) 
        : false), ...);
    
    if (!found) throw std::invalid_argument(std::format("Invalid unary operator {}", std::string(ope)));
    return result;
}

template <typename... Ts>
ExpressionVariant make_binary_operation(const Operator ope, Collection&& operands) 
{
    Logic::Expression result = Logic::Boolean(false);
    bool found = false;
    
    ((!found && (found = (Ts::ope == ope)) 
        ? (result.emplace<Ts>(std::move(operands)), true) 
        : false), ...);
    
    if (!found) throw std::invalid_argument(std::format("Invalid binary operator {}", std::string(ope)));
    return result;
}

ExpressionVariant make_operation(const Operator ope, Collection&& operands)
{
    if (operands.empty())
        throw std::invalid_argument(std::format("Not enough operands for operator {}", std::string(ope)));
    if (operands.size()==1) 
        return make_unary_operation<Negative>(ope, std::move(operands[0]));
    else 
        return make_binary_operation<Summation, Subtraction, Disjunction, Conjunction, 
            Equation, Lesser, LesserEqual,Greater, GreaterEqual>(ope, std::move(operands));
}


Expression::Expression(const Operator ope, Collection&& operands) : 
    ExpressionVariant(make_operation(ope, std::move(operands)))
{
}

Expression& Expression::operator=(const Expression& e)
{
    ExpressionVariant::operator=(e);
    return *this;
}

template <std::size_t idx>
constexpr Expression CastIterate(const Expression&e, const std::type_info& rtt) 
{
    if constexpr (idx < std::variant_size_v<ElementVariant>)
    {
        if (typeid(std::variant_alternative_t<idx, ElementVariant>) == rtt)
        {
            return e.Cast<std::variant_alternative_t<idx, ElementVariant>>();
        }
        else 
        {
            return CastIterate<idx+1>(e, rtt);
        }
    }
    return Expression();
}

Expression Expression::TryCast(const std::type_info& rtt) const
{
    return CastIterate<0>(*this, rtt);
}

Expression Expression::Cast(const std::type_info& rtt) const
{
    Expression result = CastIterate<0>(*this, rtt);
    if (result) 
        return result;
    else 
        throw CastException(AlternativeTypeId(), rtt); 
}

Expression Expression::Simplify() const
{
    return std::visit(overloaded_visit{
        [](std::monostate)  
        {
            return Expression();
        },
        [this]<IsElement T>(const T& element)   -> Expression 
        {
            return *this;
        },
        [this](const Function& f) -> Expression 
        {
            return *this;
        },
        [this](const auto& obj)
        {
            return obj.Simplify();
        }}, *this);      
}

std::size_t Expression::Assumptions() const
{
    return std::visit(overloaded_visit{
        [](std::monostate)
        {
            return 0ULL;
        },
        [this]<IsElement T>(const T& element)
        {
            return 0ULL;
        },
        [this](const Function&)
        {
            return 0ULL;
        },
        [this](const Variable&)
        {
            return 1ULL;
        },
        [this](const Tuple&) 
        {
            return 1ULL;
        },
        [this](const auto& obj)
        {
            return obj.Assumptions();
        }}, *this);

}


Expression Expression::Substitute(const Hypothesis& hypothesis) const
{
    return std::visit(overloaded_visit{
        [](std::monostate)  
        {
            return Expression();
        },
        [this]<IsElement T>(const T& element)   -> Expression 
        {
            return *this;
        },
        [this](const Function& f) -> Expression 
        {
            return *this;
        },
        [&hypothesis](const auto& obj) -> Expression
        {
            return obj.Substitute(hypothesis);
        }},   *this);      
}


Expression Expression::Matches(const Expression& e, const Hypothesis& hypothesis) const 
{
    return std::visit(overloaded_visit{
        [](std::monostate) -> Expression 
        {
            assert(false);  // should never be matching against null-expression
            return Boolean(false);
        },
        [&e, &hypothesis, this]<IsElement T>(const T& element) -> Expression 
        {
            const auto* var = e.GetIf<Variable>();
            if (var)
                return var->Matches(*this, hypothesis, true);
            if (e==element)
                return Boolean(true);            
            return Boolean(false); 
        },
        [&e, &hypothesis](const auto& obj) 
        {
            return obj.Matches(e, hypothesis);
        }},   *this);    
}

Expression Expression::Infer(const class Knowledge& knowledge, const Hypothesis& hypothesis, Trace& trace) const
{
    return std::visit(overloaded_visit{
        [this](std::monostate)
        {
            return *this;
        },
        []<IsElement T>(const T& element) -> Expression {
            return element; 
        },
        [&knowledge, &hypothesis, &trace](const auto& obj) -> Expression {
            return obj.Infer(knowledge, hypothesis, trace);
        }
    }, *this);
}

Expression::operator bool() const
{
    static_assert(std::is_same_v<std::variant_alternative_t<0, ExpressionVariant>, std::monostate>);
    return index() != 0;
}

bool Expression::operator==(const Expression& rhs) const
{
    return std::visit(overloaded_visit{
        [this](std::monostate)
        {
            return !bool(*this);
        },
        [this](const auto& rho)
        {
            return operator==(rho);
        }
        }, rhs);
}

bool Expression::operator<(const Expression& e) const
{
    std::hash<Expression> hasher; 
    return hasher(*this) < hasher(e);
}

const std::type_info& Expression::AlternativeTypeId() const 
{
    return *std::visit([](const auto& obj)
    {
        return &typeid(decltype(obj)); 
    }, *this);        
}
std::string Expression::Summary() const
{
    return std::visit(Logic::overloaded_visit{
         [this]<Logic::IsElement T>(const T& element) -> std::string 
         {
            return Logic::to_string(*this);
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
            return std::format("{} : {}", a.Left().Summary(), a.Right().Summary());
        },
        [this](const Logic::Predicate& p) -> std::string
        {
            return Logic::to_string(*this);
        },
        [](const Logic::Variable& v) -> std::string
        {
            return std::format("${}", v.Name());
        },
        [this](const auto& obj) 
        {
            return std::string("TODO Description for ")+Logic::to_string(*this);
        }
    }, *this);
}

std::string Expression::Documentation() const
{
    return std::visit(Logic::overloaded_visit{
        [](const Logic::Function& fn)
        {
            return std::string(fn.Documentation());
        },
        [this](const auto& obj) 
        {
            return Logic::to_string(*this);
        }
    }, *this);
}

std::ostream& operator<<(std::ostream& s, const Expression& e)
{
    std::visit(overloaded_visit{
        [&s](std::monostate)
        {
            s << "null";
        },   
        [&s](const auto& obj)
        {
            s << obj;
        } 
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
    return std::visit(Angel::Logic::overloaded_visit{
        [](std::monostate mono) -> size_t 
        { 
            std::hash<std::monostate> hasher;
            return hasher(mono);
        }, 
        [](const auto& obj) { return obj.Hash(); } 
    },  e);
}


}
