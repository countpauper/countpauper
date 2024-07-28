#include "Game/StatDescriptor.h"

#include <cassert>

namespace Game
{

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
    assert(IsValid());  // what does it mean
    return limit.Clip(Computation::Total());
}

bool StatDescriptor::IsValid() const
{
    return limit;   // if limit is not empty, the stat is valid
}

}
