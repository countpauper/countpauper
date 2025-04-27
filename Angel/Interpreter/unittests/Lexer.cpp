#include "Interpreter/Lexer.h"
#include "Interpreter/Tokens.h"
#include "Interpreter/Error.h"
#include <gtest/gtest.h>

namespace Interpreter::Test
{

TEST(Lexer, Empty)
{
    Lexer lexer({});
    std::deque<InputToken> result{{nullptr, 0, 0}};
    EXPECT_EQ(lexer.Process(""), result);
    EXPECT_THROW(lexer.Process("cat"), Error);
}

TEST(Lexer, Literal)
{
    Literal cat("cat");
    Lexer lexer({&cat});
    EXPECT_EQ(lexer.Process("")[0], InputToken({nullptr, 0, 0}));
    EXPECT_EQ(lexer.Process("cat")[0], InputToken({&cat, 0, 3}));
    EXPECT_EQ(lexer.Process("cat")[1], InputToken({nullptr, 3, 0}));
    EXPECT_THROW(lexer.Process("dog"), Error);
}

}