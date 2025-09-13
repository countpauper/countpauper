#include "Logic/GenericOperation.h"
#include "Logic/Expression.h"
#include <sstream>

namespace Angel::Logic
{

GenericOperation::GenericOperation()
    : Tuple()
    , ope(NewOperator::none)
{
}

GenericOperation::GenericOperation(const NewUnaryOperator& ope, const Expression& operand) 
    : Tuple({operand})
    , ope(ope)
{
}

GenericOperation::GenericOperation(const NewUnaryOperator& ope, Expression&& operand) 
    : Tuple()
    , ope(ope)
{
    Add(std::move(operand));
}

GenericOperation::GenericOperation(wchar_t tag, Expression&& operand)
    : GenericOperation(NewUnaryOperator::Find(tag), std::move(operand))
{
}

unsigned GenericOperation::Add(Expression&& operand)
{
    if (const auto* op = operand.GetIf<GenericOperation>())
    {
        if (ope.IsMultiary() && op->ope == ope) 
        {
            unsigned total = 0;
            for(auto& item: *op)
            {
                total += Add(std::move(item));
            }
            return total;
        }
    }
    return Tuple::Add(std::move(operand));    
}

unsigned GenericOperation::Add(const Expression& operand)
{
    // TODO: this can probably be optimized
    Expression copy(operand);
    return Add(std::move(copy));
}

GenericOperation& GenericOperation::operator=(const GenericOperation& o)
{
    if (o.ope != ope)
    {
        throw std::invalid_argument(std::format("Can't assign {} operation to {}", 
            o.ope.Description(), ope.Description()));
    }
    Base::operator=(o);
    return *this;
}

Expression GenericOperation::Simplify() const
{
    // TODO: Optimize moving contents into new 
    Tuple simple = Base::SimplifyItems();
    Tuple result(ope(simple));
    if (result.size()==1) 
    {
        std::cout << *this << "=" << result.front() << std::endl;
        return result.front();
    }
    else
    {
        std::cout <<  *this << "=" << std::string(ope) << " with " << result.size() << " operands " << std::endl;
        return GenericOperation(ope, std::move(result));
    }
}
Expression GenericOperation::Substitute(const Hypothesis& hypothesis) const
{
    return GenericOperation(ope, Tuple::SubstituteItems(hypothesis));
}

Expression GenericOperation::Matches(const Expression& e, const Hypothesis& hypothesis) const 
{
    return Equal{Substitute(hypothesis), e.Substitute(hypothesis)};
}

Expression GenericOperation::Infer(const class Knowledge& knowledge, const Hypothesis& hypothesis, Trace& trace) const
{
    if (ope.MinimumOperands()==0 && Base::empty())
        return GenericOperation(ope, Tuple());

    GenericOperation result(ope, Tuple{Base::front().Infer(knowledge, hypothesis, trace)});
    result.reserve(Base::size()); 

    std::transform(Base::begin()+1, Base::end(), std::back_inserter(result), [&knowledge, &hypothesis, &trace](const Expression& operand)
    {
        return operand.Infer(knowledge, hypothesis, trace);
    });
    return result;

}

const NewOperator& GenericOperation::GetOperator() const
{
    return ope;
}

std::string GenericOperation::Summary() const
{
    auto terms = size();
    if (terms==1)
        return std::format("{} with {} term", GetOperator().Description(), terms);
    else 
        return std::format("{} with {} terms", GetOperator().Description(), terms);
}    

bool GenericOperation::operator==(const GenericOperation& rhs) const
{
    return ope == rhs.ope && Tuple::operator==(rhs);
}

GenericOperation::operator bool() const
{
    return bool(ope);
}

std::size_t GenericOperation::Hash() const
{
    return Tuple::Hash() ^ ope.Hash();
}



std::string to_string(const GenericOperation& c)
{
    std::stringstream ss;
    ss << c;
    return ss.str();
}

Expression RemainingOperands(const NewOperator& ope, std::ranges::input_range auto operands)
{
    if (std::ranges::empty(operands)) 
        return Expression();
    if (std::ranges::size(operands) == 1)
        return *operands.begin();
    else 
        return GenericOperation(ope, operands);
}

GenericOperation GenericOperation::Solve(const Expression& target, Expression&& substitute) const
{
    auto it = Find(target);
    if (it == end())
        return GenericOperation();  // can't be solved, return NULL operation indicates failure 
    if (it == begin())
    {
        return ope.InvertLeft(std::move(substitute), RemainingOperands(ope, std::ranges::subrange(it+1, end())));
    }
    else if (it == end()-1)
    {
        return ope.InvertRight(RemainingOperands(ope, std::ranges::subrange(begin(), it)), std::move(substitute));
    }
    else
    {
        return ope.InvertRight(RemainingOperands(ope, std::ranges::subrange(begin(), it)), 
            ope.InvertLeft(std::move(substitute), 
                RemainingOperands(ope, std::ranges::subrange(it+1, end()))));
    }
}

void GenericOperation::AddOperand(std::string& str, const Expression& e) const
{
    if (size() == 1 && ope.Unary())
        return ope.Unary()->AddOperand(str, e);
    ope.AddOperand(str, e);
}


GenericOperation::operator std::string() const
{
    std::string result;
    for(const auto& obj: *this)
    {
        AddOperand(result, obj);
    }
    return result;
}

std::ostream& operator<<(std::ostream& os, const GenericOperation& operation)
{
    os << std::string(operation);
    return os;
}

void PrintTo(const GenericOperation& op, ::std::ostream* os)
{
    *os << op;
}

}