#include "Logic/Operator.h"
#include <locale>
#include <codecvt>
#include <format>

namespace Angel::Logic
{

struct OperatorDefinition
{
    wchar_t code;
    std::string_view altTag;
    unsigned precedence;
};

static const OperatorDefinition opdef[]{
    {L'^', "", 0},
    {L'*', "", 0},
    {L'÷', "/", 0},
    {L'+', "", 0},
    {L'-', "", 0},
    {L'&', "", 0},
    {L'|', "", 0},
    {L'¬', "~", 0},
    {L'≠', "!=", 0},
    {L'>', "", 0},
    {L'<', "", 0},
    {L'≥', ">=", 0},
    {L'≤', "<=", 0},
    {L'≥', ">=", 0},
    {L'≤', "<=", 0},
    {L'∈', "@", 0},
    {L'∀', "*", 0},
    {L'∃', "@", 0},
    {L'.', "", 0},
    {0, "", 0} // 0 terminated
};

static std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>, wchar_t> wchar_conv;

static const OperatorDefinition& FindDefinition(const std::string_view tag)
{
    std::wstring wtag = wchar_conv.from_bytes(tag.begin(), tag.end());
    for(unsigned i=0;opdef[i].code; ++i)
    {
        if (opdef[i].code == wtag[0])
            return opdef[i];
        if (opdef[i].altTag == tag) 
            return opdef[i];
    }
    throw std::invalid_argument(std::format("Undefined operator {}", tag));
}

Operator::Operator(const std::string_view tag) :
    code(0)
{
    if (!tag.empty())
    {
        const OperatorDefinition& def = FindDefinition(tag);
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

Operator::operator std::string() const
{
    return wchar_conv.to_bytes(reinterpret_cast<const wchar_t*>(&code));
}

unsigned Operator::Precedence() const
{
    return 0;   // TODO 
}

std::ostream& operator<<(std::ostream& os, const Operator& op)
{
    os << std::string(op);
    return os;
}

}