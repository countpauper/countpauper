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


class MockMap : public BlockMap
{
public:
    MockMap()
    {
        ON_CALL(*this,  GetBlock(_)).WillByDefault(Return(Block::Space));
    }

    void SetSize(Engine::Size size)
    {
        ON_CALL(*this, GetBounds()).WillByDefault(Return(Engine::IntBox(size)));
    }

    void SetHeightMap(Engine::Size size, std::initializer_list<float> heights={})
    {
        SetSize(size);
        std::vector<float> heightCopy(heights);
        ON_CALL(*this, GetBlock(_)).WillByDefault(WithArg<0>([this, size, heightCopy](Engine::Position p) -> Block
        {
            auto index = p.X() + p.Y() * size.X();
            float intHeight;
            float fracHeight = modf(heightCopy.at(index), &intHeight);

            if (p.z < intHeight)    // todo fraction of h
                return Block::Stone;
            else if (p.z > intHeight)   
                return Block::Air;
            else 
                return Block(fracHeight);

        }));
    }
    MOCK_METHOD(Engine::IntBox, GetBounds, (), (const override));
    MOCK_METHOD(Block, GetBlock, (Engine::Position pos), (const override));

private:

};

}
