#pragma once
#include <string>
#include <vector>
#include <limits>
#include "Engine/Utility/Range.h"

namespace Game
{

class StatDescriptor
{
public:
        explicit StatDescriptor(int value);
        StatDescriptor(Engine::Range<int> limit = Engine::Range<int>::max());
        void Contribute(std::string_view source, int value, bool skip0=true);
        int Total() const;
        std::string Description() const;
private:
        Engine::Range<int> limit {0, std::numeric_limits<int>::max()};
        std::vector<std::pair<std::string_view, int>> contributions;
};

}
