#include <gtest/gtest.h>
#include "Engine/Utility/Timer.h"
#include <thread>

namespace Engine::Test
{
    TEST(Timer, Positive)
    {
        Timer timer;
        EXPECT_GE(timer.Seconds(), 0);
    }

    TEST(Timer, Precision)
    {
        Timer timer;
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
        EXPECT_NEAR(timer.Seconds(), 0.01, 0.01);  // timer is more precise, but sleep is not
    }

}
