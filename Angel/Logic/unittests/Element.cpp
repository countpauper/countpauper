#include <gtest/gtest.h>
#include "Logic/Knowledge.h"
#include "Logic/Boolean.h"
#include "Logic/Integer.h"

namespace Angel::Logic::Test
{

TEST(TestElement, Boolean)
{
	EXPECT_EQ(Logic::Boolean(false), Logic::Boolean(false));
	EXPECT_EQ(Logic::Boolean(true), Logic::Boolean(true));
	EXPECT_EQ(std::string(Boolean(true)), "true");
	EXPECT_EQ(std::string(Boolean(false)), "false");
	EXPECT_TRUE(Boolean("true"));
	EXPECT_FALSE(Boolean(false));
}

TEST(TestElement, ComputeBooleanReturnsConstant)
{
	Object b = Boolean(true);
	EXPECT_EQ(b, Boolean(true));
	Knowledge k;
	EXPECT_EQ(k.Compute(Boolean(true)), Object(Boolean(true)));
	EXPECT_EQ(k.Compute(Boolean(false)), Object(Boolean(false)));
}

TEST(TestElement, Integer)
{
	EXPECT_FALSE(Integer(0));
	EXPECT_TRUE(Integer(1));
	EXPECT_EQ(Integer(3), Integer(3));
	EXPECT_NE(Integer(2), Integer(-3));
}

TEST(TestElement, ComputeIntegerReturnsConstant)
{
	Knowledge k;
	EXPECT_EQ(k.Compute(Integer(1)), Integer(1));
}

TEST(TestElement, Id)
{
	EXPECT_FALSE(Id(""));
	EXPECT_TRUE(Id("test"));
	EXPECT_EQ(Logic::Id("test"), Logic::Id("test"));
	EXPECT_NE(Logic::Id("test"), Logic::Id("icle"));
}

TEST(TestElement, ComputeIdQueriesPredicate)
{
	Knowledge k;
	EXPECT_EQ(k.Compute(Id("test")), Boolean("false"));
	EXPECT_EQ(k.Compute(Id("")), Boolean(false));
}

}
