#pragma once
#include <gmock/gmock.h>

#include "Game/HeightMap.h"

namespace Game::Test
{
using namespace ::testing;

class MockMap : public HeightMap
{
public:
    MockMap()
    {
        EXPECT_CALL(*this, GetMaterial(_)).WillRepeatedly(ReturnRef(Material::air));
    }
    MockMap(Engine::Size size) : MockMap()
    {
        EXPECT_CALL(*this, GetSize()).WillRepeatedly(Return(size));
    }
    MOCK_METHOD(Engine::Size, GetSize, (), (const override));
    MOCK_METHOD(Engine::Coordinate, GroundCoord, (Engine::Position pos), (const override));
    MOCK_METHOD(int, GroundHeight, (Engine::Position pos), (const override));
    MOCK_METHOD(const Material&, GetMaterial, (Engine::Position pos), (const override));
};

}
