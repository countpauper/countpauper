#include "Interpreter/Lexer.h"
#include "Interpreter/Terms.h"
#include "Interpreter/Error.h"
#include "Interpreter/Syntax.h"
#include <gtest/gtest.h>

namespace Interpreter::Test
{

TEST(Lexer, Empty)
{
    Lexer lexer(Lexicon{});
    std::deque<InputToken> result{InputToken()};
    EXPECT_EQ(lexer.Process(""), result);
    EXPECT_THROW(lexer.Process("cat"), Error);
}

TEST(Lexer, Literal)
{
    Term cat{Literal("cat")};
    Lexer lexer(Lexicon{&cat});
    EXPECT_EQ(lexer.Process("")[0], InputToken(InputToken()));
    EXPECT_EQ(lexer.Process("cat")[0], InputToken(cat, 0, 3));
    EXPECT_EQ(lexer.Process("cat")[1], InputToken(0, 3, 0));
    EXPECT_THROW(lexer.Process("dog"), Error);
}

TEST(Lexer, Regex)
{
    Term whitespace = Regex("\\s+");
    Lexer lexer(Lexicon{&whitespace});
    EXPECT_EQ(lexer.Process("")[0], InputToken());
    EXPECT_EQ(lexer.Process("\t ")[0], InputToken(whitespace, 0, 2));
    EXPECT_THROW(lexer.Process("not space"), Error);
}

TEST(Lexer, NotAmbiguous)
{
    Term greq = Literal(">=");
    Term gr = Literal(">");
    Term eq = Literal("=");
    Lexer lexer(Lexicon{ &gr, &eq, &greq });
    EXPECT_EQ(lexer.Process(">=")[0], InputToken(greq, 0, 2));
    EXPECT_EQ(lexer.Process("=>")[0], InputToken(eq, 0, 1));
    EXPECT_EQ(lexer.Process(">")[0], InputToken(gr, 0, 1));
}

TEST(Lexer, Lexicon)
{
    EXPECT_EQ(Lexicon(Syntax()).size(), 0);
    Syntax syntax {
        Rule{"operator", {Literal("<"), Symbol("cat")}},
        Rule{"cat",      {Literal(">")}},
    };
    EXPECT_EQ(Lexicon(syntax).size(), 3);

}

}