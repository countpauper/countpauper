#include "Parser/BNF.h"
#include "Parser/RecursiveDescentParser.h"
#include "Parser/Error.h"
#include "Parser/Source.h"
#include "RangerMatcher.h"
#include <gtest/gtest.h>

namespace Parser::Test
{

TEST(BNF, Rule)
{
    RecursiveDescentParser parser(BNF);
    EXPECT_THAT(parser.ParseIt("<a> ::= <b>"), 
        RangeEq({Symbol("syntax"), 
                    Symbol("line"), Symbol("rule"),  Symbol("rule-name"), 
                        Symbol("expression"), 
                            Symbol("list"), 
                                Symbol("term"), Symbol("rule-name"), 
                            Symbol("list"), Symbol("list-end"), 
                        Symbol("expression-tail"), Symbol("expression-end"), 
                    Symbol("remark"),
                Symbol("syntax-tail"), Symbol("syntax-end")
        }));
}

TEST(BNF, TrailingRemark)
{
    RecursiveDescentParser parser(BNF);
    EXPECT_THAT(parser.ParseIt("<a>::=<b> # this is remarkable"), 
        RangeEq({Symbol("syntax"), 
                    Symbol("line"), Symbol("rule"),  Symbol("rule-name"), 
                        Symbol("expression"), 
                            Symbol("list"), 
                                Symbol("term"), Symbol("rule-name"), 
                            Symbol("list"), Symbol("list-end"), 
                        Symbol("expression-tail"), Symbol("expression-end"), 
                    Symbol("remark"),
                Symbol("syntax-tail"),Symbol("syntax-end")
        }));
}

TEST(BNF, LeadingRemark)
{
    RecursiveDescentParser parser(BNF);
    EXPECT_THAT(parser.ParseIt("  # this rules\n <a>::=<b>"), 
        RangeEq({Symbol("syntax"), 
                    Symbol("line"), Symbol("remark"),
                Symbol("syntax-tail"),
                    Symbol("line"), Symbol("rule"),  Symbol("rule-name"), 
                        Symbol("expression"), 
                            Symbol("list"), 
                                Symbol("term"), Symbol("rule-name"), 
                            Symbol("list"), Symbol("list-end"), 
                        Symbol("expression-tail"), Symbol("expression-end"), 
                    Symbol("remark"),
                Symbol("syntax-tail"),Symbol("syntax-end")
        }));
}

TEST(BNF, Rules)
{
    RecursiveDescentParser parser(BNF);
    EXPECT_THAT(parser.ParseIt("<a> ::= <b> \r\n\n<b> ::= \"l\"\n"),
        RangeEq({Symbol("syntax"), 
                    Symbol("line"), Symbol("rule"), Symbol("rule-name"),
                    Symbol("expression"), 
                        Symbol("list"), 
                            Symbol("term"),Symbol("rule-name"),
                        Symbol("list"), Symbol("list-end"), 
                    Symbol("expression-tail"), Symbol("expression-end"), 
                    Symbol("remark"),
                Symbol("syntax-tail"),
                    Symbol("line"), Symbol("rule"),Symbol("rule-name"),
                    Symbol("expression"),
                        Symbol("list"),
                            Symbol("term"),Symbol("token"),Symbol("literal"), 
                        Symbol("list"), Symbol("list-end"), 
                    Symbol("expression-tail"), Symbol("expression-end"), 
                    Symbol("remark"),
                Symbol("syntax-tail"), Symbol("line"), Symbol("remark"), 
                Symbol("syntax-tail"), Symbol("syntax-end")
        }));        
}

TEST(BNF, Expression)
{
    RecursiveDescentParser parser(BNF);
    EXPECT_THAT(parser.ParseIt("<a>::=\"l\" | <b>\n"), 
        RangeEq({Symbol("syntax"),
                    Symbol("line"), Symbol("rule"),Symbol("rule-name"),
                    Symbol("expression"),
                        Symbol("list"),
                            Symbol("term"),Symbol("token"),Symbol("literal"),
                        Symbol("list"), Symbol("list-end"), 
                    Symbol("expression-tail"),
                        Symbol("list"),
                            Symbol("term"), Symbol("rule-name"),
                        Symbol("list"), Symbol("list-end"), 
                    Symbol("expression-tail"), Symbol("expression-end"), 
                    Symbol("remark"),
                Symbol("syntax-tail"), Symbol("line"), Symbol("remark"), 
                Symbol("syntax-tail"), Symbol("syntax-end")
            }));
    EXPECT_THROW(parser.ParseIt("<a>:==<b>|\n"), Error);
}     


TEST(BNF, List)
{
    RecursiveDescentParser parser(BNF);
    EXPECT_THAT(parser.ParseIt("<a>::=\"l\" <b>"), 
        RangeEq({Symbol("syntax"),
                    Symbol("line"), Symbol("rule"), Symbol("rule-name"),
                    Symbol("expression"),
                        Symbol("list"),
                            Symbol("term"),Symbol("token"),Symbol("literal"),
                        Symbol("list"),
                            Symbol("term"),Symbol("rule-name"),
                        Symbol("list"), Symbol("list-end"), 
                    Symbol("expression-tail"), Symbol("expression-end"), 
                    Symbol("remark"),
                Symbol("syntax-tail"), Symbol("syntax-end")
        }));
}     

TEST(BNF, Unicode)
{
    RecursiveDescentParser parser(BNF);
    EXPECT_THAT(parser.ParseIt("<日本語>::= \"(ﾉ◕ヮ◕)ﾉ*:･ﾟ✧\" '≜|ည' # ข้อคิดเห็น\n"),  
        RangeEq({Symbol("syntax"),
                    Symbol("line"), Symbol("rule"), Symbol("rule-name"),
                    Symbol("expression"),
                        Symbol("list"),
                            Symbol("term"),Symbol("token"), Symbol("literal"),
                        Symbol("list"), 
                            Symbol("term"),Symbol("token"),Symbol("regex"),
                        Symbol("list"), Symbol("list-end"), 
                    Symbol("expression-tail"), Symbol("expression-end"), 
                    Symbol("remark"), 
                Symbol("syntax-tail"), 
                    Symbol("line"), Symbol("remark"), Symbol("syntax-tail"),
                Symbol("syntax-end")
        }));
}

TEST(BNF, Epsilon)
{
    RecursiveDescentParser parser(BNF);
    EXPECT_THAT(parser.ParseIt("<epsilon>::= \"\""),  
        RangeEq({Symbol("syntax"),
                    Symbol("line"), Symbol("rule"), Symbol("rule-name"),
                    Symbol("expression"),
                        Symbol("list"),
                            Symbol("term"),Symbol("token"), Symbol("epsilon"),
                        Symbol("list"), Symbol("list-end"), 
                    Symbol("expression-tail"), Symbol("expression-end"), 
                    Symbol("remark"),
                Symbol("syntax-tail"), Symbol("syntax-end")
        }));
}

}
