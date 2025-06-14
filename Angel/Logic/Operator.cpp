#include "Logic/Operator.h"
#include "Logic/Comparator.h"
#include "Logic/Integer.h"
#include "Logic/Internal/OperationImpl.h"
#include "Logic/Internal/ComparisonImpl.h"
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

struct Definition
{
    Operator op;
    std::int32_t precedence;
    std::string_view altTag;
    std::string_view description;
};

static constexpr Definition opdef[]{
    {BinaryOperator(L'^'),  0, "",    "power"},
    {BinaryOperator(L'⋅'),  0, "*",   "multiply"},
    {BinaryOperator(L'÷'),  0, "/",   "divide"},
    {PrefixOperator(L'+'),  0, "",    "positive"},
    {BinaryOperator(L'+'),  0, "",    "add"},
    {BinaryOperator(L'-'),  0, "",    "subtract"},
    {PrefixOperator(L'-'),  0, "",    "negative"},
    {BinaryOperator(L'↑'),  0, "**",  "exponent"},
    {BinaryOperator(L'↓'),  0, "//",  "logarithm"},
    {BinaryOperator(L'∧'),  0, "&",   "and"},
    {BinaryOperator(L'∨'),  0, "|",   "or"},
    {BinaryOperator(L'⊕'), 0, "^",   "xor"},
    {PrefixOperator(L'¬'),  0, "~",   "not"},
    {Comparator(L'≠'),      0, "!=",  "not equal"},
    {Comparator(L'>'),      0, "",    "greater"},
    {Comparator(L'<'),      0, "",    "lesser"},
    {Comparator(L'≥'),      0, ">=",  "greater or equal"},
    {Comparator(L'≤'),      0, "<=",  "lesser or equal"},
    {Comparator(L'∈'),      0, "@",   "element of"},
    {Comparator(L'∀'),      0, "*",   "all of"},
    {PrefixOperator(L'∃'),   0, "@",   "any of"},
    {BinaryOperator(L'.'),      0, "",    "item"},
    {PrefixOperator(L'√'),   0, "",    "square root"},
    {PostfixOperator(L'²'), 0, "", "squared"},
    {PostfixOperator(L'³'), 0, "", "cubed"},
    {PostfixOperator(L'!'), 0, "", "factorial"},
    {Operator(),            0, "",    "none"} // 0 terminated
};

static const Definition& FindDefinition(uint32_t id)
{
    unsigned i;
    for(i=0;opdef[i].op.Id(); ++i)
    {
        if (opdef[i].op.Id() == id)
            return opdef[i];
    }
    return opdef[i];
}
static std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>, wchar_t> wchar_conv;

static const uint32_t FindId(const std::string_view tag, unsigned operands)
{
    if (tag.empty())
        return 0;

    for(unsigned i=0;opdef[i].op.Id(); ++i)
    {
        if (operands!=opdef[i].op.Operands())
            continue;
        if (tag.size()==1 && std::string(opdef[i].op)==tag)
            return opdef[i].op.Id();
        if (opdef[i].altTag == tag) 
            return opdef[i].op.Id();
    }
    throw std::invalid_argument(std::format("Undefined operator {}", tag));
}

Operator::Operator(const std::string_view tag, uint8_t operands) :
    op{.id{FindId(tag, operands)}}
{
    static_assert(sizeof(op) == 4);
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

UnaryOperator::UnaryOperator(const std::string_view tag) :
    Operator(tag, operands)
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