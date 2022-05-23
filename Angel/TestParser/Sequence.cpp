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

TEST(TestSequence, Comma)
{
    Logic::Object seq;
    std::wstringstream s(L"(cat, dog)");
    s >> seq;

	EXPECT_EQ(seq, Logic::sequence(Logic::id(L"cat"), Logic::id(L"dog")));
}

TEST(TestSequence, MakeSequence)
{
    std::wstringstream s(L"(cat,)");
    Logic::Object seq;
    s >> seq;

    EXPECT_EQ(seq, Logic::sequence(Logic::id(L"cat")));
}

TEST(TestSequence, Commas)
{
    Logic::Object seq;
    std::wstringstream s(L"( cat, dog, hamster )");
    s >> seq;

    EXPECT_EQ(seq, Logic::sequence(
		Logic::id(L"cat"), 
		Logic::id(L"dog"),
		Logic::id(L"hamster")));
}


TEST(TestSequence, Empty)
{
    Logic::Object seq;
    std::wstringstream s(L"( )");
    s >> seq;

    EXPECT_EQ(seq, Logic::sequence());
}

TEST(TestSequence, Single)
{
    Logic::Object seq;
    std::wstringstream s(L"( cat)");
    s >> seq;
    EXPECT_EQ(seq, Logic::sequence(Logic::id(L"cat")));
}


TEST(TestSequence, Multiple)
{
    Logic::Object seq;
    std::wstringstream s(L"( cat, dog )");
    s >> seq;

    EXPECT_EQ(seq, Logic::sequence(Logic::id(L"cat"), Logic::id(L"dog")));
}

TEST(TestSequence, SequenceInSequence)
{
    Logic::Object seq;
    std::wstringstream s(L"((cat, dog))");
    s >> seq;
    EXPECT_EQ(seq, sequence(sequence(Logic::id(L"cat"), Logic::id(L"dog"))));
}

TEST(TestSequence, Errors)
{
    Logic::Object seq;
    std::wstringstream badOpen(L"cat, dog)"), badClose(L"(cat, dog"), badBraces(L"(cat, dog}");
    EXPECT_THROW(badOpen >> seq, SyntaxError);
	EXPECT_THROW(badClose >> seq, SyntaxError);
	EXPECT_THROW(badBraces >> seq, SyntaxError);
}



TEST(TestSequence, Nested)
{
    Logic::Object seq;
    std::wstringstream s(L"( cat, (dog, hamster) )");
    s >> seq;

    EXPECT_EQ(seq, Logic::sequence(Logic::id(L"cat"), Logic::sequence(
        Logic::id(L"dog"), Logic::id(L"hamster"))));
}

}
}
}

