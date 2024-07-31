#pragma once
#include <gmock/gmock.h>
#include "UI/Scenery.h"
#include "Geometry/Mesh.h"

namespace Engine::Test
{

class MockScenery : public Scenery
{
public:
    MockScenery() : Scenery(mesh)
    {
    }
    MOCK_METHOD(std::string_view, Name, (), (const override));
    MOCK_METHOD(void, Render, (), (const override));
    MOCK_METHOD(Coordinate, GetCoordinate,(), (const override));
    MOCK_METHOD(Quaternion, GetOrientation,(), (const override));

    Engine::Mesh mesh;
};

}
