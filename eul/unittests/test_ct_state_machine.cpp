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
class On : public state<Solid, Blinking> { using state::state; };
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
    machine<Off, On> sm;
    sm.transition<Off,On>(button);
    EXPECT_TRUE(sm.signal(button));
    EXPECT_TRUE(sm.in<On>());
    EXPECT_TRUE(sm.in<Solid>());
}

TEST(ct_state_machine, to_sub_state)
{
    // TODO: to implement this 
    // 1) [done] Transitions should encode a chain of alternative indices to switch to 
    // 2) [done] This chain should be constructed at the construction of the transition using recursively calling variant_index_of
    // 3) When executing the transition the variants should be constructed in sequence
    // 4) The Parent states should support a constructor State(substates).
    // 5) If the parent wants its own constructor it could also be a free function wrapper to construct the substate and call the parent state with that (moved)
    // 6) This constructor should be able to be constexpr since it just constructs the next variant alternative 
    // 7) This is called to emplace a new variant *unless* the variant index is already the current variant (similar test to test_rt_state_machine)
    // 8) [done] As an optimization the sequence could be encoded in a uint32_t which uses number%std::variant_size() and the next is *variant_size()
    // 9) [done] There is a maximum depth/amount of substates however (static_assert).
    // 10) Optionally transitions could also (be templated and) use a 64 bit substate sequence. If the constructors are constexpr then its size doesn't matter there 
    machine<Off, On> sm;
    sm.transition<void,Blinking>(warning);
    EXPECT_TRUE(sm.signal(warning));
    EXPECT_TRUE(sm.in<On>());
    EXPECT_TRUE(sm.in<Blinking>());
}


TEST(ct_state_machine, DISABLED_signal_sub_state)
{
    machine<Off, On> sm;
    sm.transition<Off,On>(button);
    ASSERT_TRUE(sm.signal(button));
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
