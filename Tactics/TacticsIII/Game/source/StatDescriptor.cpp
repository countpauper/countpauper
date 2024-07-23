#include "Game/StatDescriptor.h"
#include <numeric>
#include <sstream>

namespace Game
{

StatDescriptor::operator bool() const
{
    return limit;   // if limit is not empty, the stat is valid and truthy
}

StatDescriptor::StatDescriptor(Engine::Range<int> limit) :
        limit(limit)
{
}

StatDescriptor::StatDescriptor(int value) :
    StatDescriptor(Engine::Range<int>::max())
{
    Contribute("", value);
}

StatDescriptor& StatDescriptor::Contribute(std::string_view source, int value, bool skip0)
{
    if (skip0 && value==0)
            return *this;
    contributions.emplace_back(source, value);
    return *this;
}

int StatDescriptor::Total() const
{
    auto total = std::accumulate(contributions.begin(), contributions.end(), 0,
            [](int v, const decltype(contributions)::value_type& contrib)
            {
                    return v + contrib.second;
            });
    return limit.Clip(total);
}

std::string StatDescriptor::Description() const
{
    std::stringstream stream;

    for(const auto& contrib : contributions)
    {
        if (contrib.second >= 0) {
            if (stream.tellp()) {
                stream << " + ";
            }
            stream << contrib.second;
        } else {
            stream << " - " -contrib.second;
        }

        if (!contrib.first.empty()) {
            stream << "[" << contrib.first << "]";
        }
    }
    return stream.str();
}



}
