#pragma once
#include <gmock/gmock.h>

#include "Game/HeightMap.h"
#include "Game/Block.h"

namespace Game::Test
{
using namespace ::testing;


MATCHER_P(IsPositionAbove, p, "")
{
    *result_listener << "where the vector is (" << (arg.p.x - p.x) << ", " << (arg.p.y - p.y) << ")";
    return (arg.p.x == p.x && arg.p.y == p.y && arg.Z() > p.z);
}


class MockMap : public HeightMap
{
public:
    MockMap()
    {
        ON_CALL(*this,  GetBlock(_)).WillByDefault(ReturnRef(Block::Space));
    }

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
        ON_CALL(*this, GetBlock(_)).WillByDefault(WithArg<0>([this](Engine::Position p) -> const Block&
        {
            auto h = GroundHeight(Position(p));
            if (h <= p.z)    // todo fraction of h
                return Block::Air;   
            else    
                return Block::Stone;
        }));
    }
    MOCK_METHOD(Engine::IntBox, GetBounds, (), (const override));
    MOCK_METHOD(float, GroundHeight, (Position pos), (const override));
    MOCK_METHOD(const Block&, GetBlock, (Engine::Position pos), (const override));

};

}
