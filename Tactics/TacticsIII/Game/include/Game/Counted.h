#pragma once
#include "Game/Statted.h"
#include "Game/Stat.h"
#include "Game/Counter.h"

namespace Game
{

class Counted
{
public:
    explicit Counted(Statted& stats);
    virtual ~Counted() = default;

    unsigned Available(Stat::Id) const ;
    unsigned Cost(Stat::Id counter, unsigned cost, bool truncate=false);
    void Reset(Counter::Reset at);
protected:
    void InitializeCounters();
    virtual void OnCount(Stat::Id, unsigned ) {}
private:
    Statted& stats;
    std::map<const Counter*, int> countersUsed;
};

};
