#include <gtest/gtest.h>
#include <math.h>
#include "Engine/Utility/from_string.h"
#include "Engine/Utility/Utils.h"

namespace Engine::Test
{

TEST(String, Integer)
{
    EXPECT_EQ(Engine::from_string<int>("0"), 0);
    EXPECT_EQ(Engine::from_string<int>(" 1"), 1);
    EXPECT_EQ(Engine::from_string<int>("-2"), -2);
    EXPECT_THROW(Engine::from_string<int>(""), std::exception);
    EXPECT_THROW(Engine::from_string<int>("cheese"), std::exception);
    EXPECT_THROW(Engine::from_string<int>("12345678901234567890"), std::exception);
}


TEST(String, Unsigned)
{
    EXPECT_EQ(Engine::from_string<unsigned>("0"), 0);
    EXPECT_EQ(Engine::from_string<unsigned>(" 1"), 1);
    //    ends up as 4bils, TODO: strip, check for minus in template specialization? EXPECT_THROW(Engine::from_string<unsigned>("-2"), std::runtime_error);
    EXPECT_THROW(Engine::from_string<unsigned>(""), std::exception);
    EXPECT_THROW(Engine::from_string<unsigned>("cheese"), std::exception);
}

TEST(String, Float)
{
    EXPECT_EQ(Engine::from_string<float>("0"), 0.0f);
    EXPECT_EQ(Engine::from_string<float>(" 1"), 1.0f);
    EXPECT_EQ(Engine::from_string<float>("-2"), -2.0f);
    EXPECT_EQ(Engine::from_string<float>("3.14"), 3.14f);
    EXPECT_EQ(Engine::from_string<float>("1E+2"), 100.0f);
    EXPECT_THROW(Engine::from_string<float>(""), std::exception);
    EXPECT_THROW(Engine::from_string<float>("cheese"), std::exception);
}

TEST(String, Boolean)
{
    EXPECT_EQ(Engine::from_string<bool>("0"), false);
    EXPECT_EQ(Engine::from_string<bool>(" 1"), true);
    EXPECT_EQ(Engine::from_string<bool>("true"), true);
    EXPECT_EQ(Engine::from_string<bool>("false"), false);
    EXPECT_EQ(Engine::from_string<bool>("yes"), true);
    EXPECT_EQ(Engine::from_string<bool>("no"), false);
    EXPECT_THROW(Engine::from_string<bool>(""), std::exception);
    EXPECT_THROW(Engine::from_string<bool>("cheese"), std::exception);
}

TEST(String, String)
{
    EXPECT_TRUE(Engine::from_string<std::string>("Bla") == std::string("Bla"));
}

TEST(String, Enum)
{
    enum class TestEnum { First, Second };
    const std::map<std::string_view, TestEnum> map({
        { "1st", TestEnum::First },
        { "2nd", TestEnum::Second }
        });
    EXPECT_TRUE(Engine::from_string<TestEnum>("1st", map) == TestEnum::First);
    EXPECT_TRUE(Engine::from_string<TestEnum>("2nd", map) == TestEnum::Second);
    EXPECT_THROW(Engine::from_string<TestEnum>("", map), std::exception);
    EXPECT_THROW(Engine::from_string<TestEnum>("cheese", map), std::exception);
}

TEST(String, Split)
{
    EXPECT_EQ(Engine::Split("1,2", ','), std::vector<std::string_view>({ "1", "2" }));
    EXPECT_EQ(Engine::Split("A B C", ' '), std::vector<std::string_view>({ "A", "B", "C" }));
    EXPECT_EQ(Engine::SplitSet("c|a|c|b", '|'), std::set<std::string_view>({ "a", "b", "c" }));
}

TEST(String, Strip)
{
    EXPECT_EQ(Engine::Strip("123", " \t\r\n"), "123");
    EXPECT_EQ(Engine::Strip("-123-", "-") , "123");
    EXPECT_EQ(Engine::Strip("\t 123\r\n", " \t\r\n"), "123");
    EXPECT_EQ(Engine::Strip("\t1 2\t3\r\n", " \t\r\n"), "1 2\t3");
    EXPECT_TRUE(Engine::Strip("123", "321").empty());
}


TEST(String, UpperCase)
{
    EXPECT_EQ(Engine::UpperCase("Upp3rC4se"), "UPP3RC4SE");
    EXPECT_EQ(Engine::UpperCase("o0o"), "O0O");
}

TEST(String, LowerCase)
{
    EXPECT_EQ(Engine::LowerCase("L0WeRC4$E"), "l0werc4$e");
}

TEST(String, Duration)
{
    EXPECT_EQ(FormatDuration(0), "0:00:00.000");
    EXPECT_EQ(FormatDuration(0.123), "0:00:00.123");
    EXPECT_EQ(FormatDuration(3666.999), "1:01:06.999");
    EXPECT_EQ(FormatDuration(-1), "-0:00:01.000");
}

}
