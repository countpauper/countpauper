#include "stdafx.h"
#include "Score.h"
#include <sstream>

namespace Game
{

Bonus::Bonus() :
    value(0)
{
}

Bonus::Bonus(const std::wstring& description, int value) :
    value(value),
    description(description)
{
}

Bonus::Bonus(const Bonus& other) :
    value(other.value),
    description(other.description)
{
}



Score::Score() :
    value(0)
{
}


Score::Score(const Score& other) :
    value(other.value),
    description(other.description)
{
}


unsigned Score::Value() const
{
    return value;
}

std::wstring Score::Description() const
{
    return std::to_wstring(value)+L"("+description+L")";
}

Score& Score::operator += (const Bonus& bonus)
{
    if (!bonus)
        return *this;

    value += bonus.value;
    if ((!description.empty()) && (bonus.value>=0))
        description += L"+";
    description += std::to_wstring(bonus.value) + L"(" + bonus.description + L")";
    return *this;
}

Score operator+(const Score& a, const Bonus& b)
{
    Score o(a);
    o += b;
    return o;
}

std::wistream& operator>>(std::wistream& s, Score& score)
{
    // TODO: also store description & boni somehow?
    s >> score.value;
    return s;
}
}