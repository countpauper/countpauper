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

TEST(singleton, one_instance)
{
    singleton<State> single;
    single->state = true;
    EXPECT_EQ(singleton<State>::Get().state, true);
    singleton<State> second; 
    EXPECT_EQ(second->state, true);
}

TEST(singleton, with_parameters)
{
    singleton<State> single(true);
    EXPECT_EQ(singleton<State>::Get().state, true);
    singleton<State> second; 
    EXPECT_EQ(second->state, true);
}


}