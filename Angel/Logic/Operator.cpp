#include "Logic/Operator.h"
#include "Logic/Comparator.h"
#include "Logic/Integer.h"
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
    {BinaryOperator(L'←'),  0, ":",  "with/if"},
    {Filter(L'='),          5, "",   "equal"},
    {Filter(L'≠'),          5, "!=", "not equal"},
    {Filter(L'>'),          5, "",   "greater"},
    {Filter(L'<'),          5, "",   "lesser"},
    {Filter(L'≥'),          5, ">=", "greater or equal"},
    {Filter(L'≤'),          5, "<=", "lesser or equal"},
    {Comparator(L'='),      10, "",   "equal"},
    {Comparator(L'≠'),      10, "!=", "not equal"},
    {Comparator(L'>'),      10, "",   "greater"},
    {Comparator(L'<'),      10, "",   "lesser"},
    {Comparator(L'≥'),      10, ">=", "greater or equal"},
    {Comparator(L'≤'),      10, "<=", "lesser or equal"},
    {MultiOperator(L'⊕'),  20, "^",   "xor"},
    {MultiOperator(L'∨'),   20, "|",   "or"},
    {MultiOperator(L'∧'),   30, "&",   "and"},
    {PrefixOperator(L'¬'),  35, "~",  "not"},
    {Comparator(L'∈'),      40, "@",  "element of"},
    {MultiOperator(L'+'),   50, "",    "add"},
    {MultiOperator(L'-'),   50, "",    "subtract"},
    {MultiOperator(L'⋅'),   60, "*",   "multiply"},
    {MultiOperator(L'÷'),   60, "/",   "divide"},
    {MultiOperator(L'↑'),   70, "**",  "exponent"},
    {MultiOperator(L'↓'),   70, "//",  "logarithm"},
    {PrefixOperator(L'+'),  80, "",   "positive"},
    {PrefixOperator(L'-'),  80, "",   "negative"},
    {PrefixOperator(L'√'),  85, "",   "square root"},
    {PostfixOperator(L'²'), 85, "",   "squared"},
    {PostfixOperator(L'³'), 85, "",   "cubed"},
    {PostfixOperator(L'!'), 90, "",   "factorial"},
    {PrefixOperator(L'$'),  95, "",   "value of"},
    {PrefixOperator(L'∀'),  95, "*",  "all of"},
    {PrefixOperator(L'∃'),  95, "@",  "any of"},
    {BinaryOperator(L'.'), 100, "",   "item"},
    {Operator(),             0, "",   "none"} // 0 terminated
};

std::vector<Operator> Operator::All()
{
    std::vector<Operator> result;
    for(const auto& defop : opdef)
    {
        if (defop.op.Id())
            result.push_back(defop.op);
    }
    return result;
}


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

uint32_t Operator::FindId(const std::string_view tag, unsigned operands)
{
    if (tag.empty())
        return 0;

    for(unsigned i=0;opdef[i].op.Id(); ++i)
    {
        if (operands!=opdef[i].op.Operands())
            continue;
        if (std::string(opdef[i].op)==tag)
            return opdef[i].op.Id();
        if (opdef[i].altTag == tag) 
            return opdef[i].op.Id();
    }
    return 0;
}

Operator::Operator(const std::string_view tag, uint8_t operands) :
    Operator(FindId(tag, operands))
{
    static_assert(sizeof(op) == 4);
    if (!tag.empty() && op.id==0)
        throw std::invalid_argument(std::format("Undefined operator {}", tag));    
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

std::ostream& operator<<(std::ostream& os, const Operator& op)
{
    os << std::string(op);
    return os;
}
}