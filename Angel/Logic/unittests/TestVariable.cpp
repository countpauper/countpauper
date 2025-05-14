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
	Knowledge k;
	// TODO: to be defined what the hypothesis is. either the whole root namespace 
	// or some predefined universal set that could be anything
	// for the anonyomous variable the hyptohesis is elided 
	EXPECT_EQ(k.Infer(Variable("Test")), Boolean(true));
	EXPECT_EQ(k.Infer(Variable("")), Boolean(true));
	Variables vars{{"Test", Integer(4)}};
	EXPECT_EQ(Variable("Test").Infer(k, vars), Integer(4));
}

TEST(Variable, Matching)
{
	EXPECT_TRUE(Variable("X").Matches(Integer(1), {}));
    Variables substituted = {{"Y",Integer(2)}};
	EXPECT_FALSE(Variable("Y").Matches(Integer(1), substituted) );
	EXPECT_TRUE(Variable("Y").Matches(Integer(2), substituted) );
}
}
