#pragma once
#include <string>
#include <vector>
#include "Engine/Range.h"

namespace Game
{


class StatDescriptor
{
public:

        StatDescriptor(Engine::Range<int> range = Engine::Range<int>::max());
        void Contribute(std::string_view source, int value, bool skip0=true);
        int Total() const;
        std::string Description() const;
private:
        Engine::Range<int> range;
        std::vector<std::pair<std::string_view, int>> contributions;
};

}
