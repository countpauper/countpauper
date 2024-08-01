#pragma once
#include "Game/Statted.h"
#include "Game/Counted.h"

namespace Game
{

class Counters : public Counted
{
public:
    explicit Counters(Statted& stats);
    virtual ~Counters() = default;

    unsigned Available(Stat::Id) const override;
    unsigned Cost(Stat::Id counter, unsigned cost, bool truncate=false) override;
    void Reset(Counter::Reset at) override;
protected:
    void InitializeCounters();
    virtual void OnCount(Stat::Id, unsigned ) {}
private:
    Statted& stats;
    std::map<const Counter*, int> countersUsed;
};

};
