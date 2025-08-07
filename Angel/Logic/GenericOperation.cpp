#include "Logic/Internal/GenericOperation.h"
#include "Logic/Internal/Tuple.h"
#include "Logic/Expression.h"
#include <sstream>
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

NewOperator::operator std::string() const
{
    return UnicodeToUtf8(op.sw.unicode);
}

bool NewOperator::operator==(const NewOperator& o) const
{
    return op.id == o.op.id;
}

std::size_t NewOperator::Hash() const
{
    return op.id;
}

std::string NewOperator::OperandToString(const Expression& operand, bool first) const
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
        if (expressionOperator.Precedence() < Precedence())
            return true;
        if (!first & expressionOperator.Precedence() == Precedence())
            return true; 
    }
    if (const auto* newOperation = expression.GetIf<GenericOperation>())
    {
        const auto& expressionOperator = newOperation->GetOperator();
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

class NewUnaryOperator : public NewOperator
{
public:
    NewUnaryOperator(wchar_t c) 
        : NewOperator(c, 1) 
    {
    }
    Tuple operator()(const Tuple& operands) const override
    {
        if (operands.empty())
            throw std::invalid_argument(std::format("Missing operand for unary operator {}", std::string(*this)));
        if (operands.size()>1)
            throw std::invalid_argument(std::format("Too many operands ({}) for unary operator {}", operands.size(), std::string(*this)));
        return Tuple{operands.front()};
    }
    unsigned MinimumOperands() const override { return 1; }
    virtual Expression operator()(const Expression& operand) const = 0;
};

class NewBinaryOperator : public NewOperator 
{
public:
    NewBinaryOperator(wchar_t code) 
        : NewOperator(code, 2) 
    {
    }
    virtual Expression operator()(const Expression& lhs, const Expression& rhs) const = 0;

    Tuple operator()(const Tuple& operands) const override
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
                    if (*it == absorb)
                        return Tuple{absorb};
                }
                if (identity) 
                {
                    if (*it == identity)
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
            return Tuple{identity};
        }
        return result;
    }
    unsigned MinimumOperands() const { return identity?0:1; }
protected:
    bool comparator = false;
    Expression identity;
    Expression absorb;
};


GenericOperation::GenericOperation(const NewOperator& ope, std::initializer_list<Expression> operands) :
    FlatTuple<GenericOperation>(operands),
    ope(ope)
{
}

GenericOperation::GenericOperation(const NewOperator& ope, Tuple&& operands) :
    FlatTuple<GenericOperation>(std::move(operands)),
    ope(ope)
{
}

GenericOperation::GenericOperation(wchar_t tag, Tuple&& operands) :
    GenericOperation(NewOperator::Find(tag, false), std::move(operands))
{
}
    
GenericOperation::GenericOperation(wchar_t tag, Expression&& operand) :
    GenericOperation(NewOperator::Find(tag, true), Tuple{std::move(operand)})
{
}

GenericOperation& GenericOperation::operator=(const GenericOperation& o)
{
    if (o.ope != ope)
    {
        throw std::invalid_argument(std::format("Can't assign {} operation to {}", 
            o.ope.Description(), ope.Description()));
    }
    Tuple::operator=(o);
    return *this;
}

Expression GenericOperation::Simplify() const
{
    // TODO: Optimize moving contents into new 
    Tuple simple = Tuple::SimplifyItems();
    Tuple result(ope(simple));

    if (result.size()==1)
        return result.front();
    else
        return GenericOperation(ope, std::move(result));
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



class Sum : public NewBinaryOperator
{
public:
    Sum() : NewBinaryOperator(L'+')
    {
        identity = Integer(0);
        precedence = 50; 
        description = "add";
    }
    Expression operator()(const Expression& lhs, const Expression& rhs) const override 
    {
        if (const auto* lhNumber = lhs.GetIf<Number>())
        {
            return *lhNumber + rhs.Cast<Number>();
        }
        else if (const auto* lhContainer = lhs.GetIf<Container>())
        {
            return *lhContainer + rhs.Cast<Container>();
        }
        else 
        {
            assert(false); // possibly missing infer or shouldn't get here. Or perhaps during simplify? 
            return NewSummation({lhs, rhs});
        }
    }
    std::string OperandToString(const Expression& operand, bool first) const
    {
        std::string result = NewOperator::OperandToString(operand, first);
        if (result.size()>2 && result[1]=='-')
            result = result.substr(1);
        return result;
    }
};
static const Sum sum;

GenericOperation NewSummation(std::initializer_list<Expression> operands)
{
    return GenericOperation(sum, operands);
}

}