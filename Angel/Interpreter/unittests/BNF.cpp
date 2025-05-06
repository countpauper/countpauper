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
        RangeEq({Symbol("syntax"), 
                    Symbol("rule"),  Symbol("rule-name"), 
                        Symbol("expression"), 
                            Symbol("list"), 
                                Symbol("term"), Symbol("rule-name"), 
                            Symbol("list"), Symbol("list-end"), 
                        Symbol("expression-tail"), Symbol("expression-end"), 
                Symbol("syntax-tail"), Symbol("syntax-end")
        }));
}

TEST(BNF, Rules)
{
    RecursiveDescentParser parser(BNF);
    EXPECT_THAT(parser.ParseIt("<a> ::= <b> \r\n\n<b> ::= \"l\"\n"),
        RangeEq({Symbol("syntax"), 
                    Symbol("rule"), Symbol("rule-name"),
                    Symbol("expression"), 
                        Symbol("list"), 
                            Symbol("term"),Symbol("rule-name"),
                        Symbol("list"), Symbol("list-end"), 
                    Symbol("expression-tail"), Symbol("expression-end"), 
                Symbol("syntax-tail"),Symbol("line-end"),
                    Symbol("rule"),Symbol("rule-name"),
                    Symbol("expression"),
                        Symbol("list"),
                            Symbol("term"),Symbol("literal"),Symbol("string"), 
                        Symbol("list"), Symbol("list-end"), 
                    Symbol("expression-tail"), Symbol("expression-end"), 
                Symbol("syntax-tail"), Symbol("syntax-end")
        }));        
}

TEST(BNF, Expression)
{
    RecursiveDescentParser parser(BNF);
    EXPECT_THAT(parser.ParseIt("<a>::=\"l\" | <b>\n"), 
        RangeEq({Symbol("syntax"),
                    Symbol("rule"),Symbol("rule-name"),
                    Symbol("expression"),
                        Symbol("list"),
                            Symbol("term"),Symbol("literal"),Symbol("string"),
                        Symbol("list"), Symbol("list-end"), 
                    Symbol("expression-tail"),
                        Symbol("list"),
                            Symbol("term"), Symbol("rule-name"),
                        Symbol("list"), Symbol("list-end"), 
                    Symbol("expression-tail"), Symbol("expression-end"), 
                Symbol("syntax-tail"), Symbol("syntax-end")
            }));
    EXPECT_THROW(parser.ParseIt("<a>:==<b>|\n"), Error);
}     


TEST(BNF, List)
{
    RecursiveDescentParser parser(BNF);
    EXPECT_THAT(parser.ParseIt("<a>::=\"l\" <b>"), 
        RangeEq({Symbol("syntax"),
                    Symbol("rule"), Symbol("rule-name"),
                    Symbol("expression"),
                        Symbol("list"),
                            Symbol("term"),Symbol("literal"),Symbol("string"),
                        Symbol("list"),
                            Symbol("term"),Symbol("rule-name"),
                        Symbol("list"), Symbol("list-end"), 
                    Symbol("expression-tail"), Symbol("expression-end"), 
                Symbol("syntax-tail"), Symbol("syntax-end")
        }));
}     

TEST(BNF, Unicode)
{
    RecursiveDescentParser parser(BNF);
    EXPECT_THAT(parser.ParseIt("<日本語>::= \"(ﾉ◕ヮ◕)ﾉ*:･ﾟ✧\" '😄|😁'\n"),  
        RangeEq({Symbol("syntax"),
                    Symbol("rule"), Symbol("rule-name"),
                    Symbol("expression"),
                        Symbol("list"),
                            Symbol("term"),Symbol("literal"), Symbol("string"),
                        Symbol("list"), 
                            Symbol("term"),Symbol("literal"),Symbol("regex"),
                        Symbol("list"), Symbol("list-end"), 
                    Symbol("expression-tail"), Symbol("expression-end"), 
                Symbol("syntax-tail"), Symbol("syntax-end")
        }));
}

TEST(BNF, Epsilon)
{
    RecursiveDescentParser parser(BNF);
    EXPECT_THAT(parser.ParseIt("<epsilon>::= \"\""),  
        RangeEq({Symbol("syntax"),
                    Symbol("rule"), Symbol("rule-name"),
                    Symbol("expression"),
                        Symbol("list"),
                            Symbol("term"),Symbol("literal"), Symbol("epsilon"),
                        Symbol("list"), Symbol("list-end"), 
                    Symbol("expression-tail"), Symbol("expression-end"), 
                Symbol("syntax-tail"), Symbol("syntax-end")
        }));
}

}
