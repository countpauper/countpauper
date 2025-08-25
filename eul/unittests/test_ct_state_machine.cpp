#include "ct_state_machine.hpp"
#include <gtest/gtest.h>
#include <gmock/gmock.h>

namespace eul::Test
{

using namespace testing;

class StateMock 
{
public:
    MOCK_METHOD(expectation, on, (stateIF& state, const event&));    
};

class MockChild : public state<> 
{ 
public:
    using state::state;
    StateMock* mock = nullptr;  // TODO: mock reference should not be copyable, just movable. Maybe just unique ptr?
    expectation on(const event& ev) override
    {
        if (mock)
            return mock->on(*this, ev);
        else 
            return std::unexpected(ENODEV);
    }
};

class Solid : public MockChild { };
class Blinking : public MockChild { }; 
class On : public state<Solid, Blinking> { }; 
class Off : public MockChild { } ;


event button;
event warning; 
event test; 

TEST(ct_state_machine, initial)
{
    machine<Off, On> sm {};
    EXPECT_TRUE(sm.in<Off>());
}

TEST(ct_state_machine, concepts)
{
    static_assert(is_branch_state<On>);
    static_assert(!is_leaf_state<On>);
    static_assert(!is_branch_state<Off>);
    static_assert(is_leaf_state<Off>);
}

TEST(ct_state_machine, transition)
{
    machine<Off, On> sm {};
    auto t = sm.transition<Off,On>(button);
    EXPECT_TRUE(sm.signal(button));
    EXPECT_TRUE(sm.in<On>());
    EXPECT_TRUE(sm.in<Solid>());
}

TEST(ct_state_machine, to_sub_state)
{
    machine<Off, On> sm;
    auto t = sm.transition<Blinking>(warning);
    EXPECT_TRUE(sm.signal(warning));
    EXPECT_TRUE(sm.in<On>());
    EXPECT_TRUE(sm.in<Blinking>());
}


TEST(ct_state_machine, signal_sub_state)
{
    machine<Off, On> sm;
    auto t = sm.transition<Off,On>(button);
    EXPECT_TRUE(sm.change<On>());
    ASSERT_TRUE(sm.in<On>());
    StateMock mock;
    dynamic_cast<Solid&>(sm.get()).mock = &mock;    
    EXPECT_CALL(mock, on(_, test)).Times(1);
    EXPECT_TRUE(sm.signal(test));
}

TEST(ct_state_machine, non_transition_events_are_sent_to_active_state)
{
    machine<Off, On> sm;
    StateMock mock;
    dynamic_cast<Off&>(sm.get()).mock = &mock;
    EXPECT_CALL(mock, on(_, test)).Times(1);
    sm.signal(test);
}

}
