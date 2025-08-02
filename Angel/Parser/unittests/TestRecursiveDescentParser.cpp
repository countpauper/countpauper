
#include "Parser/RecursiveDescentParser.h"
#include "Parser/Syntax.h"
#include "Parser/TokenStream.h"
#include "Parser/Lexer.h"
#include "Parser/Error.h"
#include "Parser/Source.h"
#include "RangerMatcher.h"
#include <sstream>
#include <gtest/gtest.h>

namespace Parser::Test
{

TEST(RecursiveDescenterParser, ParseEmpty)
{    
    Syntax syntax{
        {"S", {Literal("s")}},
        {"S", {Epsilon()}}
    };
    RecursiveDescentParser parser(syntax);
    EXPECT_THAT(parser.ParseIt(""), RangeEq({Symbol("S")}));
}

TEST(RecursiveDescenterParser, ParseNotEmpty)
{    
    Syntax syntax{
        {"S", {Literal("s")}}
    };
    RecursiveDescentParser parser(syntax);
    EXPECT_THROW(parser.ParseIt(""), Error);
}

TEST(RecursiveDescenterParser, ParseLiteral)
{    
    Syntax syntax{
        {"S", {Literal("s")}}
    };
    RecursiveDescentParser parser(syntax);
    EXPECT_THAT(parser.ParseIt("s"), RangeEq({Symbol("S")}));
}

TEST(RecursiveDescenterParser, ParseRegex)
{    
    Syntax syntax{
        {"S", {Regex("[0-9]+")}}
    };
    RecursiveDescentParser parser(syntax);
    EXPECT_THAT(parser.ParseIt("123"), RangeEq({Symbol("S")}));
}
    
TEST(RecursiveDescenterParser, ParseSymbol)
{    
    Syntax syntax{
        {"S", {Literal("b"), Symbol("F")}},
        {"F", {Literal("o"), Symbol("F")}},
        {"F", {Epsilon()}}
    };
    RecursiveDescentParser parser(syntax);
    EXPECT_THAT(parser.ParseIt("bo"), RangeEq({Symbol("S"),Symbol("F"),Symbol("F")}));
}

TEST(RecursiveDescenterParser, ReferenceNestedSymbol)
{    
    Syntax syntax{
        {"S", {Literal("$"), Symbol("F")}},
        {"F", {Literal("o"), Symbol("F")}},
        {"F", {Epsilon()}}
    };
    RecursiveDescentParser parser(syntax);
    SymbolStream os;
    Source src("$o");
    parser.Parse(src, os);
    ParsedSymbol symbol;
    os >> symbol;
    EXPECT_EQ(symbol.symbol, "S");
    EXPECT_EQ(symbol.location, (SourceSpan{0, 2, &src}));
}

TEST(RecursiveDescenterParser, ParseRegexRule)
{    
    Syntax syntax{
        {"S", {Literal("b"), Symbol("F")}},
        {"F", {Regex("[0-9]"), Symbol("F")}},
        {"F", {Regex("x+")}}
    };
    RecursiveDescentParser parser(syntax);
    EXPECT_THAT(parser.ParseIt("b0x"), RangeEq({Symbol("S"),Symbol("F"),Symbol("F")}));
}

TEST(RecursiveDescenterParser, ParseMiddleRecursion)
{    
    Syntax syntax{
        {"S", {Literal("b"), Symbol("F"), Literal("b")}},
        {"F", {Literal("o"), Symbol("F")}},
        {"F", {Epsilon()}}
    };
    RecursiveDescentParser parser(syntax);
    EXPECT_THAT(parser.ParseIt("bob"), RangeEq({Symbol("S"),Symbol("F"), Symbol("F")}));
}

TEST(RecursiveDescenterParser, ParseEpsilon)
{    
    Syntax syntax{
        {"S", {Literal("b"), Symbol("F")}},
        {"F", {Literal("o"), Symbol("F")}},
        {"F", {Epsilon(Symbol::epsilon)}}
    };
    RecursiveDescentParser parser(syntax);
    EXPECT_THAT(parser.ParseIt("bo"), RangeEq({Symbol("S"),Symbol("F"),Symbol("F"), Symbol::epsilon}));
}


TEST(RecursiveDescenterParser, NonDeterministicFails)
{
    Syntax syntax{
        {"S", {Symbol("F"), Literal("h")}},
        {"F", {Literal("a")}},
        {"F", {Literal("a"), Symbol("F")}}
    };
    RecursiveDescentParser parser(syntax);
    EXPECT_THROW(parser.ParseIt("aah"), Error);
}

TEST(RecursiveDescenterParser, Fail)
{    
    Syntax syntax{
        {"S", {Literal("b"), Symbol("F")}},
        {"F", {Literal("o"), Symbol("F")}},
        {"F", {Epsilon()}}
    };
    RecursiveDescentParser parser(syntax);
    Source source("bot");
    try 
    {
        parser.ParseIt(source);
    } 
    catch(const Error& e)
    {
        EXPECT_EQ(e.Location(), SourceSpan(2,1, &source));
    }
}

TEST(RecursiveDescenterParser, InMediasRes)
{ 
    Syntax syntax{
        {"S", {Literal("b"), Symbol("F")}},
        {"F", {Literal("o"), Symbol("F")}},
        {"F", {Epsilon()}}
    };
    RecursiveDescentParser parser(syntax);
    EXPECT_THAT(parser.ParseIt("oo", "F"), RangeEq({Symbol("F"),Symbol("F"), Symbol("F")}));
}

TEST(RecursiveDescenterParser, InfiniteRecursionIsPrevented)
{ 
    Syntax syntax{
        {"S", {Symbol("F"), Literal("end")}},
        {"F", {Epsilon(), Symbol("S"), Literal("oops")}}
    };
    EXPECT_THROW(RecursiveDescentParser parser(syntax), std::runtime_error);

}


}
