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
    std::string OperandToString(const Expression& operand, bool first) const 
    {
        return "null";
    }
};

static NoneOperator noneOperator;
const NewOperator& NewOperator::none = noneOperator;

NewOperator::NewOperator()
    : op{.id=0}
    , inverse(&none)
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

bool NewOperator::IsCommutative() const 
{
    return commutative;
}

const NewOperator& NewOperator::Inversion() const
{
    if (inverse)
        return *inverse;
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

void NewOperator::SelfInvertible()
{
    inverse = this;
}

bool NewOperator::SetInvertible(wchar_t inversion)
{
    auto inv_op=all.find(Code{.sw{inversion, op.sw.operands, IsPostfix(), IsComparator(), 0, 0}});    
    if (inv_op==all.end())
        inv_op=all.find(Code{.sw{inversion, op.sw.operands, !IsPostfix(), IsComparator(), 0, 0}});    

    // it's not an error if not found. One of the inversions is constructed first. 
    // if not found postpone and the inversion will make the circular reference
    if (inv_op==all.end())
        return false;
    
    inverse = inv_op->second;
    inv_op->second->inverse = this;
    return true;
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
    commutative = true;
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
        result.AddAt(result.cbegin(), std::move(constant));
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

}