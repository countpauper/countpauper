
#include "Interpreter/RecursiveDescentParser.h"

#include "Interpreter/Syntax.h"
#include "Interpreter/TokenStream.h"
#include "Interpreter/Lexer.h"
#include "Interpreter/Error.h"
#include <sstream>
#include <gtest/gtest.h>

namespace Interpreter 
{

TEST(RecursiveDescenterParser, ParseLiteral)
{    
    Syntax syntax{
        {"S", {Literal("s")}}
    };
    RecursiveDescentParser parser(syntax);
    EXPECT_EQ(parser.ParseIt("s"), "<S>");
}

TEST(RecursiveDescenterParser, ParseRegex)
{    
    Syntax syntax{
        {"S", {Regex("[0-9]+")}}
    };
    RecursiveDescentParser parser(syntax);
    EXPECT_EQ(parser.ParseIt("123"), "<S>");
}
    
TEST(RecursiveDescenterParser, ParseSymbol)
{    
    Syntax syntax{
        {"S", {Literal("b"), Symbol("F")}},
        {"F", {Literal("o"), Symbol("F")}},
        {"F", {Literal("o")}}
    };
    RecursiveDescentParser parser(syntax);
    EXPECT_EQ(parser.ParseIt("bo"), "<S> <F>");
}

TEST(RecursiveDescenterParser, ParseRegexRule)
{    
    Syntax syntax{
        {"S", {Literal("b"), Symbol("F")}},
        {"F", {Regex("[0-9]"), Symbol("F")}},
        {"F", {Regex("x+")}}
    };
    RecursiveDescentParser parser(syntax);
    EXPECT_EQ(parser.ParseIt("b0x"), "<S> <F> <F>");
}

TEST(RecursiveDescenterParser, ParseMiddleRecursion)
{    
    Syntax syntax{
        {"S", {Literal("b"), Symbol("F"), Literal("b")}},
        {"F", {Literal("o"), Symbol("F")}},
        {"F", {Literal("o")}}
    };
    RecursiveDescentParser parser(syntax);
    EXPECT_EQ(parser.ParseIt("bob"), "<S> <F>");
}

TEST(RecursiveDescenterParser, ParseEpsilon)
{    
    Syntax syntax{
        {"S", {Literal("b"), Symbol("F")}},
        {"F", {Literal("o"), Symbol("F")}},
        {"F", {epsilon}}
    };
    RecursiveDescentParser parser(syntax);
    EXPECT_EQ(parser.ParseIt("bo"), "<S> <F>");
}


TEST(RecursiveDescenterParser, HeadTail)
{
    Syntax syntax{
        {"S", {Symbol("F"), Literal("h")}},
        {"F", {Literal("a")}},
        {"F", {Literal("a"), Symbol("F")}}
    };
    RecursiveDescentParser parser(syntax);
    EXPECT_EQ(parser.ParseIt("aah"), "<S> <F> <F>");
}

TEST(RecursiveDescenterParser, Fail)
{    
    Syntax syntax{
        {"S", {Literal("b"), Symbol("F")}},
        {"F", {Literal("o"), Symbol("F")}},
        {"F", {epsilon}}
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
        {"F", {epsilon, Symbol("S"), Literal("oops")}}
    };
    EXPECT_THROW(RecursiveDescentParser parser(syntax), std::runtime_error);

}


}
