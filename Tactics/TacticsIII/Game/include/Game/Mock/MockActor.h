#pragma once
#include <gmock/gmock.h>

#include "Game/Actor.h"
#include "MockStatted.h"
#include "MockCounted.h"
#include "MockEquipped.h"
#include "UI/Mock/MockScenery.h"
#include <unordered_map>

namespace Game::Test
{
using namespace ::testing;

class MockActor : public Actor
{
public:
    MockActor()
    {
        ON_CALL(*this, Move(_, _)).WillByDefault(Invoke([this](const class World&, Engine::Position destination)
        {
            ON_CALL(*this, Position()).WillByDefault(Return(destination));
        }));
        ON_CALL(stats, Get(_, _, _)).WillByDefault(Return(Computation(0)));
    }

    MockActor(std::string_view name) :
        MockActor()
    {
        SetName(name);
    }

    void SetName(std::string_view name)
    {
        ON_CALL(appearance, Name()).WillByDefault(Return(name));
        ON_CALL(stats, Name()).WillByDefault(Return(name));
    }

    void SetStats(std::unordered_map<Stat::Id, int> _stats)
    {
        for(const auto& [stat, value]: _stats)
        {
            ON_CALL(stats, Get(stat, _, _)).WillByDefault(Return(Computation(value)));
            ON_CALL(counts, Available(stat)).WillByDefault(Return(value));
        }
    }

    MOCK_METHOD(void, Move, (const class World& world, Engine::Position destination), (override));
    MOCK_METHOD(Engine::Position, Position, (), (const override));
    MOCK_METHOD(Engine::Size, Size, (), (const override));

    const Engine::Scenery& GetAppearance() const override { return appearance; }
    Statted& GetStats() { return stats; }
    const Statted& GetStats() const { return stats; }
    Counted& GetCounts() { return counts; }
    const Counted& GetCounts() const { return counts; }
    const Equipped& GetEquipment() const { return equipment; }
    Equipped& GetEquipment() { return equipment; }

    Engine::Test::MockScenery appearance;
    NiceMock<MockStatted> stats;
    NiceMock<MockCounted> counts;
    NiceMock<MockEquipped> equipment;
};

}
