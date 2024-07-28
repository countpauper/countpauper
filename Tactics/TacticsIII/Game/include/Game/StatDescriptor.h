#pragma once
#include <limits>
#include "Utility/Range.h"
#include "Game/Computation.h"

namespace Game
{

class StatDescriptor : public Computation
{
public:
    StatDescriptor(Engine::Range<int> limit = Engine::Range<int>::null());
    explicit StatDescriptor(int value); // for unit tests
    int Total() const override;
    bool IsValid() const;
    StatDescriptor& Contribute(std::string_view source, int value, bool skip0=true);
protected:
    Engine::Range<int> limit {0, std::numeric_limits<int>::max()};
};

}
