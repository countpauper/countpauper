#include "Logic/NewOperator.h"
#include "Logic/Expression.h"
#include <string>
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
    void AddOperand(std::string& str, const Expression& operand) const
    {
    }

    const NewUnaryOperator* Unary() const override
    {
        return nullptr;
    }

    GenericOperation InvertLeft(Expression&&, Expression&&) const 
    {
        return GenericOperation();
    }
    GenericOperation InvertRight(Expression&&, Expression&&) const 
    {
        return GenericOperation();
    }
};

static NoneOperator noneOperator;
const NewOperator& NewOperator::none = noneOperator;

NewOperator::NewOperator()
    : op{.id=0}
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


bool NewOperator::IsUnary() const
{
    return op.sw.operands == 1;
}

bool NewOperator::IsMultiary() const
{
    return op.sw.operands == 3;
}

bool NewOperator::IsPostfix() const 
{ 
    return op.sw.postfix; 
}

bool NewOperator::IsComparator() const
{
    return op.sw.comparison;
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
        if (!expressionOperator.IsUnary() && newOperation->size()<2)
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

}