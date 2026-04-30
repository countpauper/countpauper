#pragma once
#include <gmock/gmock.h>

#include "Game/Conditions.h"

namespace Game::Test
{
using namespace ::testing;

class MockConditions : public Conditions
{
public:
    MOCK_METHOD(unsigned, GetCondition, (std::function<bool(const Condition& condition)> pred), (const override));
    MOCK_METHOD(void, SetCondition, (const Condition& condition, unsigned level), (override));
    MOCK_METHOD(Computation, ConditionalBonus, (Stat::Id id), (const override));

};

}
