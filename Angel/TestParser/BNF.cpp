#include "pch.h"
#include "Parser/BNF.h"

namespace Angel::Parser::BNF::Test
{

    TEST(TestBNF, Literal)
    {
        Literal l("Test");
        EXPECT_NO_THROW(Parse(Rule(l), "Test"));
        EXPECT_THROW(Parse(Rule(l), "Fail"), SyntaxError);
        EXPECT_THROW(Parse(Rule(l), "Test "), SyntaxError);
    }

}