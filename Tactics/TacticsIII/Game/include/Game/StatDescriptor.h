#pragma once
#include <string>
#include <vector>
#include <limits>
#include "Utility/Range.h"

namespace Game
{

class StatDescriptor
{
public:
    StatDescriptor(Engine::Range<int> limit = Engine::Range<int>::null());
    explicit StatDescriptor(int value); // for unit tests
    StatDescriptor& Contribute(std::string_view source, int value, bool skip0=true);
    int Total() const;
    std::string Description() const;
    operator bool() const;
private:
    Engine::Range<int> limit {0, std::numeric_limits<int>::max()};
    std::vector<std::pair<std::string_view, int>> contributions;
};

}
