#include "rt_state_machine.hpp"
#include <gtest/gtest.h>
#include <gmock/gmock.h>

namespace eul::Test
{
using namespace testing;

class RTStateMock : public state  
{
public:
    using state::state;
    std::string_view name; 

    MOCK_METHOD(expectation, entry, (), (override));
    MOCK_METHOD(expectation, exit, (), (override));
};

TEST(rt_state_machine, initial)
{    
    RTStateMock on;
    RTStateMock off;
    EXPECT_CALL(off, entry());
    EXPECT_CALL(on, entry()).Times(0);
    machine sm { off, on};
    EXPECT_TRUE(sm.in(off));
    EXPECT_TRUE(sm.in(sm));
}

TEST(rt_state_machine, transition)
{
    RTStateMock solid;
    state blinking;
    RTStateMock on { solid, blinking };
    RTStateMock off;
    machine sm { off, on};
    event button; 
    transition off_button(off, button, on);
    InSequence seq;
    EXPECT_CALL(off, exit());
    EXPECT_CALL(on, entry());
    EXPECT_CALL(solid, entry());

    EXPECT_TRUE(sm.signal(button));
    EXPECT_TRUE(sm.in(on));
    EXPECT_TRUE(sm.in(solid));
}

TEST(rt_state_machine, to_sub_state)
{
    RTStateMock solid { };
    solid.name="solid";
    RTStateMock blinking { };
    blinking.name="blinking";
    RTStateMock on { solid, blinking };
    on.name="on";
    state off;
    machine sm { off, on};
    event button;
    event warning;
    transition button_on(off, button, on);
    transition warn_blink(sm, warning, blinking);
    sm.signal(button);
    ASSERT_TRUE(sm.in(on));

    InSequence seq;
    EXPECT_CALL(solid, exit());
    EXPECT_CALL(blinking, entry());
    EXPECT_CALL(on, entry()).Times(0);

    EXPECT_TRUE(sm.signal(warning));
    EXPECT_TRUE(sm.in(on));
    EXPECT_TRUE(sm.in(blinking));
}

TEST(rt_state_machine, internal_transitions_only_execute_behavior)
{
    RTStateMock on;
    RTStateMock off;
    machine sm { off, on};
    MockFunction<transition::BEHAVIOUR> testBehaviour;
    event test;
    transition offTest(off, test, testBehaviour.AsStdFunction());
    EXPECT_CALL(testBehaviour, Call(_,test)).Times(1);
    EXPECT_CALL(off, exit()).Times(0);
    EXPECT_CALL(off, entry()).Times(0);

    ASSERT_TRUE(sm.in(off));
    EXPECT_TRUE(sm.signal(test));
}

TEST(rt_state_machine, behaviour_errors_are_propagated_to_signaller)
{
    RTStateMock solid { };    
    RTStateMock on { solid };
    machine sm { on};
    MockFunction<transition::BEHAVIOUR> testBehaviour;
    event test;
    transition onTest(on, test, testBehaviour.AsStdFunction());
    EXPECT_CALL(testBehaviour, Call(_,test)).WillOnce(Return(std::unexpected(EBADF)));
    EXPECT_ERROR(sm.signal(test), EBADF);
}

TEST(rt_state_machine, guard_returning_EACCESS_blocks_transition)
{
    RTStateMock solid;
    state blinking;
    RTStateMock on { solid, blinking };
    RTStateMock off;
    machine sm { off, on};
    event button; 
    MockFunction<transition::BEHAVIOUR> guard;
    transition off_button(off, button, on, guard.AsStdFunction());
    EXPECT_CALL(guard, Call(_,_)).WillOnce(Return(EACCES));
    InSequence seq;
    EXPECT_CALL(off, exit()).Times(0);
    EXPECT_CALL(on, entry()).Times(0);
    EXPECT_ERROR(sm.signal(button), ECHILD);
    EXPECT_TRUE(sm.in(off));
}
TEST(rt_state_machine, exit_errors_prevent_transition)
{
    RTStateMock on;
    RTStateMock off;
    machine sm { off, on};
    event button;
    transition button_on(off, button, on);
    EXPECT_CALL(off, exit()).Times(1).WillOnce(Return(std::unexpected(EBADF)));
    EXPECT_CALL(on, entry()).Times(0);
    EXPECT_EQ(sm.signal(button).error(), EBADF);
    EXPECT_FALSE(sm.in(on));
    EXPECT_FALSE(sm.in(off));
}

TEST(rt_state_machine, entry_errors_prevent_transition)
{
    RTStateMock on;
    RTStateMock off;
    machine sm { off, on};
    event button;
    transition button_on(off, button, on);
    EXPECT_CALL(on, entry()).Times(1).WillOnce(Return(std::unexpected(EBADMSG)));
    EXPECT_EQ(sm.signal(button).error(), EBADMSG);
    EXPECT_FALSE(sm.in(on));
    EXPECT_FALSE(sm.in(off));
}

}
