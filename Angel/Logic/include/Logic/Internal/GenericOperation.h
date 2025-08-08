#pragma once

#include "Logic/Internal/FlatTuple.h"
#include "Logic/Operator.h"
#include <initializer_list>
#include <ranges>
#include <map>
#include <format>

namespace Angel::Logic
{
class Expression;


class NewOperator
{
public:
    virtual ~NewOperator() = default;
    operator std::string() const;
    bool operator==(const NewOperator& o) const;
    std::string_view Description() const;
    bool NeedsBracesAround(const Expression& expresison, bool first) const;
    unsigned Precedence() const;    
    virtual unsigned MinimumOperands() const = 0;
    virtual Tuple operator()(const Tuple& operands) const = 0;
    std::size_t Hash() const;
    virtual std::string OperandToString(const Expression& operand, bool first) const;
    static const NewOperator& Find(wchar_t tag, bool unary);
protected:
    constexpr NewOperator(const wchar_t tag, uint8_t operands, bool postfix=false, bool comparator=false) :
        op{.sw{tag, operands, postfix, comparator, 0, 0}}
    {
        all.insert(std::make_pair(op, this));
    }
protected:
    union Code
    {
        uint32_t id;
        struct __attribute__ ((packed)) 
        {
            wchar_t unicode; // 1 unicode char in the Basic Multilingual Plane.
            uint8_t operands:2; // noperator 0, unary, binary for pairs, 3 for multiary  
            bool postfix:1 = false;
            bool comparison:1 = false;
            uint8_t reserved:4 = 0;
            uint8_t reserved2 = 0;;
        } sw;
        bool operator<(const Code& other) const { return id < other.id; }
    };
    Code op;
    std::string_view description;
    unsigned precedence;
    static std::map<Code, NewOperator*> all;
};

class NewBinaryOperator : public NewOperator 
{
public:
    NewBinaryOperator(wchar_t code);
    ~NewBinaryOperator();
    virtual Expression operator()(const Expression& lhs, const Expression& rhs) const = 0;
    Tuple operator()(const Tuple& operands) const override;
    unsigned MinimumOperands() const override;
protected:
    const Expression* identity;
    const Expression* absorb;
};

class GenericOperation : public FlatTuple<GenericOperation>
{
public:
    GenericOperation(const GenericOperation&) = default;
    GenericOperation(const NewOperator& ope, std::initializer_list<Expression> operands);
    GenericOperation(const NewOperator& ope, Tuple&& operands);
    GenericOperation(wchar_t tag, Tuple&& operands);
    GenericOperation(wchar_t tag, Expression&& operand);

    template <std::ranges::input_range R>
	explicit GenericOperation(Operator ope, R items) 
        : FlatTuple<GenericOperation>(items) 
        , ope(ope)
    {
        if (ope.Operands() < 3) 
        {
            if (items.size() != ope.Operands())
                throw std::invalid_argument(std::format("Operation {} needs {} operands, not {}", ope, ope.Operands(), items.size()));
        }
        else 
        {
            if (!bool(ope.Identity()) && items.size() == 0)
                throw std::invalid_argument("Operands list of {} can not be empty");
        }  
    }
    GenericOperation& operator=(const GenericOperation& e);

    Expression Simplify() const;
    Expression Substitute(const Hypothesis& hypothesis) const;
    Expression Matches(const Expression& e, const Hypothesis& hypothesis) const;
    Expression Infer(const class Knowledge& knowledge, const Hypothesis& hypothesis, Trace& trace) const;
    const NewOperator& GetOperator() const;
    std::string OperandToString(const Expression& e, bool first) const;
    std::string Summary() const;

    bool operator==(const GenericOperation& rhs) const;
 
    template<typename T> 
    bool operator<(const T& rhs) const
    {
        std::hash<Expression> hasher;
        return Hash() < hasher(rhs);
    }
    std::size_t Hash() const;
protected:
    using Base=FlatTuple<GenericOperation>;
private:

    const NewOperator& ope; 
};

std::ostream& operator<<(std::ostream& s, const GenericOperation& e);


}
