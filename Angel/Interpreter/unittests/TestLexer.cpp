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


static Source empty("");
static Source cat("cat");
static Source whitespace("\t ");
static Source unicode("ç≤π");

TEST(Lexer, Empty)
{
    Lexer lexer(Lexicon{});
    EXPECT_THAT(lexer.Process(empty), RangeEq({ InputToken{0,{0,0,&empty} }}));
    EXPECT_THROW(lexer.Process(cat), Error);
}


TEST(Lexer, Literal)
{
    Term term{Literal("cat")};
    Lexer lexer(Lexicon{&term});

    EXPECT_THAT(lexer.Process(empty), RangeEq({ InputToken(0,{0,0,&empty}) }));
    EXPECT_THAT(lexer.Process(cat), RangeEq({InputToken(term, {0, 3, &cat}), InputToken(0, {3, 0, &cat}) }));
    EXPECT_THROW(lexer.Process(whitespace), Error);
}

TEST(Lexer, Unicode)
{
    Term byte = Literal("ç");
    Term regexUnicode = Regex("[∀-⋿]");
    Term two_byte = Literal("π");
    Lexer lexer(Lexicon{&byte, &regexUnicode, &two_byte});
    auto tokens = lexer.Process(unicode);
    EXPECT_THAT(tokens, RangeEq({
        InputToken(byte, unicode.span(0,2)),
        InputToken(regexUnicode, unicode.span(2,3)),
        InputToken(two_byte, unicode.span(5,2)),
        InputToken(0, unicode.span(7,0))
    }));
} 

TEST(Lexer, Regex)
{
    Term term = Regex("\\s+");
    Lexer lexer(Lexicon{&term});
    EXPECT_THAT(lexer.Process(empty), RangeEq({InputToken(0, empty.span(0,0))}));
    EXPECT_THAT(lexer.Process(whitespace), RangeEq(
        {InputToken(term, whitespace.span(0,2)), 
        InputToken(0,whitespace.span(2,0))}));
    EXPECT_THROW(lexer.Process(cat), Error);
}

TEST(Lexer, NotAmbiguous)
{
    Term greq = Literal(">=");
    Term gr = Literal(">");
    Term eq = Literal("=");
    Lexer lexer(Lexicon{ &gr, &eq, &greq });
    Source srcGreq(">=");
    EXPECT_THAT(lexer.Process(srcGreq), RangeEq({
        InputToken(greq, srcGreq.span(0, 2)), 
        InputToken(0, srcGreq.span(2,0))}));
    Source srcEqgr("=>");
    EXPECT_THAT(lexer.Process(srcEqgr), RangeEq({
        InputToken(eq, srcEqgr.span(0,1)), 
        InputToken(gr, srcEqgr.span(1,1)), 
        InputToken(0, srcEqgr.span(2,0))}));
    Source srcGr(">");
    EXPECT_THAT(lexer.Process(srcGr), RangeEq({
        InputToken(gr, srcGr.span(0,1)), 
        InputToken(0, srcGr.span(1,0))}));
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