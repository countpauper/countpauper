#include "Interpreter/BNF.h"
#include "Interpreter/RecursiveDescentParser.h"
#include "Interpreter/Error.h"
#include "RangerMatcher.h"
#include <gtest/gtest.h>

namespace Interpreter::Test
{

TEST(BNF, Rule)
{
    RecursiveDescentParser parser(BNF);
    EXPECT_THAT(parser.ParseIt("<a> ::= <b>"), 
        RangeEq({Symbol("syntax"), Symbol("rule"), Symbol("rule-name"),Symbol("opt-whitespace"),Symbol("opt-whitespace"),Symbol("expression"), Symbol("list"), Symbol("term"), Symbol("rule-name")}));
}

TEST(BNF, Rules)
{
    RecursiveDescentParser parser(BNF);
    EXPECT_THAT(parser.ParseIt("<a> ::= <b> \r\n\n<b> ::= \"l\"\n"),
        RangeEq({Symbol("syntax"), Symbol("rule"), Symbol("rule-name"),Symbol("opt-whitespace"),Symbol("opt-whitespace"),Symbol("expression"), Symbol("list"), Symbol("term"),Symbol("rule-name"),
                Symbol("syntax-tail"),Symbol("line-end"),Symbol("rule"),Symbol("rule-name"),Symbol("opt-whitespace"),Symbol("opt-whitespace"),Symbol("expression"),Symbol("list"),Symbol("term"),Symbol("literal"),Symbol("string")}));        
}

TEST(BNF, Expression)
{
    RecursiveDescentParser parser(BNF);
    EXPECT_THAT(parser.ParseIt("<a>::=\"l\" | <b>\n"), 
        RangeEq({Symbol("syntax"),Symbol("rule"),Symbol("rule-name"),Symbol("expression"),Symbol("list"),Symbol("term"),Symbol("literal"),Symbol("string"),Symbol("opt-whitespace"),
                 Symbol("expression-tail"),Symbol("opt-whitespace"),Symbol("list"),Symbol("term"), Symbol("rule-name")}));
    EXPECT_THROW(parser.ParseIt("<a>:==<b>|\n"), Error);
}     

TEST(BNF, List)
{
    RecursiveDescentParser parser(BNF);
    EXPECT_THAT(parser.ParseIt("<a>::=\"l\" <b>"), 
        RangeEq({Symbol("syntax"),Symbol("rule"),Symbol("rule-name"),Symbol("expression"),Symbol("list"),Symbol("term"),Symbol("literal"),Symbol("string"),Symbol("opt-whitespace"),Symbol("list"),Symbol("term"),Symbol("rule-name")}));
}       

TEST(BNF, Unicode)
{
    RecursiveDescentParser parser(BNF);
    EXPECT_THAT(parser.ParseIt("<日本語>::= \"(ﾉ◕ヮ◕)ﾉ*:･ﾟ✧\" '😄|😁'\n"),  
        RangeEq({Symbol("syntax"),Symbol("rule"),Symbol("rule-name"),Symbol("opt-whitespace"),Symbol("expression"),Symbol("list"),Symbol("term"),Symbol("literal"),Symbol("string"),Symbol("opt-whitespace"),Symbol("list"),Symbol("term"),Symbol("literal"),Symbol("regex")}));
}

}
