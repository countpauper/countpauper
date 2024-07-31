#pragma once
#include <gmock/gmock.h>

#include "Game/Counted.h"

namespace Game::Test
{

class MockCounted : public Counted
{
public:
    MockCounted(Statted& stats) :
        Counted(stats)
    {
    }
    MOCK_METHOD(void, OnCount, (Stat::Id stat, unsigned remaining), (override));
};

}
