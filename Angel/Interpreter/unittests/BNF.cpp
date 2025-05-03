#include "Interpreter/BNF.h"
#include "Interpreter/RecursiveDescentParser.h"
#include <gtest/gtest.h>

namespace Interpreter::Test
{

TEST(BNF, ParseRule)
{
    RecursiveDescentParser parser(BNF);
    EXPECT_EQ(parser.ParseIt("<a> ::= <b>\n"), "<syntax> <rule> <rule-name> <opt-whitespace> <opt-whitespace> <expression> <list> <term> <rule-name> <line-end>");
}

TEST(BNF, Unicode)
{
    // 日本語
}

}
