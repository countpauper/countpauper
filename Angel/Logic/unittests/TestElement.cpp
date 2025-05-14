#include <gtest/gtest.h>
#include "Logic/Knowledge.h"
#include "Logic/Boolean.h"
#include "Logic/Integer.h"

namespace Angel::Logic::Test
{

TEST(Element, Boolean)
{
	EXPECT_EQ(Logic::Boolean(false), Boolean(false));
	EXPECT_EQ(Logic::Boolean(true), Boolean(true));
	EXPECT_EQ(std::string(Boolean(true)), "true");
	EXPECT_EQ(std::string(Boolean(false)), "false");
	EXPECT_TRUE(Boolean("true"));
	EXPECT_FALSE(Boolean(false));
}

TEST(Element, ComputeBooleanReturnsConstant)
{
	Expression b = Boolean(true);
	EXPECT_EQ(b, Boolean(true));
	Knowledge k;
	EXPECT_EQ(k.Infer(Boolean(true)), Boolean(true));
	EXPECT_EQ(k.Infer(Boolean(false)), Boolean(false));
}

TEST(Element, Integer)
{
	EXPECT_FALSE(Integer(0));
	EXPECT_TRUE(Integer(1));
	EXPECT_EQ(Integer(3), Integer(3));
	EXPECT_NE(Integer(2), Integer(-3));
}

TEST(Element, ComputeIntegerReturnsConstant)
{
	Knowledge k;
	EXPECT_EQ(k.Infer(Integer(1)), Integer(1));
}

TEST(TestElement, Id)
{
	EXPECT_FALSE(Id(""));
	EXPECT_TRUE(Id("test"));
	EXPECT_EQ(Logic::Id("test"), Logic::Id("test"));
	EXPECT_NE(Logic::Id("test"), Logic::Id("icle"));
}

TEST(Element, ComputeIdReturnsConstant)
{
	Knowledge k;
	EXPECT_EQ(k.Infer(Id("test")), Id("test"));
	EXPECT_EQ(k.Infer(Id("")), Id(""));
}

}
