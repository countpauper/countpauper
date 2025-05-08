
#include "Interpreter/RecursiveDescentParser.h"
#include "Interpreter/Syntax.h"
#include "Interpreter/TokenStream.h"
#include "Interpreter/Lexer.h"
#include "Interpreter/Error.h"
#include "RangerMatcher.h"
#include <sstream>
#include <gtest/gtest.h>

namespace Interpreter::Test
{

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
        {"F", {Literal("o")}}
    };
    RecursiveDescentParser parser(syntax);
    EXPECT_THAT(parser.ParseIt("bob"), RangeEq({Symbol("S"),Symbol("F")}));
}

TEST(RecursiveDescenterParser, ParseEpsilon)
{    
    Syntax syntax{
        {"S", {Literal("b"), Symbol("F")}},
        {"F", {Literal("o"), Symbol("F")}},
        {"F", {Epsilon()}}
    };
    RecursiveDescentParser parser(syntax);
    EXPECT_THAT(parser.ParseIt("bo"), RangeEq({Symbol("S"),Symbol("F"),Symbol("F")}));
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
    try 
    {
        parser.ParseIt("bot");
    } 
    catch(const Error& e)
    {
        EXPECT_EQ(e.Location(), SourceSpan({2,1}));
    }
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
