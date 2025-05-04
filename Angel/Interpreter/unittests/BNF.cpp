#include "Interpreter/BNF.h"
#include "Interpreter/RecursiveDescentParser.h"
#include "Interpreter/Error.h"
#include <gtest/gtest.h>

namespace Interpreter::Test
{

TEST(BNF, Rule)
{
    RecursiveDescentParser parser(BNF);
    EXPECT_EQ(parser.ParseIt("<a> ::= <b>"), "<syntax> <rule> <rule-name> <opt-whitespace> <opt-whitespace> <expression> <list> <term> <rule-name>");
}

TEST(BNF, Rules)
{
    RecursiveDescentParser parser(BNF);
    EXPECT_EQ(parser.ParseIt("<a> ::= <b> \r\n\n<b> ::= \"l\"\n"), 
        "<syntax> <rule> <rule-name> <opt-whitespace> <opt-whitespace> <expression> <list> <term> <rule-name> "
        "<syntax-tail> <line-end> <rule> <rule-name> <opt-whitespace> <opt-whitespace> <expression> <list> <term> <literal> <string>");        
}

TEST(BNF, Expression)
{
    RecursiveDescentParser parser(BNF);
    EXPECT_EQ(parser.ParseIt("<a>::=\"l\" | <b>\n"), 
        "<syntax> <rule> <rule-name> <expression> <list> <term> <literal> <string> <opt-whitespace> "
        "<expression-tail> <opt-whitespace> <list> <term> <rule-name>"
    );
    EXPECT_THROW(parser.ParseIt("<a>:==<b>|\n"), Error);
}     

TEST(BNF, List)
{
    RecursiveDescentParser parser(BNF);
    EXPECT_EQ(parser.ParseIt("<a>::=\"l\" <b>"), 
        "<syntax> <rule> <rule-name> <expression> <list> <term> <literal> <string> <opt-whitespace> <list> <term> <rule-name>"
    );
}       

TEST(BNF, Unicode)
{
    RecursiveDescentParser parser(BNF);
    EXPECT_EQ(parser.ParseIt("<日本語>::= \"(ﾉ◕ヮ◕)ﾉ*:･ﾟ✧\" '😄|😁'\n"),  //  
        "<syntax> <rule> <rule-name> <opt-whitespace> <expression> <list> <term> <literal> <string> <opt-whitespace> <list> <term> <literal> <regex>");
}

}
