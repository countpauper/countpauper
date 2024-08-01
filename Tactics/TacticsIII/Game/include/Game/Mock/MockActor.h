#pragma once
#include <gmock/gmock.h>

#include "Game/Actor.h"
#include "MockStatted.h"
#include "MockCounted.h"
#include "UI/Mock/MockScenery.h"

namespace Game::Test
{
using namespace ::testing;

class MockActor : public Actor
{
public:
    MockActor()
    {
        ON_CALL(*this, Move(_, _)).WillByDefault(Invoke([this](const class HeightMap&, Engine::Position destination)
        {
            ON_CALL(*this, Position()).WillByDefault(Return(destination));
        }));
    }

    MockActor(std::string_view name) :
        MockActor()
    {
        SetName(name);
    }

    void SetName(std::string_view name)
    {
        EXPECT_CALL(appearance, Name()).WillRepeatedly(Return(name));
        EXPECT_CALL(stats, Name()).WillRepeatedly(Return(name));
    }

    MOCK_METHOD(void, Move, (const class HeightMap& map, Engine::Position destination), (override));
    MOCK_METHOD(Engine::Position, Position, (), (const override));

    const Engine::Scenery& GetAppearance() const override { return appearance; }
    Statted& GetStats() { return stats; }
    const Statted& GetStats() const { return stats; }
    Counted& GetCounts() { return counts; }
    const Counted& GetCounts() const { return counts; }

    Engine::Test::MockScenery appearance;
    MockStatted stats;
    MockCounted counts;

};

}
