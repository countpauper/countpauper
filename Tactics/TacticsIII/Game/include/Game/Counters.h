#pragma once
#include "Game/Statted.h"
#include "Game/Counted.h"

namespace Game
{

class Counters : public Counted
{
public:
    explicit Counters(const StatDefinition& definition, const Statted& stats);
    Counters(const StatDefinition& definition, const Statted& stats, const json& data);
    virtual ~Counters() = default;

    Counters(const Counters& o);
    Counters(Counters&& o);
    Counters& operator=(const Counters& o);
    Counters& operator=(Counters&& o);

    unsigned Available(Stat::Id) const override;
    unsigned Cost(Stat::Id counter, unsigned cost, bool truncate=false) override;
    void Reset(Counter::Reset at) override;

    json Serialize() const;
protected:
    void InitializeCounters(const StatDefinition& definition);
    virtual void OnCount(Stat::Id, unsigned ) {}
private:
    const Statted* stats;
    using AvailableMap = std::map<const Counter*, int>;
    AvailableMap countersUsed;

    AvailableMap::iterator Find(Stat::Id stat);
};

}
