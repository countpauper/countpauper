#pragma once
#include <gmock/gmock.h>

#include "Game/Counted.h"

namespace Game::Test
{

class MockCounted : public Counted
{
public:
    MOCK_METHOD(unsigned, Available, (Stat::Id), (const override));
    MOCK_METHOD(unsigned, Cost, (Stat::Id counter, unsigned cost, bool truncate), (override));
    MOCK_METHOD(void, Reset, (Counter::Reset at), (override));
    MOCK_METHOD(void, OnCount, (Stat::Id stat, unsigned remaining), (override));
};

}
