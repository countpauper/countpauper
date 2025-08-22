#include "ct_state_machine.hpp"
#include <gtest/gtest.h>
#include <gmock/gmock.h>

namespace eul::Test
{

using namespace testing;

class StateMock 
{
public:
    MOCK_METHOD(void, OnEvent, (StateIF& state, const Event&));    
};

class Solid : public State<> {  };
class Blinking : public State<> { }; 
class On : public State<Solid, Blinking> { };
class Off : public State<> 
{ 
public:
    StateMock* mock = nullptr;  // TODO: mock reference should not be copyable, just movable. Maybe just unique ptr?
    void OnEvent(const Event& event) override
    {
        if (mock)
            mock->OnEvent(*this, event);
    }
};

Event button;
Event warning; 

TEST(CTStateMachine, Initial)
{
    Machine<Off, On> sm {};
    EXPECT_TRUE(sm.InState<Off>());
}

TEST(CTStateMachine, Concepts)
{
    static_assert(is_branch_state<On>);
    static_assert(!is_leaf_state<On>);
    static_assert(!is_branch_state<Off>);
    static_assert(is_leaf_state<Off>);
}

TEST(CTStateMachine, Transition)
{
    Machine<Off, On> sm;
    sm.Transition<Off,On>(button);
    EXPECT_TRUE(sm.Signal(button));
    EXPECT_TRUE(sm.InState<On>());
    EXPECT_TRUE(sm.InState<Solid>());
}

TEST(CTStateMachine, DISABLED_ToSubstate)
{
    // TODO: to implement this 
    // 1) Transitions should encode a chain of alternative indices to switch to 
    // 2) This chain should be constructed at the construction of the transition using recursively calling variant_index_of
    // 3) When executing the transition the variants should be constructed in sequence
    // 4) The Parent states should support a constructor State(substates).
    // 5) If the parent wants its own constructor it could also be a free function wrapper to construct the substate and call the parent state with that (moved)
    // 6) This constructor should be able to be constexpr since it just constructs the next variant alternative 
    // 7) This is called to emplace a new variant *unless* the variant index is already the current variant (similar test to test_rt_state_machine)
    // 8) As an optimization the sequence could be encoded in a uint32_t which uses number%std::variant_size() and the next is *variant_size()
    // 9) There is a maximum depth/amount of substates however (static_assert).
    // 10) Optionally transitions could also (be templated and) use a 64 bit substate sequence. If the constructors are constexpr then its size doesn't matter there 
    Machine<Off, On> sm;
    sm.Transition<void,Blinking>(warning);
    EXPECT_TRUE(sm.Signal(warning));
    EXPECT_TRUE(sm.InState<On>());
    EXPECT_TRUE(sm.InState<Blinking>());
}

TEST(CTStateMachine, NonTransitionEventsAreSentToActiveState)
{
    Event test;
    Machine<Off, On> sm;
    StateMock mock;
    dynamic_cast<Off&>(sm.GetState()).mock = &mock;
    EXPECT_CALL(mock, OnEvent(_, test)).Times(1);
    sm.Signal(test);
}

}
