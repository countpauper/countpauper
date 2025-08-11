#include "Logic/GenericOperation.h"
#include "Logic/Expression.h"
#include <sstream>

namespace Angel::Logic
{

GenericOperation::GenericOperation() :
    Tuple(),
    ope(NewOperator::none)
{
}


GenericOperation::GenericOperation(const NewOperator& ope, std::initializer_list<Expression> operands) :
    Tuple(),
    ope(ope)
{
    for(auto operand: operands)
    {
        Add(std::move(operand));
    }
}

GenericOperation::GenericOperation(const NewOperator& ope, Tuple&& operands) :
    Tuple(),
    ope(ope)
{
    for(auto&& operand: operands)
    {
        Add(std::move(operand));
    }  
}

GenericOperation::GenericOperation(const NewUnaryOperator& ope, Expression&& operand) :
    Tuple(),
    ope(ope)
{
    Add(std::move(operand));
}

GenericOperation::GenericOperation(wchar_t tag, Tuple&& operands) :
    GenericOperation(NewOperator::Find(tag, false), std::move(operands))
{
}
    
GenericOperation::GenericOperation(wchar_t tag, Expression&& operand) :
    GenericOperation(NewOperator::Find(tag, true), Tuple{std::move(operand)})
{
}

unsigned GenericOperation::Add(Expression&& operand)
{
    if (const auto* op = operand.GetIf<GenericOperation>())
    {
        if (ope.Operands() == 3 && op->ope == ope) 
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
        return GenericOperation(ope, {});

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

GenericOperation GenericOperation::Solve(const Expression& target, const Expression& substitute) const
{
    if (!ope.Inversion())
        return GenericOperation();
    GenericOperation inversion(ope.Inversion(), {});
    for(const auto& operand: *this)
    {
        if (operand == target)
        {
            inversion.Add(substitute);
        }
        else if (const auto subOperation = operand.GetIf<GenericOperation>())
        {
            if (subOperation->Assumptions().Get(target)!=False)
            {
                auto subInversion = subOperation->Solve(target, substitute);
                if (subInversion)
                    inversion.Add(std::move(subInversion));
                else
                    inversion.Add(operand);
            }
            else 
            {
                inversion.Add(operand);
            }
        }
        else 
        {
            inversion.Add(operand);
        }
    }
    return inversion;
}


std::string GenericOperation::OperandToString(const Expression& e, bool first) const
{
    return ope.OperandToString(e, first);
}

std::ostream& operator<<(std::ostream& os, const GenericOperation& operation)
{
    bool first = true;
    for(const auto& obj: operation)
    {
        os << operation.OperandToString(obj, first);
        first = false;
    }
    return os;
}

}