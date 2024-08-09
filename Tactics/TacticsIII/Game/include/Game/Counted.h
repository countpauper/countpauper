#pragma once
#include "Game/Stat.h"
#include "Game/Counter.h"

namespace Game
{

class Counted
{
public:
    virtual ~Counted() = default;
    virtual unsigned Available(Stat::Id) const = 0;
    // returns the amount actually removed. If truncate is false it is cost on success, 0 on failure. Else it can be partial success 0..cost
    virtual unsigned Cost(Stat::Id counter, unsigned cost, bool truncate=false) = 0;
    virtual void Reset(Counter::Reset at) = 0;
protected:
    virtual void OnCount(Stat::Id, unsigned ) {}
private:
};
};
