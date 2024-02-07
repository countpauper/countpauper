#pragma once
#include <gmock/gmock.h>

#include "Game/Statted.h"
#include "Game/Stat.h"
#include "Game/StatDescriptor.h"

namespace Game::Test
{

class MockStatted : public Statted
{
public:
        MOCK_METHOD(StatDescriptor, Get, (Stat::Id id), (const override));
        MOCK_METHOD(const class StatDefinition&,  Definition, (), (const override));
};

}
