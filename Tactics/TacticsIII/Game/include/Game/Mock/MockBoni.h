#pragma once
#include <gmock/gmock.h>

#include "Game/Boni.h"

namespace Game::Test
{
using namespace ::testing;

class MockBoni : public Boni
{
public:
    MOCK_METHOD(Computation, Bonus, (Stat::Id id), (const override));
};

}
