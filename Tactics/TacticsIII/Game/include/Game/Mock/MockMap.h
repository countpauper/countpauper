#pragma once
#include <gmock/gmock.h>

#include "Game/HeightMap.h"

namespace Game::Test
{
using namespace ::testing;


MATCHER_P(IsPositionAbove, p, "")
{
    *result_listener << "where the vector is (" << arg.x - p.x << ", " << arg.y - p.y << ")";
    return (arg.x == p.x && arg.y == p.y && arg.z > p.z);
}


class MockMap : public HeightMap
{
public:
    MockMap() = default;

    void SetHeightMap(Engine::Size size, std::initializer_list<float> heights={})
    {
        ON_CALL(*this, GetBounds()).WillByDefault(Return(Engine::IntBox(size)));
        unsigned idx=0;
        for(auto height: heights)
        {
            unsigned x = idx % size.x;
            unsigned y = idx / size.x;
            ON_CALL(*this, GroundHeight(IsPositionAbove(Engine::Position(x,y,-1)))).WillByDefault(Return(height));
            ++idx;
        }
    }
    MOCK_METHOD(Engine::IntBox, GetBounds, (), (const override));
    MOCK_METHOD(float, GroundHeight, (Engine::Position pos), (const override));
};

}
