#pragma once
#include <gmock/gmock.h>
#include "Game/Equipped.h"

namespace Game::Test
{
using namespace ::testing;

class MockEquipped: public Equipped
{
public:
    MockEquipped()
    {
    }
    MOCK_METHOD(void, Equip, (const Equipment& equipment), (override));
    MOCK_METHOD(bool, Unequip, (const Equipment& item), (override));
    MOCK_METHOD(unsigned, Unequip, (const Restrictions filter), (override));
    MOCK_METHOD(std::vector<const Equipment*>, GetEquipped, (const Restrictions& filter), (const override));
    MOCK_METHOD(Computation, Get, (Stat::Id id, const class Boni* extraBoni, const Restrictions& filter), (const override));
};

}
