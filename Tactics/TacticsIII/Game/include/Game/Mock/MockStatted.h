#pragma once
#include <gmock/gmock.h>

#include "Game/Statted.h"
#include "Game/Stat.h"
#include "Game/StatDescriptor.h"
#include "Game/StatDefinition.h"

namespace Game::Test
{
using namespace ::testing;

class MockStatted : public Statted
{
public:
    MockStatted()
    {
        ON_CALL(*this, Definition()).WillByDefault(::testing::ReturnRef(definition));
        ON_CALL(*this, Name()).WillByDefault(::testing::Return("Mock"));
        ON_CALL(*this, Get(_)).WillByDefault(Invoke([this](Stat::Id id )
        {
            return Statted::Get(id);
        }));
    }
    MOCK_METHOD(std::string_view, Name, (), (const override));
    MOCK_METHOD(StatDescriptor, Get, (Stat::Id id), (const override));
    MOCK_METHOD(const class StatDefinition&,  Definition, (), (const override));

    StatDefinition definition;
};

}
