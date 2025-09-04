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




GenericOperation GenericOperation::Solve(const Expression& target, Expression&& substitute) const
{
    if (!ope.IsCommutative())
    {
        return SolveNonCommutative(target, std::move(substitute));
    }
    if (!ope.Inversion())
        return GenericOperation();  
    return SolveCommutative(target, std::move(substitute));
}

GenericOperation GenericOperation::SolveNonCommutative(const Expression& target, Expression&& substitute) const
{
    if (front()!=target)
    {
        GenericOperation solution = *this;
        for(auto& operand: solution)
        {
            if (operand==target && substitute)
                operand = std::move(substitute);
            else if (const auto subOperation = operand.GetIf<GenericOperation>())
            {
                auto subInversion = subOperation->Solve(target, std::move(substitute));
                if (subInversion)
                    operand = subInversion;
            }
        }
        if (substitute)
            return solution;
        else 
            return GenericOperation();        
    }
    else
    {
        return SolveCommutative(target, std::move(substitute));
    }

}

GenericOperation GenericOperation::SolveCommutative(const Expression& target, Expression&& substitute) const
{
   GenericOperation solution(ope.Inversion(), *this);
    if (solution.ope.IsCommutative())
        return solution.SolveInversionCommutative(target ,std::move(substitute));
    else 
        return solution.SolveInversionNonCommutative(target, std::move(substitute));

}

GenericOperation GenericOperation::SolveInversionCommutative(const Expression& target, Expression&& substitute)
{
    for(auto& operand: *this)
    {
        if ((operand == target) && (substitute))
        {
            operand = std::move(substitute);
        }
        else if (const auto subOperation = operand.GetIf<GenericOperation>())
        {
            auto subInversion = subOperation->Solve(target, std::move(substitute));
            if (subInversion)
                operand = std::move(subInversion);
        }
    }
    if (substitute) // substituted
        return *this;
    else
        return GenericOperation();
}

GenericOperation GenericOperation::SolveInversionNonCommutative(const Expression& target, Expression&& substitute)
{
    iterator targetIt=end();
    for(auto it=begin(); it!=end(); ++it)
    {
        if ((targetIt==end()) && (*it == target))
        {
            targetIt = it;
        }
        else if (const auto subOperation = it->GetIf<GenericOperation>())
        {
            auto subInversion = subOperation->Solve(target, std::move(substitute));
            if (subInversion)
                *it = std::move(subInversion);
        }
    }
    if (targetIt == end())
        return GenericOperation();
    erase(targetIt);
    AddAt(begin(), std::move(substitute));
    return *this;
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