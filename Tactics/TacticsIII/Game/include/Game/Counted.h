#pragma once
#include "Game/Statted.h"
#include "Game/Stat.h"
#include "Game/Counter.h"

namespace Game
{

class Counted
{
public:
    virtual ~Counted() = default;
    virtual unsigned Available(Stat::Id) const = 0;
    virtual unsigned Cost(Stat::Id counter, unsigned cost, bool truncate=false) = 0;
    virtual void Reset(Counter::Reset at) = 0;
protected:
    virtual void OnCount(Stat::Id, unsigned ) {}
private:
};

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
