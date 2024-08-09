#pragma once
#include <gmock/gmock.h>

#include "Game/Statistics.h"
#include "Game/Stat.h"
#include "Game/Computation.h"
#include "Game/StatDefinition.h"

namespace Game::Test
{
using namespace ::testing;

class MockStatted : public Statistics
{
public:
    MockStatted()
    {
        ON_CALL(*this, Definition()).WillByDefault(::testing::ReturnRef(definition));
        ON_CALL(*this, Name()).WillByDefault(::testing::Return("Mock"));
        ON_CALL(*this, Get(_, _)).WillByDefault(Invoke([this](Stat::Id id, const Restrictions& restrict )
        {
            return Statistics::Get(id, restrict);
        }));
    }
    MOCK_METHOD(std::string_view, Name, (), (const override));
    MOCK_METHOD(Computation, Get, (Stat::Id id, const Restrictions& restrict), (const override));
    MOCK_METHOD(const class StatDefinition&,  Definition, (), (const override));

    StatDefinition definition;
};

}
