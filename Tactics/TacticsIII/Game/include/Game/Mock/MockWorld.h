#pragma once
#include <gmock/gmock.h>
#include "Game/World.h"
#include "Game/Mock/MockMap.h"

namespace Game::Test
{
using namespace ::testing;

class MockWorld : public World
{
public:
    MockWorld() = default;
    const BlockMap& GetMap() const
    {
        return map;
    }
    MOCK_METHOD(bool, Obstacle, (Position at, const Actor* except), (const override));
    NiceMock<MockMap> map;
};

}
