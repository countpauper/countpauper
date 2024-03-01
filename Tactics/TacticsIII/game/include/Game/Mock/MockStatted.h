#pragma once
#include <gmock/gmock.h>

#include "Game/Statted.h"
#include "Game/Stat.h"
#include "Game/StatDescriptor.h"
#include "Game/StatDefinition.h"

namespace Game::Test
{

class MockStatted : public Statted
{
public:
    MockStatted(std::initializer_list<std::pair<const Stat::Id, Stat>> stats={})
    {
        definition.insert(stats);
        ON_CALL(*this, Definition()).WillByDefault(::testing::ReturnRef(definition));
    }
    MOCK_METHOD(StatDescriptor, Get, (Stat::Id id), (const override));
    MOCK_METHOD(const class StatDefinition&,  Definition, (), (const override));

    StatDefinition definition;
};

}
