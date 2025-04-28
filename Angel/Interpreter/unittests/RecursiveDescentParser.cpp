
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
        {"S", {Literal("b"), Symbol("F")}},
        {"F", {Literal("o"), Symbol("F")}},
        {"F", {Literal("o")}}
    };
    RecursiveDescentParser parser(syntax);
    EXPECT_EQ(parser.ParseIt("bo"), std::vector<std::string>({"S", "F"}));
}

TEST(RecursiveDescenterParser, ParseRegex)
{    
    Syntax syntax{
        {"S", {Literal("b"), Symbol("F")}},
        {"F", {Regex("[0-9]"), Symbol("F")}},
        {"F", {Regex("x+")}}
    };
    RecursiveDescentParser parser(syntax);
    EXPECT_EQ(parser.ParseIt("b0x"), std::vector<std::string>({"S", "F", "F"}));
}

TEST(RecursiveDescenterParser, ParseMiddleRecursion)
{    
    Syntax syntax{
        {"S", {Literal("b"), Symbol("F"), Literal("b")}},
        {"F", {Literal("o"), Symbol("F")}},
        {"F", {Literal("o")}}
    };
    RecursiveDescentParser parser(syntax);
    EXPECT_EQ(parser.ParseIt("bob"), std::vector<std::string>({"S", "F"}));
}

TEST(RecursiveDescenterParser, ParseEpsilon)
{    
    Syntax syntax{
        {"S", {Literal("b"), Symbol("F")}},
        {"F", {Literal("o"), Symbol("F")}},
        {"F", {epsilon}}
    };
    RecursiveDescentParser parser(syntax);
    EXPECT_EQ(parser.ParseIt("bo"), std::vector<std::string>({"S", "F", "F"}));
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

// TODO test source reference 
// TODO test with regex


}
