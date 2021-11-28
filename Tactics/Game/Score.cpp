#include "stdafx.h"
#include "Score.h"
#include <sstream>
#include <numeric>
#include "Engine/from_string.h"

namespace Game
{

Bonus::Bonus() :
    value(0)
{
}


Bonus::Bonus( int value) :
    value(value)
{
}

Bonus::Bonus(const std::wstring_view description, int value) :
    value(value),
    description(description)
{
}

Bonus::Bonus(const Bonus& other) :
    value(other.value),
    description(other.description)
{
}



Score::Score() 
{
}

Score::Score(const std::wstring_view description, int value) :
    Score(Bonus(description, value))
{
}

Score::Score(const Bonus& bonus) :
    boni({ bonus })
{
}


Score::Score(const Score& other) :
    boni(other.boni)
{
}


unsigned Score::Value() const
{
    int totalBonus = std::accumulate(boni.begin(), boni.end(), 0, [](int v, const Bonus& b) { return v+b.value; });
    if (totalBonus >= 0)
    {
        return unsigned(totalBonus);
    }
    else
        return 0;
}

std::wstring Score::Description() const
{
    std::wstring description = std::to_wstring(Value())+L"(";
    bool first = true;
    for (const auto& bonus : boni)
    {
        if (!bonus)
            continue;
        if ((!first) && (bonus.value >= 0))
            description += L"+";
        description += std::to_wstring(bonus.value);
        if (!bonus.description.empty())
            description += L"(" + bonus.description + L")";
        first = false;
    }

    description += L")";
    return description;
}

Score& Score::operator += (const Bonus& bonus)
{
    boni.push_back(bonus);
    return *this;
}

Score& Score::operator+=(const Score& other)
{
    boni.insert(boni.end(), other.boni.begin(), other.boni.end());

    return *this;
}

Score& Score::operator-=(const Score& other)
{
    for (const auto& penalty : other.boni)
    { 
        boni.emplace_back(Bonus(penalty.description, -penalty.value));
    }
    return *this;
}

bool Score::operator==(const Score& other) const
{
    return Value() == other.Value();
}

bool Score::operator==(unsigned value) const
{
    return Value() == value;
}

Score operator+(const Score& a, const Bonus& b)
{
    Score o(a);
    o += b;
    return o;
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
    int value;
    s >> value;
    score = Score() + Bonus(L"File", value);
    return s;
}

std::ostream& operator<<(std::ostream& s, const Score& score)
{
    s << Engine::from_string<std::string>(score.Description()).c_str();
    return s;
}

}
