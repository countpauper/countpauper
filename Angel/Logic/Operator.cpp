#include "Logic/Operator.h"
#include <locale>
#include <codecvt>
#include <format>

namespace Angel::Logic
{

struct OperatorDefinition
{
    wchar_t code;
    uint8_t operands;
    uint8_t precedence;
    std::string_view altTag;
    std::string_view description;
};

static const OperatorDefinition opdef[]{
    {L'^', 2, 0, "",    "power"},
    {L'*', 2, 0, "",    "multiply"},
    {L'÷', 2, 0, "/",   "divide"},
    {L'+', 1, 0, "",    "positive"},
    {L'+', 2, 0, "",    "add"},
    {L'-', 2, 0, "",    "subtract"},
    {L'-', 1, 0, "",    "negative"},
    {L'&', 2, 0, "",    "and"},
    {L'|', 2, 0, "",    "or"},
    {L'¬', 1, 0, "~",   "not"},
    {L'≠', 2, 0, "!=",  "not equal"},
    {L'>', 2, 0, "",    "grearer"},
    {L'<', 2, 0, "",    "lesser"},
    {L'≥', 2, 0, ">=",  "greater or equal"},
    {L'≤', 2, 0, "<=",  "lesser or equal"},
    {L'∈', 2, 0, "@",   "element of"},
    {L'∀', 2, 0, "*",   "all of"},
    {L'∃', 1, 0, "@",   "any of"},
    {L'.', 2, 0, "",    "item"},
    {   0, 2, 0, "",    "none"} // 0 terminated
};

static std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>, wchar_t> wchar_conv;

static const OperatorDefinition& FindDefinition(unsigned code)
{
    unsigned i;
    for(i=0;opdef[i].code; ++i)
    {
        if (opdef[i].code == code)
            return opdef[i];
    }
    return opdef[i];
}

static const OperatorDefinition& FindDefinition(const std::string_view tag, unsigned operands)
{
    std::wstring wtag = wchar_conv.from_bytes(tag.begin(), tag.end());
    for(unsigned i=0;opdef[i].code; ++i)
    {
        if (operands!=opdef[i].operands)
            continue;   // TODO: could separate two types of operands and use separate tables
        if (opdef[i].code == wtag[0])
            return opdef[i];
        if (opdef[i].altTag == tag) 
            return opdef[i];
    }
    throw std::invalid_argument(std::format("Undefined operator {}", tag));
}

Operator::Operator(const std::string_view tag, unsigned operands) :
    code(0)
{
    if (!tag.empty())
    {
        const OperatorDefinition& def = FindDefinition(tag, operands);
        code = def.code;
    }
}

bool Operator::operator==(const Operator& rhs) const
{
    return code == rhs.code;
}

Operator::operator bool() const
{
    return code!=0;
}

const std::string_view Operator::Description() const
{
    return FindDefinition(code).description;
}

Operator::operator std::string() const
{
    return wchar_conv.to_bytes(reinterpret_cast<const wchar_t*>(&code));
}

unsigned Operator::Precedence() const
{
    return FindDefinition(code).precedence;
}

std::ostream& operator<<(std::ostream& os, const Operator& op)
{
    os << std::string(op);
    return os;
}

}