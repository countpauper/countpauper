#include "Game/Move.h"
#include "Game/Mock/MockActor.h"
#include "Game/Mock/MockWorld.h"
#include <gtest/gtest.h>


namespace Game::Test
{
using namespace ::testing;



TEST(Move, step)
{
    MockActor actor("a");
    actor.SetStats({
        {Stat::speed, 2},
        {Stat::jump, 1},
        {Stat::ap, 1}
    });

    NiceMock<MockWorld> world;
    world.map.SetHeightMap({2,2,2}, {0, 2.0, 0.0, 1.0});

    Move action(world, actor, Engine::Position(1,1,0));
    EXPECT_EQ(action.Description(), "Move (1, 1, 0)");
    EXPECT_EQ(action.AP(), 1);
    EXPECT_TRUE(action.CanDo());

    std::stringstream log;
    auto delta = action.Execute(log).front();
    EXPECT_EQ(log.str(), "a moves to (1, 1, 0)\n");
    EXPECT_EQ(delta.Position(), Engine::Position(1,1,0));
    EXPECT_EQ(delta.Available(Stat::ap), 0);
}

TEST(Move, cant_move_with_speed_0)
{
    MockActor actor("b");
    actor.SetStats({
        {Stat::speed, 0},
        {Stat::jump, 1},
        {Stat::ap, 1}
    });
    NiceMock<MockWorld> world;

    Move action(world, actor, Engine::Position(1,1,0));
    EXPECT_FALSE(action.CanDo());
    std::stringstream log;
    auto deltas = action.Execute(log);
    EXPECT_TRUE(deltas.empty());
    EXPECT_EQ(log.str(), "b can't move, because speed (0) is 0\n");
}

TEST(Move, cant_jump_too_high)
{
    MockActor actor("b");
    actor.SetStats({
        {Stat::speed, 1},
        {Stat::jump, 1},
        {Stat::ap, 1}
    });
    NiceMock<MockWorld> world;
    world.map.SetHeightMap({2,1,4}, {0, 2.0});
    Move action(world, actor, Engine::Position(1,0,2));
    EXPECT_FALSE(action.CanDo());
    std::stringstream log;
    auto deltas = action.Execute(log);
    EXPECT_TRUE(deltas.empty());
    EXPECT_EQ(log.str(), "b can't move, because the destination is not reachable\n");
}
}
