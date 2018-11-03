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
    base(0)
{
}

Score::Score(unsigned value) :
    base(value)
{
}

Score::Score(const Score& other) :
    base(other.base),
    boni(other.boni)
{
}


unsigned Score::Evaluate() const
{
    int value = base;
    for (const auto& bonus : boni)
    {
        value += bonus.value;
    }
    if (value > 0)
        return unsigned(value);
    else
        return 0;
}

std::wstring Score::Description() const
{
    std::wstringstream ss;
    ss << base;
    for (const auto bonus : boni)
    {
        if (!bonus)
            continue;
        ss << " +" << bonus.value << L"(" << bonus.description << L")";
    }
    return ss.str(); 
}

Score& Score::operator += (const Bonus& other)
{
    boni.push_back(other);
    return *this;
}

Score& Score::operator+=(const Score& other)
{
    boni.push_back(Bonus(other.Description(), other.Evaluate()));
    return *this;
}

Score& Score::operator-=(const Score& other)
{
    boni.push_back(Bonus(std::wstring(L"-( ") + other.Description() + std::wstring(L" )"), - int(other.Evaluate())));
    return *this;
}

Score operator+(const Score& a, const Score& b)
{
    Score o(a);
    o += b;
    return o;
}

std::wistream& operator>>(std::wistream& s, Score& score)
{
    // TODO: also store description & boni somehow?
    s >> score.base;
    return s;
}
}