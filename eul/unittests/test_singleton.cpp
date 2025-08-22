#include <gtest/gtest.h>
#include "singleton.hpp"

namespace eul::Test
{

struct State 
{
    State() : state(false) {}
    State(bool initial) : state(initial) {}
    bool state = false;
};

TEST(Singleton, OneInstance)
{
    Singleton<State> single;
    single->state = true;
    EXPECT_EQ(Singleton<State>::Get().state, true);
    Singleton<State> second; 
    EXPECT_EQ(second->state, true);
}

TEST(Singleton, WithParameters)
{
    Singleton<State> single(true);
    EXPECT_EQ(Singleton<State>::Get().state, true);
    Singleton<State> second; 
    EXPECT_EQ(second->state, true);
}


}