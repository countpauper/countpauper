#pragma once

#include "Logic/Internal/Tuple.h"
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
    NewOperator();
    virtual ~NewOperator() = default;
    operator std::string() const;
    bool operator==(const NewOperator& o) const;
    operator bool() const;
    std::string_view Description() const;
    bool NeedsBracesAround(const Expression& expresison, bool first) const;
    unsigned Precedence() const;    
    virtual unsigned MinimumOperands() const = 0;
    bool IsUnary() const;
    bool IsMultiary() const;
    bool IsPostfix() const;
    bool IsComparator() const;
    bool IsCommutative() const;

    const NewOperator& Inversion() const;
    virtual Tuple operator()(const Tuple& operands) const = 0;
    std::size_t Hash() const;
    virtual std::string OperandToString(const Expression& operand, bool first) const = 0;
    static const NewOperator& Find(wchar_t tag, bool unary);
    static const NewOperator& none;
protected:
    constexpr NewOperator(const wchar_t tag, uint8_t operands, bool postfix=false, bool comparator=false) :
        op{.sw{tag, operands, postfix, comparator, 0, 0}}
    {
       all.insert(std::make_pair(op, this));
    }
    void SelfInvertible();
    bool SetInvertible(const wchar_t inversion);
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
    bool commutative = false; 
    const NewOperator* inverse=nullptr;
    static std::map<Code, NewOperator*> all;
};

class NewUnaryOperator : public NewOperator
{
public:
    NewUnaryOperator(wchar_t c);
    Tuple operator()(const Tuple& operands) const override;
    unsigned MinimumOperands() const override;
    virtual Expression operator()(const Expression& operand) const = 0;
    std::string OperandToString(const Expression& operand, bool first) const override;
};

class NewBinaryOperator : public NewOperator 
{
public:
    NewBinaryOperator(wchar_t code, bool multiary=false);
    ~NewBinaryOperator();
    virtual Expression operator()(const Expression& lhs, const Expression& rhs) const = 0;
    Tuple operator()(const Tuple& operands) const override;
    unsigned MinimumOperands() const override;
    std::string OperandToString(const Expression& operand, bool first) const override;
protected:
    const Expression* identity;
    const Expression* absorb;
};

}

