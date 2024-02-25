#include <gtest/gtest.h>
#include "UI/Debug.h"
#include <filesystem>

namespace Engine::Debug::Test
{

    TEST(Logging, TimerLogs)
    {
        Timer timer("test");
        // Test output manually
    }

    TEST(Logging, LogNoParameters)
    {
        Log<true>("logging shown");
        Log<false>("not shown");
    }

    TEST(Logging, LogWithParameters)
    {
        Log<true>("logging %s", "shown");
        Log<false>("logging %s shown", "not");
    }



}
