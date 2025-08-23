#include "rt_state_machine.hpp"
#include <gtest/gtest.h>
#include <gmock/gmock.h>


// TODO: 
// Must be able to implement entry and exit with error handling 
// Must not use heap 

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
    MOCK_METHOD(expectation, on, (const event&), (override));
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

TEST(rt_state_machine, non_transitioning_events_are_sent_to_active_state)
{
    RTStateMock on;
    RTStateMock off;
    machine sm { off, on};
    event test;
    EXPECT_CALL(off, on(test)).Times(1);
    EXPECT_CALL(on, on(_)).Times(0);
    ASSERT_TRUE(sm.in(off));
    EXPECT_TRUE(sm.signal(test));
}

TEST(rt_state_machine, signal_errors_are_propagated)
{
    RTStateMock solid { };    
    RTStateMock on { solid };
    machine sm { on};
    event test;
    EXPECT_CALL(on, on(test)).Times(1).WillOnce(Return(std::unexpected(EBADF)));
    EXPECT_CALL(solid, on(test)).Times(0);
    EXPECT_EQ(sm.signal(test).error(), EBADF);
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
