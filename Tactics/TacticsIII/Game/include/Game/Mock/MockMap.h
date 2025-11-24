#pragma once
#include <gmock/gmock.h>

#include "Game/HeightMap.h"

namespace Game::Test
{
using namespace ::testing;

class MockMap : public HeightMap
{
public:
    MockMap(float height=0)
    {
        EXPECT_CALL(*this, GroundHeight(_)).WillRepeatedly(Return(height));
    }
    MockMap(Engine::Size size) : MockMap(0)
    {
        EXPECT_CALL(*this, GetBounds()).WillRepeatedly(Return(Engine::IntBox(size)));
    }
    MOCK_METHOD(Engine::IntBox, GetBounds, (), (const override));
    MOCK_METHOD(Engine::Coordinate, GroundCoord, (Engine::Position pos), (const override));
    MOCK_METHOD(float, GroundHeight, (Engine::Position pos), (const override));
};

}
