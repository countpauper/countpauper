#include <gtest/gtest.h>
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
    std::stringstream s("{} ");
    s >> set;

	EXPECT_EQ(set, Logic::set());
}


TEST(TestSet, Single)
{
    Logic::Object set;
    std::stringstream s("{ cat}");
    s >> set;

    EXPECT_EQ(set, Logic::set(Logic::id("cat")));
}


TEST(TestSet, Multiple)
{
    Logic::Object set;
    std::stringstream s("{ cat, dog, cat } ");
    s >> set;

    EXPECT_EQ(set, Logic::set(Logic::id("cat"), Logic::id("dog")));
}

TEST(TestSet, Nested)
{
    Logic::Object set;
    std::strinstream s("{ {cat, dog }, cat}");
    s >> set;

	EXPECT_EQ(set, Logic::set(
		Logic::set(Logic::id("cat"), Logic::id("dog")), 
		Logic::id("cat")));
}
*/

}
}
}

