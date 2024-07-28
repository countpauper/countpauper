#include "Game/Computation.h"
#include <numeric>
#include <sstream>

namespace Game
{

Computation::Computation(std::string_view description, int value) :
    contributions{
        {description, value}
    }
{
}

int Computation::Total() const
{
    auto total = std::accumulate(contributions.begin(), contributions.end(), 0,
            [](int v, const decltype(contributions)::value_type& contrib)
            {
                    return v + contrib.second;
            });
    return total;
}

std::string Computation::Description() const
{
    std::stringstream stream;

    for(const auto& contrib : contributions)
    {
        if (contrib.second >= 0)
        {
            if (stream.tellp())
                stream << " + ";
            stream << contrib.second;
        }
        else
        {
            if (stream.tellp())
                stream << " - " << -contrib.second;
            else
                stream << contrib.second;
        }

        if (!contrib.first.empty())
        {
            stream << "[" << contrib.first << "]";
        }
    }
    return stream.str();
}


bool Computation::empty() const
{
    return contributions.empty();
}

Computation& Computation::operator+=(const Computation& o)
{
    contributions.insert(contributions.end(), o.contributions.begin(), o.contributions.end());
    return *this;
}

Computation& Computation::operator-=(const Computation& o)
{
    for(const auto& c: o.contributions)
    {
        contributions.emplace_back(c.first, -c.second);
    }
    return *this;
}

Computation operator+(const Computation& a, const Computation& b)
{
    Computation result(a);
    return result+=b;
}

Computation operator-(const Computation& a, const Computation& b)
{
    Computation result(a);
    return result -= b;
}


}
