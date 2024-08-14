#pragma once
#include "Game/Statted.h"
#include "Game/Counted.h"

namespace Game
{

class Counters : public Counted
{
public:
    explicit Counters(const Statted& stats);
    virtual ~Counters() = default;

    Counters(const Counters& o);
    Counters(Counters&& o);
    Counters& operator=(const Counters& o);
    Counters& operator=(Counters&& o);

    unsigned Available(Stat::Id) const override;
    unsigned Cost(Stat::Id counter, unsigned cost, bool truncate=false) override;
    void Reset(Counter::Reset at) override;

    void Deserialize(const json& data);
    json Serialize() const;
protected:
    void InitializeCounters();
    virtual void OnCount(Stat::Id, unsigned ) {}
private:
    const Statted* stats;
    std::map<const Counter*, int> countersUsed;
};

};
