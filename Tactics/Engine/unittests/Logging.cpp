#include <gtest/gtest.h>
#include "UI/Logging.h"
#include "UI/Debug.h"

namespace Engine::Logging::Test
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

    TEST(LogLevel, LogNoParameters)
    {
        constexpr Level testCategory = Info;
        Log<testCategory, Info>("info  shown");
        Log<testCategory, Debug>("Debug not shown");
    }

}
