#include "pch.h"
#include "Parser/StringEncoding.h"

#pragma warning(disable:4566)	// google test can't represent unicode

namespace Angel::Parser::Test
{
    TEST(TestEncoding, Empty)
    {
        EXPECT_EQ(to_utf8(L""), "");
        EXPECT_EQ(from_utf8(""), L"");
    }

    TEST(TestEncoding, ASCII)
    {
        EXPECT_EQ(to_utf8(L"Test 123"), "Test 123");
        EXPECT_EQ(from_utf8("Test 123"), L"Test 123");
    }

    TEST(TestEncoding, Greek)
    {
        std::string greek("αβγΔΩ");
        EXPECT_EQ(to_utf8(L"\u03b1\u03b2\u03b3\u0394\u03a9"), "αβγΔΩ");
        EXPECT_EQ(from_utf8("αβγΔΩ"), L"\u03b1\u03b2\u03b3\u0394\u03a9");
    }

}
