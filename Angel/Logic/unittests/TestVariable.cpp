#include <gtest/gtest.h>
#include "Logic/Knowledge.h"
#include "Logic/Variable.h"
#include "Logic/Integer.h"

namespace Angel::Logic::Test
{

TEST(Variable, Expression)
{
    EXPECT_FALSE(Variable(""));
    EXPECT_TRUE(Variable("Test"));
	EXPECT_EQ(Variable("Test"), Variable("Test"));
	EXPECT_NE(Variable("Test"), Variable("Icle"));
}

TEST(Variable, Query)
{
	//Knowledge k;
	// TODO: this should return true and an (empty) set of all knowledge?
	//EXPECT_EQ(k.Query(Variable("Test")), Boolean(true));
	// TODO: this is the anonymous variable and should return true and no hypothesis
	//EXPECT_EQ(k.Query(Variable("")), Boolean(true));
}

/*
TEST(Variable, Matching)
{
	EXPECT_TRUE(Variable("X").Matching(Integer(1), {}));
    Variables substituted = {{"Y",integer(2)}};
	EXPECT_FALSE(Variable("Y").Matching(Integer(1), substituted) );
	EXPECT_TRUE(Variable("Y").Matching(Integer(2), substituted) );
}
*/
}
