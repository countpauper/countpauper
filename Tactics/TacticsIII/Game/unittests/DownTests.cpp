#include "Game/Down.h"
#include "Game/Mock/MockActor.h"
#include "Game/Mock/MockWorld.h"
#include "Definition.h"
#include <gtest/gtest.h>

namespace Game::Test
{
using namespace ::testing;

TEST(Down, can_go_down)
{
    NiceMock<MockWorld> world;
    NiceMock<MockActor> actor("a");
    actor.SetStats({{Stat::ap, 1}});

    Down action(world, actor);
    EXPECT_EQ(action.Description(), "Go down");
    EXPECT_EQ(action.AP(), 1);
    EXPECT_TRUE(action.CanDo());

    std::stringstream log;
    auto deltas = action.Execute(log);
    EXPECT_EQ(log.str(), "a goes down");
    ASSERT_EQ(deltas.size(), 1);

    const auto& delta = deltas[0];
    EXPECT_EQ(delta.Available(Stat::ap), 0);
    EXPECT_TRUE(delta.GetConditions().Is<Downed>());
}

TEST(Down, cannot_go_down_without_ap)
{
    NiceMock<MockWorld> world;
    NiceMock<MockActor> actor("a");
    actor.SetStats({{Stat::ap, 0}});
    actor.Move(world, {0, 0, 0});

    EXPECT_CALL(actor.counts, Cost(_, _, _)).Times(0);

    Down action(world, actor);
    EXPECT_FALSE(action.CanDo());

    std::stringstream log;
    auto deltas = action.Execute(log);
    EXPECT_TRUE(deltas.empty());
    EXPECT_EQ(log.str(), "a stays up ");
}

}
