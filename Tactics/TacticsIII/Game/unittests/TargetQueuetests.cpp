#include "Game/TargetQueue.h"
#include "Game/Position.h"
#include "Game/Mock/MockActor.h"
#include <gtest/gtest.h>

namespace Game::Test
{


TEST(TargetQueue, Void)
{
    EXPECT_TRUE(TargetQueue<>().IsComplete());
    EXPECT_TRUE(TargetQueue<>().IsEmpty());
    EXPECT_EQ(TargetQueue<>().as_tuple(), std::tuple<>());

}

TEST(TargetQueue, Position)
{
    TargetQueue<Position> queue;
    EXPECT_FALSE(queue.IsComplete());
    EXPECT_TRUE(queue.IsEmpty());
    EXPECT_THROW(queue.pop(), std::runtime_error);
    queue.push_back(Position{1,2,3});
    EXPECT_FALSE(queue.IsEmpty());
    EXPECT_TRUE(queue.IsComplete());
    EXPECT_EQ(queue.as_tuple(), std::make_tuple(Position{1,2,3}));
    EXPECT_EQ(std::get<Position>(queue[0]), Position(1,2,3));
    EXPECT_EQ(std::get<Position>(queue.pop()), Position(1,2,3));
    EXPECT_TRUE(queue.IsEmpty());
    EXPECT_FALSE(queue.IsComplete());
}

TEST(TargetQueue, TargetAndPosition)
{
    TargetQueue<Actor*, Position> queue;
    EXPECT_THROW(queue.push_back(Position(-1,0,-3)), std::bad_variant_access);
    EXPECT_TRUE(queue.IsEmpty());
    MockActor actor;
    queue.push_back(&actor);
    EXPECT_FALSE(queue.IsEmpty());
    EXPECT_FALSE(queue.IsComplete());
    queue.push_back(Position(-1,0,-3));
    EXPECT_TRUE(queue.IsComplete());
    EXPECT_EQ(std::get<Actor*>(queue[0]), &actor);
    EXPECT_EQ(std::get<Position>(queue[1]), Position(-1,0, -3));
    EXPECT_EQ(queue.as_tuple(), std::make_tuple(static_cast<Actor*>(&actor), Position(-1, 0, -3)));
}


TEST(TargetQueue, VariadicTarget)
{
    using TargetOptions = std::variant<Actor*, Position>;
    TargetQueue<TargetOptions> queue;
    queue.push_back(Position(-4,2,1));
    EXPECT_TRUE(queue.IsComplete());
    EXPECT_EQ(std::get<Position>(std::get<TargetOptions>(queue.pop())), Position(-4,2,1));
    ASSERT_TRUE(queue.IsEmpty());
    MockActor actor;
    queue.push_back(&actor);
    EXPECT_TRUE(queue.IsComplete());
    EXPECT_EQ(std::get<Actor*>(std::get<TargetOptions>(queue[0])), &actor);
}

}
