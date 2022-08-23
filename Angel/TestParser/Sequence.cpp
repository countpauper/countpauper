#include "pch.h"
#include "Parser/Parser.h"
#include "Logic/Sequence.h"
#include "Logic/Sequence.h"
#include <ios>

namespace Angel
{
namespace Parser
{
namespace Test
{


TEST(TestSequence, Empty)
{
    Logic::Object seq;
    std::stringstream s("( )");
    s >> seq;

    EXPECT_EQ(seq, Logic::sequence());
}

TEST(TestSequence, Single)
{
    Logic::Object seq;
    std::stringstream s("(cat)");
    s >> seq;
    EXPECT_EQ(seq, Logic::sequence(Logic::id("cat")));
}


TEST(TestSequence, Comma)
{
    Logic::Object seq;
    std::stringstream s("cat, dog");
    s >> seq;
    EXPECT_EQ(seq, Logic::sequence(Logic::id("cat"), Logic::id("dog")));
}

TEST(TestSequence, BracesComma)
{    Logic::Object seq;
    std::stringstream s("(cat, dog)");
    s >> seq;

    EXPECT_EQ(seq, Logic::sequence(Logic::id("cat"), Logic::id("dog")));
}


TEST(TestSequence, Commas)
{
    Logic::Object seq;
    std::stringstream s("cat, dog, hamster");
    s >> seq;

    EXPECT_EQ(seq, Logic::sequence(
		Logic::id("cat"), 
		Logic::id("dog"),
		Logic::id("hamster")));
}


TEST(TestSequence, DISABLED_SequenceInSequence)
{   // THis doesn't work yet, because cat,dog is a sequence returned by the parser, (cat,dog) is a sequence for the parser 
    // the braces expression matcher can not distinguish and create a nested sequence
    Logic::Object seq;
    std::stringstream s("((cat, dog))");
    s >> seq;
    EXPECT_EQ(seq, sequence(sequence(Logic::id("cat"), Logic::id("dog"))));
}

TEST(TestSequence, Nested)
{
    Logic::Object seq;
    std::stringstream s("( cat, (dog, hamster) )");
    s >> seq;

    EXPECT_EQ(seq, Logic::sequence(Logic::id("cat"), Logic::sequence(
        Logic::id("dog"), Logic::id("hamster"))));
}

TEST(TestSequence, Errors)
{
    Logic::Object seq;
    std::stringstream badOpen("cat, dog)"), badClose("(cat, dog"), badBraces("(cat, dog}");
    // TODO: succeeds but not a sequence EXPECT_THROW(badOpen >> seq, SyntaxError);
	EXPECT_THROW(badClose >> seq, SyntaxError);
	EXPECT_THROW(badBraces >> seq, SyntaxError);
}


}
}
}

