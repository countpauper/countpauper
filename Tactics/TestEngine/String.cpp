#include "pch.h"
#include <math.h>
#include "from_string.h"
#include "Utils.h"

namespace Engine::Test
{

TEST(String, Integer)
{
    EXPECT_EQ(Engine::from_string<int>(L"0"), 0);
    EXPECT_EQ(Engine::from_string<int>(L" 1"), 1);
    EXPECT_EQ(Engine::from_string<int>(L"-2"), -2);
    EXPECT_THROW(Engine::from_string<int>(L""), std::exception);
    EXPECT_THROW(Engine::from_string<int>(L"cheese"), std::exception);
    EXPECT_THROW(Engine::from_string<int>(L"12345678901234567890"), std::exception);
}


TEST(String, Unsigned)
{
    EXPECT_EQ(Engine::from_string<unsigned>(L"0"), 0);
    EXPECT_EQ(Engine::from_string<unsigned>(L" 1"), 1);
    //    ends up as 4bils, TODO: strip, check for minus in template specialization? EXPECT_THROW(Engine::from_string<unsigned>(L"-2"), std::runtime_error);
    EXPECT_THROW(Engine::from_string<unsigned>(L""), std::exception);
    EXPECT_THROW(Engine::from_string<unsigned>(L"cheese"), std::exception);
}

TEST(String, Float)
{
    EXPECT_EQ(Engine::from_string<float>(L"0"), 0.0f);
    EXPECT_EQ(Engine::from_string<float>(L" 1"), 1.0f);
    EXPECT_EQ(Engine::from_string<float>(L"-2"), -2.0f);
    EXPECT_EQ(Engine::from_string<float>(L"3.14"), 3.14f);
    EXPECT_EQ(Engine::from_string<float>(L"1E+2"), 100.0f);
    EXPECT_THROW(Engine::from_string<float>(L""), std::exception);
    EXPECT_THROW(Engine::from_string<float>(L"cheese"), std::exception);
}

TEST(String, Boolean)
{
    EXPECT_EQ(Engine::from_string<bool>(L"0"), false);
    EXPECT_EQ(Engine::from_string<bool>(L" 1"), true);
    EXPECT_EQ(Engine::from_string<bool>(L"true"), true);
    EXPECT_EQ(Engine::from_string<bool>(L"false"), false);
    EXPECT_EQ(Engine::from_string<bool>(L"yes"), true);
    EXPECT_EQ(Engine::from_string<bool>(L"no"), false);
    EXPECT_THROW(Engine::from_string<bool>(L""), std::exception);
    EXPECT_THROW(Engine::from_string<bool>(L"cheese"), std::exception);
}

TEST(String, String)
{
    EXPECT_TRUE(Engine::from_string<std::wstring>(L"Bla") == std::wstring(L"Bla"));
    EXPECT_TRUE(Engine::from_string<std::string>(L"Bla") == std::string("Bla"));
}

TEST(String, Enum)
{
    enum class TestEnum { First, Second };
    std::map<const std::wstring_view, TestEnum> map({
        { L"1st", TestEnum::First },
        { L"2nd", TestEnum::Second }
        });
    EXPECT_TRUE(Engine::from_string<TestEnum>(L"1st", map) == TestEnum::First);
    EXPECT_TRUE(Engine::from_string<TestEnum>(L"2nd", map) == TestEnum::Second);
    EXPECT_THROW(Engine::from_string<TestEnum>(L"", map), std::exception);
    EXPECT_THROW(Engine::from_string<TestEnum>(L"cheese", map), std::exception);
}

TEST(String, Split)
{
    EXPECT_EQ(Engine::Split("1,2", ','), std::vector<std::string>({ "1", "2" }));
    EXPECT_EQ(Engine::Split(L"A B C", L' '), std::vector<std::wstring>({ L"A", L"B", L"C" }));
    EXPECT_EQ(Engine::SplitSet("c|a|c|b", '|'), std::set<std::string>({ "a", "b", "c" }));
}

TEST(String, Strip)
{
    EXPECT_EQ(Engine::Strip(L"123", L" \t\r\n"), std::wstring(L"123"));
    EXPECT_EQ(Engine::Strip(L"-123-", L"-") , std::wstring(L"123"));
    EXPECT_EQ(Engine::Strip(L"\t 123\r\n", L" \t\r\n"), std::wstring(L"123"));
    EXPECT_EQ(Engine::Strip(L"\t1 2\t3\r\n", L" \t\r\n"), std::wstring(L"1 2\t3"));
    EXPECT_TRUE(Engine::Strip(L"123", L"321").empty());
}


TEST(String, UpperCase)
{
    EXPECT_EQ(L"UPP3RC4SE", Engine::UpperCase(L"Upp3rC4se"));
    EXPECT_EQ(L"O0O", Engine::UpperCase(L"o0o"));
}


TEST(String, Duration)
{
    EXPECT_EQ(FormatDuration(0), "0:00:00.000");
    EXPECT_EQ(FormatDuration(0.123), "0:00:00.123");
    EXPECT_EQ(FormatDuration(3666.999), "1:01:06.999");
    EXPECT_EQ(FormatDuration(-1), "-0:00:01.000");
}

}
