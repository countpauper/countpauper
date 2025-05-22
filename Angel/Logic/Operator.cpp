#include "Logic/Operator.h"
#include <locale>
#include <codecvt>
#include <format>
#include "string.h"

namespace Angel::Logic
{

constexpr uint32_t make_code(wchar_t unicode, uint8_t operands)
{
    return static_cast<uint32_t>(unicode) | static_cast<uint32_t>(operands)<<16; 
}

static const Operator::Definition opdef[]{
    {make_code(L'^', 2), 0, "",    "power"},
    {make_code(L'*', 2), 0, "",    "multiply"},
    {make_code(L'÷', 2), 0, "/",   "divide"},
    {make_code(L'+', 1), 0, "",    "positive"},
    {make_code(L'+', 2), 0, "",    "add"},
    {make_code(L'-', 2), 0, "",    "subtract"},
    {make_code(L'-', 1), 0, "",    "negative"},
    {make_code(L'&', 2), 0, "",    "and"},
    {make_code(L'|', 2), 0, "",    "or"},
    {make_code(L'¬', 1), 0, "~",   "not"},
    {make_code(L'≠', 2), 0, "!=",  "not equal"},
    {make_code(L'>', 2), 0, "",    "grearer"},
    {make_code(L'<', 2), 0, "",    "lesser"},
    {make_code(L'≥', 2), 0, ">=",  "greater or equal"},
    {make_code(L'≤', 2), 0, "<=",  "lesser or equal"},
    {make_code(L'∈', 2), 0, "@",   "element of"},
    {make_code(L'∀', 2), 0, "*",   "all of"},
    {make_code(L'∃', 1), 0, "@",   "any of"},
    {make_code(L'.', 2), 0, "",    "item"},
    {                0,  0, "",    "none"} // 0 terminated
};


/*
static const Operator::Definition opdef[]{
    {.code{.sw{L'^', 0, 2}}, .precedence=0, .altTag="",    .description="power"},
    {.code{.sw{L'*', 0, 2}}, .precedence=0, .altTag="",    .description="multiply"},
    {.code{.sw{L'÷', 0, 2}}, .precedence=0, .altTag="/",   .description="divide"},
    {.code{.sw{L'+', 0, 1}}, .precedence=0, .altTag="",    .description="positive"},
    {.code{.sw{L'+', 0, 2}}, .precedence=0, .altTag="",    .description="add"},
    {.sw{L'-', 0, 2}, .precedence=0, .altTag="",    .description="subtract"},
    {.sw{L'-', 0, 1}, .precedence=0, .altTag="",    .description="negative"},
    {.sw{L'&', 0, 2}, .precedence=0, .altTag="",    .description="and"},
    {.sw{L'|', 0, 2}, .precedence=0, .altTag="",    .description="or"},
    {.sw{L'¬', 0, 1}, .precedence=0, .altTag="~",   .description="not"},
    {.sw{L'≠', 0, 2}, .precedence=0, .altTag="!=",  .description="not equal"},
    {.sw{L'>', 0, 2}, .precedence=0, .altTag="",    .description="grearer"},
    {.sw{L'<', 0, 2}, .precedence=0, .altTag="",    .description="lesser"},
    {.sw{L'≥', 0, 2}, .precedence=0, .altTag=">=",  .description="greater or equal"},
    {.sw{L'≤', 0, 2}, .precedence=0, .altTag="<=",  .description="lesser or equal"},
    {.sw{L'∈', 0, 2}, .precedence=0, .altTag="@",   .description="element of"},
    {.sw{L'∀', 0, 2}, .precedence=0, .altTag="*",   .description="all of"},
    {.sw{L'∃', 0, 1}, .precedence=0, .altTag="@",   .description="any of"},
    {.sw{L'.', 0, 2}, .precedence=0, .altTag="",    .description="item"},
    {.code {      0}, .precedence=0, .altTag="",    .description="none"} // 0 terminated
};

*/
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