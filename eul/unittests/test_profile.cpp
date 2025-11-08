#include "profile.hpp"
#include "mock_timer.hpp"
#include <gtest/gtest.h>

namespace eul::Test
{

using namespace ::testing;

TEST(profile, start_stop)
{
    /* TODO unimplemented
    timer_mock tim;
    profile p(tim);
    EXPECT_CALL(tim, Time()).WillOnce(Return(0.001f));
    static profile::point start(p, "start");
    EXPECT_CALL(tim, Time()).WillOnce(Return(0.0013f));
    static profile::point end(p, "end");
    EXPECT_EQ(*p.total("start"), 0.3e-3f);
    EXPECT_ERROR(p.total("end"), ENODATA);
    EXPECT_ERROR(p.total("foo"), ENOENT);
    EXPECT_EQ(p.count("start"), 1);
    EXPECT_EQ(p.count("end"), 1);
    EXPECT_EQ(p.count("bar"), 0);
*/
}

// TODO: average min and max over a loop 
// TODO: calibrate() measures the time over nothing to subtract from the rest (default 0.0)
}