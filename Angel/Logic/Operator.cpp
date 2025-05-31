#include "Logic/Operator.h"
#include "Logic/Integer.h"
#include "Logic/Internal/IntOperation.h"
#include <locale>
#include <codecvt>
#include <format>
#include "string.h"

namespace Angel::Logic
{

constexpr uint32_t make_code(wchar_t unicode, uint8_t operands, bool postfix=false)
{
    return static_cast<uint32_t>(unicode) | static_cast<uint32_t>(operands&3)<<16 | (postfix?(1<<18):0); 
}

static const Operator::Definition opdef[]{
    {make_code(L'^', 2), 0, "",    "power"},
    {make_code(L'*', 2), 0, "",    "multiply"},
    {make_code(L'÷', 2), 0, "/",   "divide"},
    {make_code(L'+', 1), 0, "",    "positive"},
    {make_code(L'+', 2), 0, "",    "add"},
    {make_code(L'-', 2), 0, "",    "subtract"},
    {make_code(L'-', 1), 0, "",    "negative"},
    {make_code(L'↑', 2), 0, "**",  "exponent"},
    {make_code(L'↓', 2), 0, "//",  "logarithm"},
    {make_code(L'∧', 2), 0, "&",   "and"},
    {make_code(L'∨', 2), 0, "|",   "or"},
    {make_code(L'⊕',2), 0, "^",   "xor"},
    {make_code(L'¬', 1), 0, "~",   "not"},
    {make_code(L'≠', 2), 0, "!=",  "not equal"},
    {make_code(L'>', 2), 0, "",    "greater"},
    {make_code(L'<', 2), 0, "",    "lesser"},
    {make_code(L'≥', 2), 0, ">=",  "greater or equal"},
    {make_code(L'≤', 2), 0, "<=",  "lesser or equal"},
    {make_code(L'∈', 2), 0, "@",   "element of"},
    {make_code(L'∀', 2), 0, "*",   "all of"},
    {make_code(L'∃', 1), 0, "@",   "any of"},
    {make_code(L'.', 2), 0, "",    "item"},
    {make_code(L'√', 1), 0, "",    "square root"},
    {make_code(L'²', 1, true), 0, "", "squared"},
    {make_code(L'³', 1, true), 0, "", "cubed"},
    {make_code(L'!', 1, true), 0, "", "factorial"},
    {                0,  0, "",    "none"} // 0 terminated
};
static const Operator::Definition& FindDefinition(uint32_t id)
{
    unsigned i;
    for(i=0;opdef[i].code.id; ++i)
    {
        if (opdef[i].code.id == id)
            return opdef[i];
    }
    return opdef[i];
}
static std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>, wchar_t> wchar_conv;

static const uint32_t FindId(const std::string_view tag, unsigned operands)
{
    if (tag.empty())
        return 0;

    std::wstring wtag = wchar_conv.from_bytes(tag.begin(), tag.end());
    for(unsigned i=0;opdef[i].code.id; ++i)
    {
        if (operands!=opdef[i].code.sw.operands)
            continue;
        if (opdef[i].code.sw.unicode == wtag[0])
            return opdef[i].code.id;
        if (opdef[i].altTag == tag) 
            return opdef[i].code.id;
    }
    throw std::invalid_argument(std::format("Undefined operator {}", tag));
}

Operator::Operator(const std::string_view tag, uint8_t operands) :
    op{.id{FindId(tag, operands)}}
{
    static_assert(sizeof(op) == 4);
}

bool Operator::operator==(const Operator& rhs) const
{
    return op.id == rhs.op.id;
}

Operator::operator bool() const
{
    return op.id!=0;
}

const std::string_view Operator::Description() const
{
    return FindDefinition(op.id).description;
}

Operator::operator std::string() const
{
    wchar_t wstr[2] { op.sw.unicode, 0};
    return wchar_conv.to_bytes(wstr);
}

unsigned Operator::Precedence() const
{
    return FindDefinition(op.id).precedence;
}

template<>
Integer& operate<BinaryOperator{L'+'}>(Integer& lhs, const Integer& rhs)
{
    lhs+=rhs;
    return lhs;
}

template<>
Integer& operate<BinaryOperator{L'-'}>(Integer& lhs, const Integer& rhs)
{
    lhs-=rhs;
    return lhs;
}

template<>
Integer& operate<BinaryOperator{L'*'}>(Integer& lhs, const Integer& rhs)
{
    lhs*=rhs;
    return lhs;
}

template<>
Integer& operate<BinaryOperator{L'/'}>(Integer& lhs, const Integer& rhs)
{
    lhs/=rhs;
    return lhs;
}

template<>
Integer& operate<BinaryOperator{L'^'}>(Integer& lhs, const Integer& rhs)
{
    lhs^=rhs;
    return lhs;
}

template<> 
bool compare<BinaryOperator{L'='}>(Expression& lhs, const Expression& rhs)
{
    return lhs == rhs;
}

UnaryOperator::UnaryOperator() : 
    Operator(0,0)
{        
}

UnaryOperator::UnaryOperator(const std::string_view tag) :
    Operator(tag, operands)
{
}

BinaryOperator::BinaryOperator() : 
    Operator(0,0)
{        
}

BinaryOperator::BinaryOperator(const std::string_view tag) :
    Operator(tag, operands)
{
}

std::ostream& operator<<(std::ostream& os, const Operator& op)
{
    os << std::string(op);
    return os;
}

std::ostream& operator<<(std::ostream& os, const BinaryOperator& op)
{
    os << Operator(op);
    return os;
}


std::ostream& operator<<(std::ostream& os, const UnaryOperator& op)
{
    os << Operator(op);
    return os;
}
}