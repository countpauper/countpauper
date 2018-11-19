#include <boost/test/unit_test.hpp>
#include <math.h>
#include "from_string.h"
#include "Utils.h"

BOOST_AUTO_TEST_SUITE(String);

BOOST_AUTO_TEST_CASE(Integer)
{
    BOOST_CHECK_EQUAL(Engine::from_string<int>(L"0"), 0);
    BOOST_CHECK_EQUAL(Engine::from_string<int>(L" 1"), 1);
    BOOST_CHECK_EQUAL(Engine::from_string<int>(L"-2"), -2);
    BOOST_CHECK_THROW(Engine::from_string<int>(L""), std::exception);
    BOOST_CHECK_THROW(Engine::from_string<int>(L"cheese"), std::exception);
    BOOST_CHECK_THROW(Engine::from_string<int>(L"12345678901234567890"), std::exception);
}

BOOST_AUTO_TEST_CASE(Unsigned)
{
    BOOST_CHECK_EQUAL(Engine::from_string<unsigned>(L"0"), 0);
    BOOST_CHECK_EQUAL(Engine::from_string<unsigned>(L" 1"), 1);
//    ends up as 4bils, TODO: strip, check for minus in template specialization? BOOST_CHECK_THROW(Engine::from_string<unsigned>(L"-2"), std::runtime_error);
    BOOST_CHECK_THROW(Engine::from_string<unsigned>(L""), std::exception);
    BOOST_CHECK_THROW(Engine::from_string<unsigned>(L"cheese"), std::exception);
}

BOOST_AUTO_TEST_CASE(Float)
{
    BOOST_CHECK_EQUAL(Engine::from_string<float>(L"0"), 0.0f);
    BOOST_CHECK_EQUAL(Engine::from_string<float>(L" 1"), 1.0f);
    BOOST_CHECK_EQUAL(Engine::from_string<float>(L"-2"), -2.0f);
    BOOST_CHECK_EQUAL(Engine::from_string<float>(L"3.14"), 3.14f);
    BOOST_CHECK_EQUAL(Engine::from_string<float>(L"1E+2"), 100.0f);
    BOOST_CHECK_THROW(Engine::from_string<float>(L""), std::exception);
    BOOST_CHECK_THROW(Engine::from_string<float>(L"cheese"), std::exception);
}

BOOST_AUTO_TEST_CASE(Boolean)
{
    BOOST_CHECK_EQUAL(Engine::from_string<bool>(L"0"), false);
    BOOST_CHECK_EQUAL(Engine::from_string<bool>(L" 1"), true);
    /* // TODO boolean template specialization
    BOOST_CHECK_EQUAL(Engine::from_string<bool>(L"true"), true);
    BOOST_CHECK_EQUAL(Engine::from_string<bool>(L"false"), false);
    */
    BOOST_CHECK_THROW(Engine::from_string<bool>(L""), std::exception);
    BOOST_CHECK_THROW(Engine::from_string<bool>(L"cheese"), std::exception);
}

BOOST_AUTO_TEST_CASE(String)
{
    BOOST_CHECK(Engine::from_string<std::wstring>(L"Bla") == std::wstring(L"Bla"));
    BOOST_CHECK(Engine::from_string<std::string>(L"Bla") == std::string("Bla"));
}

BOOST_AUTO_TEST_CASE(Enum)
{
    enum class TestEnum { First, Second };
    std::map<std::wstring, TestEnum> map({
        { L"1st", TestEnum::First },
        { L"2nd", TestEnum::Second }
    });
    BOOST_CHECK(Engine::from_string<TestEnum>(L"1st", map) == TestEnum::First);
    BOOST_CHECK(Engine::from_string<TestEnum>(L"2nd", map) == TestEnum::Second);
    BOOST_CHECK_THROW(Engine::from_string<TestEnum>(L"", map), std::exception);
    BOOST_CHECK_THROW(Engine::from_string<TestEnum>(L"cheese", map), std::exception);
}

BOOST_AUTO_TEST_CASE(Split)
{
    BOOST_CHECK(Engine::Split("1,2", ',') == std::vector<std::string>({ "1", "2" }));
    BOOST_CHECK(Engine::Split(L"A B C", L' ') == std::vector<std::wstring>({ L"A", L"B", L"C" }));
    BOOST_CHECK(Engine::SplitSet("c|a|c|b", '|') == std::set<std::string>({ "a", "b", "c" }));
}

BOOST_AUTO_TEST_SUITE_END()