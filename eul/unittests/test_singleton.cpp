#include <gtest/gtest.h>
#include "singleton.hpp"
#include "copy_counter.hpp"

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

TEST(singleton, constructed_in_place)
{
    copy_count count;
    {
        singleton<copy_counter> single(count);
        EXPECT_EQ((*single)->current(), 1);
        EXPECT_EQ(count.assigned, 0);
        EXPECT_EQ(count.moved, 0);
    }
    EXPECT_EQ(count.destructed, 0);
    EXPECT_EQ(count.current(), 1);
}   

}