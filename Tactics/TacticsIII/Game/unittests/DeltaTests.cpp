#include "Game/Delta.h"
#include "Game/Mock/MockActor.h"
#include "Game/Mock/MockWorld.h"
#include <gtest/gtest.h>
#include <iterator>

namespace Game::Test
{


TEST(Delta, Move)
{
    MockActor actor;
    Delta delta(actor);

    MockWorld world;
    Position destination(3, 4, 0);
    delta.Move(world, destination);
    
    EXPECT_CALL(actor, Move(Ref(world), destination));
    delta.Apply();
}

TEST(Delta, Counter)
{
    MockActor actor;
    EXPECT_CALL(actor.counts, Available(Stat::Id::ap)).WillRepeatedly(Return(2));

    Delta delta(actor);
    EXPECT_EQ(delta.Cost(Stat::Id::ap, 1, true), 1);
    EXPECT_EQ(delta.Available(Stat::Id::ap), 1);

    EXPECT_CALL(actor.counts, Cost(Stat::Id::ap, 1u, true)).WillOnce(Return(1u));
    delta.Apply();
}

TEST(Delta, GainCondition)
{
    MockActor actor;

    Delta delta(actor);
    EXPECT_CALL(actor.conditions, GetCondition(_)).Times(1);
    EXPECT_FALSE(delta.GetConditions().Is<Downed>());

    delta.GetConditions().Apply<Downed>();
    EXPECT_TRUE(delta.GetConditions().Is<Downed>());

    EXPECT_CALL(actor.conditions, SetCondition(Ref(*Engine::Singleton<Downed>()), 1U));
    delta.Apply();
}

TEST(Delta, LoseCondition)
{
    MockActor actor;
    EXPECT_CALL(actor.conditions, GetCondition(_)).WillOnce(Return(1u));

    Delta delta(actor);
    EXPECT_TRUE(delta.GetConditions().Is<KO>());

    delta.GetConditions().Apply<KO>(0);
    EXPECT_FALSE(delta.GetConditions().Is<KO>());

    EXPECT_CALL(actor.conditions, SetCondition(Ref(*Engine::Singleton<KO>()), 0U));
    delta.Apply();
}

}
