#include <gtest/gtest.h>
#include "Logic/Knowledge.h"
#include "Logic/Variable.h"
#include "Logic/Integer.h"

namespace Angel::Logic::Test
{

TEST(TestVariable, Expression)
{
    EXPECT_FALSE(Variable(""));
    EXPECT_TRUE(Variable("Test"));
	EXPECT_EQ(Variable("Test"), Variable("Test"));
	EXPECT_NE(Variable("Test"), Variable("Icle"));
	EXPECT_NE(Variable("Test"), Boolean(true));
}

TEST(TestVariable, Query)
{
	Knowledge k;
	EXPECT_FALSE(k.Query(var("Test")));
	EXPECT_FALSE(k.Query(var("")));
}


TEST(TestVariable, Matching)
{
	EXPECT_TRUE(Variable("X").Matching(Integer(1), {}));
    Variables substituted = {{"Y",integer(2)}};
	EXPECT_FALSE(Variable("Y").Matching(Integer(1), substituted) );
	EXPECT_TRUE(Variable("Y").Matching(Integer(2), substituted) );
}
}
