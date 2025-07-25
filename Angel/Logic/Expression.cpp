#include "Logic/Expression.h"
#include "Logic/Boolean.h"
#include "Logic/Integer.h"
#include "Logic/Id.h"
#include "Logic/Knowledge.h"
#include <sstream>
#include <cassert>

namespace Angel::Logic
{

Expression::Expression(const Expression& e) :
    ExpressionVariant(e)
{
}

Expression& Expression::operator=(const Expression& e)
{
    ExpressionVariant::operator=(e);
    return *this;
}

Operator Expression::GetOperator() const
{
    return std::visit(overloaded_visit{
        [](const Operation& operation) -> Operator 
        {
            return operation.GetOperator();
        },
        [](const Ordering& ordering) -> Operator 
        {
            return ordering.GetComparator();
        },        
        [this](const auto& obj)
        {
            return Operator();
        }}, *this);   
}

bool Expression::IsComparison() const 
{
    return Is<Ordering>();
}

std::size_t Expression::size() const
{
    return std::visit(overloaded_visit{
        [this](const Container& container)
        {
            return container.size();
        },
        [this](const Operation& operation)
        {
            return operation.size();
        },  
        [this](const Ordering& ordering)
        {
            return ordering.size();
        },                     
        [this](const auto& obj)
        {
            return 0ULL;
        }}, *this);   
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

const std::type_info& Expression::AlternativeTypeId() const 
{
    return *std::visit(overloaded_visit{
        [this]<is_variant T>(const T& obj)
        {
            return &obj.AlternativeTypeId();
        },
        [this](const auto& obj)
        {
            return &ExpressionVariant::AlternativeTypeId();
        }}, *this);   
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
        [this](const auto& obj) -> Expression
        {
            return obj.Simplify();
        }}, *this);      
}

Set Expression::Assumptions() const
{
    return std::visit(overloaded_visit{
        [](std::monostate)
        {
            return Set{};
        },
        [this]<IsElement T>(const T&)
        {
            return Set{};
        },        
        [this](const Function&)
        {
            return Set{};
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
        [this]<IsElement T>(const T& element) -> Expression 
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
            return False;
        },
        [&e, &hypothesis, this]<IsElement T>(const T& element) -> Expression 
        {
            const auto* var = e.GetIf<Variable>();
            if (var)
                return var->Matches(*this, hypothesis, true);
            if (e==element)
                return True;            
            return False; 
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
        []<IsElement T>(const T& element) -> Expression 
        {
            return element; 
        },
        [](const Number& number) -> Expression 
        {
            return number;
        },
        [&knowledge, &hypothesis, &trace](const auto& obj) -> Expression {
            return obj.Infer(knowledge, hypothesis, trace);
        }
    }, *this);
}

Expression::operator bool() const
{
    return !std::holds_alternative<std::monostate>(*this);
}

bool Expression::operator<(const Expression& e) const
{
    std::hash<Expression> hasher; 
    return hasher(*this) < hasher(e);
}

std::string Expression::Summary() const
{
    return std::visit(Logic::overloaded_visit{
        [this]<Logic::IsElement T>(const T& element) -> std::string 
        {
            return Logic::to_string(*this);
        },
        [this](const Number& number) 
        {
            return Logic::to_string(number);
        },
        [](const Logic::Function& fn)
        {
            return std::string(fn.ShortDescription());
        },
        [](const Operation& operation) -> std::string 
        {
            return operation.Summary(); 
        },
        [](const Ordering& ordering) -> std::string 
        {
            return ordering.Summary();
        },          
        [](const Container& container) -> std::string 
        {
            return container.Summary();
        },
        [](const Logic::Association& a) -> std::string
        {
            return std::format("{} : {}", a.Left().Summary(), a.Right().Summary());
        },
        [this](const Logic::Predicate& p) -> std::string
        {
            return Logic::to_string(*this);
        },
        [this](const Logic::Variable& v) -> std::string
        {
             return Logic::to_string(*this);
        },
        [this](const auto& obj) 
        {
            return std::string("Missing Description for ")+Logic::to_string(*this);
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
        [](const auto& obj) 
        { 
            return obj.Hash(); 
        } 
    },  e);
}


}
