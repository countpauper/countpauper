#include "Interpreter/Lexer.h"
#include "Interpreter/Terms.h"
#include "Interpreter/Error.h"
#include "Interpreter/Syntax.h"
#include "RangerMatcher.h"
#include <gtest/gtest.h>
#include <gmock/gmock.h>

namespace Interpreter::Test
{


template<typename T>
inline auto RangeEq(std::initializer_list<T> ilist) {
    return RangeEqVector(std::vector<T>(ilist));
}

TEST(Lexer, Empty)
{
    Lexer lexer(Lexicon{});
    EXPECT_THAT(lexer.Process(""), RangeEq({ InputToken() }));
    EXPECT_THROW(lexer.Process("cat"), Error);
}


TEST(Lexer, Literal)
{
    Term cat{Literal("cat")};
    Lexer lexer(Lexicon{&cat});
    EXPECT_THAT(lexer.Process(""), RangeEq({ InputToken() }));
    EXPECT_THAT(lexer.Process("cat"), RangeEq({InputToken(cat, 0, 3), InputToken(0, 3, 0) }));
    EXPECT_THROW(lexer.Process("dog"), Error);
}

TEST(Lexer, Regex)
{
    Term whitespace = Regex("\\s+");
    Lexer lexer(Lexicon{&whitespace});
    EXPECT_THAT(lexer.Process(""), RangeEq({InputToken()}));
    EXPECT_THAT(lexer.Process("\t "), RangeEq({InputToken(whitespace, 0, 2), InputToken(0,2,0)}));
    EXPECT_THROW(lexer.Process("not space"), Error);
}

TEST(Lexer, NotAmbiguous)
{
    Term greq = Literal(">=");
    Term gr = Literal(">");
    Term eq = Literal("=");
    Lexer lexer(Lexicon{ &gr, &eq, &greq });
    EXPECT_THAT(lexer.Process(">="), RangeEq({InputToken(greq, 0, 2), InputToken(0,2,0)}));
    EXPECT_THAT(lexer.Process("=>"), RangeEq({InputToken(eq, 0, 1), InputToken(gr,1,1), InputToken(0,2,0)}));
    EXPECT_THAT(lexer.Process(">"), RangeEq({InputToken(gr, 0, 1), InputToken(0,1,0)}));
}

TEST(Lexer, Lexicon)
{
    EXPECT_EQ(Lexicon(Syntax()).size(), 0);
    Syntax syntax {
        Rule{"operator", {Literal("<"), Symbol("cat")}},
        Rule{"cat",      {Literal(">")}},
    };
    EXPECT_EQ(Lexicon(syntax).size(), 2);
}

}