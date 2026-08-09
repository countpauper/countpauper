#pragma once
#include <gmock/gmock.h>

#include "Game/MapItf.h"
#include "Game/Stack.h"

namespace Game::Test
{
using namespace ::testing;


MATCHER_P(IsPositionAbove, p, "")
{
    *result_listener << "where the vector is (" << (arg.p.x - p.x) << ", " << (arg.p.y - p.y) << ")";
    return (arg.p.x == p.x && arg.p.y == p.y && arg.Z() > p.z);
}


class MockMap : public MapItf
{
public:
    MockMap()
    {
        Stack stack;
        ON_CALL(*this, StackAt(_,_)).WillByDefault([stack](int, int) -> const Stack&
        {
            return stack;
        });
    }

    void SetSize(Engine::Size size)
    {
        ON_CALL(*this, GetBounds()).WillByDefault(Return(Engine::IntBox(size)));
    }

    void SetHeightMap(Engine::Size size, std::initializer_list<ZType> heights={})
    {
        SetSize(size);

        std::vector<Stack> slices;
        slices.clear();
        unsigned heightIndex=0;
        ZType mapHeight {size.z};

        for(int y=0;y<size.y; ++y)
        {
            for (int x=0;x<size.x; ++x)
            {
                auto heightIt = heights.begin()+(heightIndex % heights.size());
                slices.push_back(Stack({{Material::stone, *heightIt, 300.0f}, 
                            {Material::air, mapHeight - *heightIt, 300.0f}}));
                ++heightIndex;
            }
        }
        ON_CALL(*this, StackAt(_,_)).WillByDefault([slices, size](int x, int y) -> const Stack&
        {
            return slices.at(x + y * size.y);
        });
    }
    MOCK_METHOD(Engine::IntBox, GetBounds, (), (const override));
    MOCK_METHOD(const Stack&, StackAt, (int,int), (const override));
private:

};

}
