#include "rt_state_machine.hpp"
#include "captor.hpp"
#include <gtest/gtest.h>
#include <gmock/gmock.h>

namespace eul::Test
{
using namespace testing;


class RTStateMock : public state
{
public:
    RTStateMock()
    {
        ConnectCallbacksToMocks();
    }

    RTStateMock(std::initializer_list<std::reference_wrapper<state>> children) :
        state(children)
    {
        ConnectCallbacksToMocks();
    }

    std::string_view name;

    MOCK_METHOD(expectation, entry, (const event&), (const));
    MOCK_METHOD(expectation, exit, (const event&), (const));
private:
    MockFunction<state::behaviour> bhv_entry;
    MockFunction<state::behaviour> bhv_exit;
    void ConnectCallbacksToMocks()
    {
        state::entry(bhv_entry.AsStdFunction());
        state::exit(bhv_exit.AsStdFunction());

        ON_CALL(bhv_entry, Call(_, _)).WillByDefault(WithArg<1>([this](const event& _event)
        {
            return RTStateMock::entry(_event);
        }));
        ON_CALL(bhv_exit, Call(_, _)).WillByDefault(WithArg<1>([this](const event& _event)
        {
            return RTStateMock::exit(_event);
        }));        
    }
};

TEST(rt_state_machine, initial)
{    
    state on;
    MockFunction<state::behaviour> enterOn;
    on.entry(enterOn.AsStdFunction());

    state off;
    MockFunction<state::behaviour> enterOff;
    off.entry(enterOff.AsStdFunction());

    EXPECT_CALL(enterOff, Call(_,machine::construction));
    EXPECT_CALL(enterOn, Call(_,_)).Times(0);
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
    state::transition off_button(off, button, on);
    //InSequence seq;
    EXPECT_CALL(off, exit(button));
    EXPECT_CALL(on, entry(button));
    EXPECT_CALL(solid, entry(button));

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
    state::transition button_on(off, button, on);
    state::transition warn_blink(sm, warning, blinking);
    sm.signal(button);
    ASSERT_TRUE(sm.in(on));

    InSequence seq;
    EXPECT_CALL(solid, exit(warning));
    EXPECT_CALL(blinking, entry(warning));
    EXPECT_CALL(on, entry(_)).Times(0);

    EXPECT_TRUE(sm.signal(warning));
    EXPECT_TRUE(sm.in(on));
    EXPECT_TRUE(sm.in(blinking));
}

TEST(rt_state_machine, internal_transitions_only_execute_behavior)
{
    RTStateMock on;
    RTStateMock off;
    machine sm { off, on};
    MockFunction<state::behaviour> testBehaviour;
    event test;
    state::guard_transition offTest(off, test, testBehaviour.AsStdFunction());
    EXPECT_CALL(testBehaviour, Call(_,test)).Times(1);
    EXPECT_CALL(off, exit(_)).Times(0);
    EXPECT_CALL(off, entry(_)).Times(0);

    ASSERT_TRUE(sm.in(off));
    EXPECT_TRUE(sm.signal(test));
}

TEST(rt_state_machine, guard_errors_are_propagated_to_signaller)
{
    RTStateMock solid { };    
    RTStateMock on { solid };
    machine sm { on};
    MockFunction<state::behaviour> testBehaviour;
    event test;
    state::guard_transition onTest(on, test, testBehaviour.AsStdFunction());
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
    MockFunction<state::behaviour> guard;
    state::guard_transition off_button(off, button, on, guard.AsStdFunction());
    EXPECT_CALL(guard, Call(_,_)).WillOnce(Return(EACCES));
    InSequence seq;
    EXPECT_CALL(off, exit(_)).Times(0);
    EXPECT_CALL(on, entry(_)).Times(0);
    EXPECT_ERROR(sm.signal(button), ECHILD);
    EXPECT_TRUE(sm.in(off));
}

expectation free_guard(bool* called, const state&, const event&)
{
    if (*called) 
        return std::unexpected(EALREADY);
    *called = true;
    return as_expected;
}


TEST(rt_state_machine, guard_can_be_any_invocable)
{
    state from;    
    state to;
    machine sm { from, to };
    bool called = false;
    captor<expectation(*)(bool*, const state&, const event&), bool*> guard(free_guard, &called);
    event test;
    state::guard_transition<decltype(guard)> guardTest(from, test, to, guard);
    EXPECTED(sm.signal(test));
    EXPECT_TRUE(sm.in(to));
    EXPECT_TRUE(called);

}

TEST(rt_state_machine, exit_errors_prevent_transition)
{
    RTStateMock on;
    RTStateMock off;
    machine sm { off, on};
    event button;
    state::transition button_on(off, button, on);
    EXPECT_CALL(off, exit(_)).Times(1).WillOnce(Return(std::unexpected(EBADF)));
    EXPECT_CALL(on, entry(_)).Times(0);
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
    state::transition button_on(off, button, on);
    EXPECT_CALL(on, entry(button)).Times(1).WillOnce(Return(std::unexpected(EBADMSG)));
    EXPECT_EQ(sm.signal(button).error(), EBADMSG);
    EXPECT_FALSE(sm.in(on));
    EXPECT_FALSE(sm.in(off));
}


}
