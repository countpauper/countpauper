#include "Logic/Internal/GenericOperation.h"
#include "Logic/Internal/Tuple.h"
#include "Logic/Expression.h"
#include <iostream>
#include <string>
#include <sstream>
#include <codecvt>

namespace Angel::Logic
{


std::string UnicodeToUtf8(wchar_t tag)
{
    static std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>, wchar_t> wchar_conv;
    wchar_t wstr[2] { tag, 0};
    return wchar_conv.to_bytes(wstr);
}

class NoneOperator : public NewOperator
{
public:
    unsigned MinimumOperands() const override
    {
        return 0;
    }
    Tuple operator()(const Tuple& operands) const override
    {
        return Tuple();
    }
    std::string OperandToString(const Expression& operand, bool first) const 
    {
        return "null";
    }
};

static NoneOperator noneOperator;

NewOperator::NewOperator()
    : op{.id=0}
    , inversion(&noneOperator)
{
}

NewOperator::operator std::string() const
{
    return UnicodeToUtf8(op.sw.unicode);
}

bool NewOperator::operator==(const NewOperator& o) const
{
    return op.id == o.op.id;
}

NewOperator::operator bool() const
{
    return op.id!=0;
}

std::size_t NewOperator::Hash() const
{
    return op.id;
}

unsigned NewOperator::Operands() const 
{ 
    return op.sw.operands; 
}

bool NewOperator::IsPostfix() const 
{ 
    return op.sw.postfix; 
}

const NewOperator& NewOperator::Inversion() const
{
    if (inversion)
        return *inversion;
    else
        return noneOperator;
}

std::string_view NewOperator::Description() const
{
    if (!description.empty())
        return description;
    else
        return UnicodeToUtf8(op.sw.unicode);
}

unsigned NewOperator::Precedence() const
{
    return precedence;
}

bool NewOperator::NeedsBracesAround(const Expression& expression, bool first) const
{
    if (const auto* operation = expression.GetIf<Operation>())
    {
        auto expressionOperator = operation->GetOperator();
        if (expressionOperator.Operands()>1 && operation->size()<2)
            return false;
        if (expressionOperator.Precedence() < Precedence())
            return true;
        if (!first & expressionOperator.Precedence() == Precedence())
            return true; 
    }
    if (const auto* newOperation = expression.GetIf<GenericOperation>())
    {
        const auto& expressionOperator = newOperation->GetOperator();
        if (expressionOperator.Operands()>1 && newOperation->size()<2)
            return false;
        if (expressionOperator.Precedence() < Precedence())
            return true;
        if (!first & expressionOperator.Precedence() == Precedence())
            return true; 
    }
    return false;
}

const NewOperator& NewOperator::Find(wchar_t tag, bool unary)
{
    if (unary)
    {
        auto it = all.find(Code{.sw{.unicode=tag, .operands=1, .postfix=false}});
        if (it!= all.end())
            return *it->second;
        it = all.find(Code{.sw{.unicode=tag, .operands=1, .postfix=true}});
        if (it!= all.end())
            return *it->second;
        throw std::invalid_argument(std::format("Unknown unary operator {}", UnicodeToUtf8(tag) ));
    } 
    else 
    {
        auto it = all.find(Code{.sw{.unicode=tag, .operands=2, .comparison=false}});
        if (it!= all.end())
            return *it->second;
        it = all.find(Code{.sw{.unicode=tag, .operands=3, .comparison=false}});
        if (it!= all.end())
            return *it->second;
        it = all.find(Code{.sw{.unicode=tag, .operands=2, .comparison=true}});
        if (it!= all.end())
            return *it->second;
        it = all.find(Code{.sw{.unicode=tag, .operands=3, .comparison=true}});
        if (it!= all.end())
            return *it->second;   
        throw std::invalid_argument(std::format("Unknown binary operator {}", UnicodeToUtf8(tag) ));                    
    }
}


std::ostream& operator<<(std::ostream& os, const NewOperator& op)
{
    os << std::string(op);
    return os;
}

std::map<NewOperator::Code, NewOperator*> NewOperator::all;

NewUnaryOperator::NewUnaryOperator(wchar_t c) 
    : NewOperator(c, 1) 
{
}
Tuple NewUnaryOperator::operator()(const Tuple& operands) const
{
    if (operands.empty())
        throw std::invalid_argument(std::format("Missing operand for unary operator {}", std::string(*this)));
    if (operands.size()>1)
        throw std::invalid_argument(std::format("Too many operands ({}) for unary operator {}", operands.size(), std::string(*this)));
    return Tuple{(*this)(operands.front())};
}

unsigned NewUnaryOperator::MinimumOperands() const 
{ 
    return 1; 
}


std::string NewUnaryOperator::OperandToString(const Expression& operand, bool) const
{
    std::string result;
    if (NeedsBracesAround(operand, op.sw.postfix))
        result = std::format("({})", to_string(operand));
    else 
        result = to_string(operand);
    if (op.sw.postfix)
    {
        return result + UnicodeToUtf8(op.sw.unicode);
    }
    else 
    {
        return UnicodeToUtf8(op.sw.unicode) + result;
    }    
    return result;
}



NewBinaryOperator::NewBinaryOperator(wchar_t code, bool multiary) 
    : NewOperator(code, multiary?3:2) 
{
}

NewBinaryOperator::~NewBinaryOperator() = default;

Tuple NewBinaryOperator::operator()(const Tuple& operands) const
{
    Expression constant;
    Tuple result(operands);
    auto it = result.begin();
    while(it!=result.end())
    {
        if (it->IsConstant())
        {
            if (absorb)
            {
                if (*it == *absorb)
                    return Tuple{*absorb};
            }
            if (identity) 
            {
                if (*it == *identity)
                {
                    it = result.erase(it);
                    continue;
                }
            }
            if (constant)
                constant = (*this)(constant, *it);
            else
                constant = *it;
            it = result.erase(it);
        }
        else 
            ++it;
    }
    if (constant) 
    {
        auto begin = result.cbegin();
        result.AddAt(begin, std::move(constant));
    }
    if (identity && result.empty())
    {
        return Tuple{*identity};
    }
    return result;
}

unsigned NewBinaryOperator::MinimumOperands() const 
{ 
    return identity?0:1;
}


std::string NewBinaryOperator::OperandToString(const Expression& operand, bool first) const
{
    std::string result;
    if (!first)
        result += UnicodeToUtf8(op.sw.unicode);
    
    if (NeedsBracesAround(operand, first))
        result += std::format("({})", to_string(operand));
    else 
        result += to_string(operand);
    return result;
}

GenericOperation::GenericOperation() :
    Tuple(),
    ope(noneOperator)
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