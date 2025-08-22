#include "rt_state_machine.hpp"
#include <gtest/gtest.h>
#include <gmock/gmock.h>


// TODO: Must support hierarchical states 
// Must support generic event system that could be linked to Bus 
// Must be able to implement entry and exit with error handling 
// Must not use heap 

namespace eul::Test
{
using namespace testing;

class RTStateMock : public State  
{
public:
    using State::State;
    std::string_view name; 

    MOCK_METHOD(void, Entry, (), (override));
    MOCK_METHOD(void, Exit, (), (override));
    MOCK_METHOD(void, OnEvent, (const Event&), (override));
};

TEST(RTStateMachine, Initial)
{    
    RTStateMock on;
    RTStateMock off;
    EXPECT_CALL(off, Entry());
    EXPECT_CALL(on, Entry()).Times(0);
    Machine sm { off, on};
    EXPECT_TRUE(sm.InState(off));
    EXPECT_TRUE(sm.InState(sm));
}

TEST(RTStateMachine, Transition)
{
    RTStateMock solid;
    State blinking;
    RTStateMock on { solid, blinking };
    RTStateMock off;
    Machine sm { off, on};
    Event button; 
    Transition off_button(off, button, on);
    InSequence seq;
    EXPECT_CALL(off, Exit());
    EXPECT_CALL(on, Entry());
    EXPECT_CALL(solid, Entry());

    EXPECT_TRUE(sm.Signal(button));
    EXPECT_TRUE(sm.InState(on));
    EXPECT_TRUE(sm.InState(solid));
}

TEST(RTStateMachine, ToSubstate)
{
    RTStateMock solid { };
    solid.name="solid";
    RTStateMock blinking { };
    blinking.name="blinking";
    RTStateMock on { solid, blinking };
    on.name="on";
    State off;
    Machine sm { off, on};
    Event button;
    Event warning;
    Transition button_on(off, button, on);
    Transition warn_blink(sm, warning, blinking);
    sm.Signal(button);
    ASSERT_TRUE(sm.InState(on));

    InSequence seq;
    EXPECT_CALL(solid, Exit());
    EXPECT_CALL(blinking, Entry());
    EXPECT_CALL(on, Entry()).Times(0);

    EXPECT_TRUE(sm.Signal(warning));
    EXPECT_TRUE(sm.InState(on));
    EXPECT_TRUE(sm.InState(blinking));
}

TEST(RTStateMachine, NonTransitionEventsAreSentToActiveState)
{
    RTStateMock on;
    RTStateMock off;
    Machine sm { off, on};
    Event test;
    EXPECT_CALL(off, OnEvent(test)).Times(1);
    EXPECT_CALL(on, OnEvent(_)).Times(0);
    ASSERT_TRUE(sm.InState(off));
    sm.Signal(test);
}

}
