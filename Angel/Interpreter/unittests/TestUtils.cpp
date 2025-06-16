#include "Interpreter/Utils.h"
#include <gtest/gtest.h>


namespace Interpreter::Test
{

TEST(Utils, RightTrim)
{
    EXPECT_EQ(rtrim("bla "), "bla");
    EXPECT_EQ(rtrim("bla \t\n"), "bla");
    EXPECT_EQ(rtrim("bla\tdi bla\n"), "bla\tdi bla");
    EXPECT_EQ(rtrim("bla...","."), "bla");
}


TEST(Utils, LeftTrim)
{
    EXPECT_EQ(ltrim("  bla "), "bla ");
    EXPECT_EQ(ltrim("\t\n\vbla"), "bla");
    EXPECT_EQ(ltrim("\tbla di\nbla  "), "bla di\nbla  ");
    EXPECT_EQ(ltrim(":bla",":"), "bla");
}

TEST(Utils, Unclose)
{
    EXPECT_EQ(Unclose("\"literal\"", '"'), "literal");
    EXPECT_EQ(Unclose("'regex'", '\''), "regex");
    EXPECT_EQ(Unclose("<symbol>", '<','>'), "symbol");
    EXPECT_THROW(Unclose("\"", '"'), std::invalid_argument);
    EXPECT_THROW(Unclose("\"open", '"'), std::invalid_argument);
    EXPECT_THROW(Unclose("open\"", '"'), std::invalid_argument);
    
}

}
