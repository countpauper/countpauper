#include "pch.h"
#include "Parser/Parser.h"
#include "Logic/Set.h"

namespace Angel
{
namespace Parser
{
namespace Test
{

    /* Reeanble with BNF
TEST(TestSet, Empty)
{
    Logic::Object set;
    std::wstringstream s(L"{} ");
    s >> set;

	EXPECT_EQ(set, Logic::set());
}


TEST(TestSet, Single)
{
    Logic::Object set;
    std::wstringstream s(L"{ cat}");
    s >> set;

    EXPECT_EQ(set, Logic::set(Logic::id(L"cat")));
}


TEST(TestSet, Multiple)
{
    Logic::Object set;
    std::wstringstream s(L"{ cat, dog, cat } ");
    s >> set;

    EXPECT_EQ(set, Logic::set(Logic::id(L"cat"), Logic::id(L"dog")));
}

TEST(TestSet, Nested)
{
    Logic::Object set;
    std::wstringstream s(L"{ {cat, dog }, cat}");
    s >> set;

	EXPECT_EQ(set, Logic::set(
		Logic::set(Logic::id(L"cat"), Logic::id(L"dog")), 
		Logic::id(L"cat")));
}
*/

}
}
}

