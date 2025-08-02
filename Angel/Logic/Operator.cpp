#include "Logic/Operator.h"
#include "Logic/Comparator.h"
#include "Logic/Internal/ComparisonImpl.h"
#include "Logic/Expression.h"
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
//    {BinaryOperator(L'∈'),  10, "@",  "element of"},    // TODO Element although it is a lot like item
//    {BinaryOperator(L'⊆'),  10, "@@", "subset"},        // TODO basically alias for and(*A ∈ B)
    {MultiOperator(L'⊕'),  20, "^",   "xor"},
    {MultiOperator(L'∨'),   20, "|",   "or"},
    {MultiOperator(L'∧'),   30, "&",   "and"},
    {PrefixOperator(L'¬'),  40, "~",  "not"},  
    {MultiOperator(L'+'),   50, "",    "add"},
    {MultiOperator(L'-'),   50, "",    "subtract"},
    {MultiOperator(L'⋅'),   60, "*",   "multiply"},
    {MultiOperator(L'÷'),   60, "/",   "divide"},
    {MultiOperator(L'↑'),   70, "**",  "exponent"}, 
    {MultiOperator(L'↓'),   70, "//",  "logarithm"}, 
    {PrefixOperator(L'+'),  75, "",   "positive"},
    {PrefixOperator(L'-'),  75, "",   "negative"},
    {PrefixOperator(L'√'),  75, "",   "square root"},
    {PostfixOperator(L'²'), 80, "",   "squared"},
    {PostfixOperator(L'³'), 80, "",   "cubed"},
 //   {PostfixOperator(L'!'), 85, "",   "factorial"}, // TODO 
    {PrefixOperator(L'∀'),  90, "*",  "all of"},
    {PrefixOperator(L'∃'),  90, "@",  "any of"},    // TODO Any
    {BinaryOperator(L'.'),  95, "",   "item"},
    {PrefixOperator(L'$'), 100, "",   "value of"},
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

bool Operator::NeedsBracesAround(const Expression& expression, bool first) const
{
    const auto* operation = expression.GetIf<Operation>();
    if (!operation)
        return false;
    auto expressionOperator = operation->GetOperator();
    if (expressionOperator.Precedence() < Precedence())
        return true;
    if (!first & expressionOperator.Precedence() == Precedence())
        return true; 
    return false;
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