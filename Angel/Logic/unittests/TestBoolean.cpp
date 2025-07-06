#include <gtest/gtest.h>
#include "Logic/Knowledge.h"
#include "Logic/Boolean.h"
#include "Logic/Expression.h"


namespace Angel::Logic::Test
{

TEST(Boolean, Construction)
{
	EXPECT_EQ(False, False);
	EXPECT_EQ(True, True);
	EXPECT_EQ(std::string(True), "true");
	EXPECT_EQ(std::string(False), "false");
	EXPECT_TRUE(Boolean("true"));
	EXPECT_FALSE(False);
	EXPECT_THROW(Boolean("False"), std::invalid_argument);	
}

TEST(Boolean, And)
{
	EXPECT_EQ(True & True, True);
	EXPECT_EQ(True & False, False);
	EXPECT_EQ(False & True, False);
}

TEST(Boolean, Or)
{
	EXPECT_EQ(True | False, True);
	EXPECT_EQ(False | True, True);
	EXPECT_EQ(False | False, False);
}

TEST(Boolean, Cast)
{
	Expression b = True;
	EXPECT_EQ(b, True);

	EXPECT_EQ(Boolean(Integer(0)), False);
	EXPECT_EQ(Boolean(Integer(-1)), True);
	EXPECT_EQ(Boolean(List{}), False);
	EXPECT_EQ(Boolean(List{False}), True);
	EXPECT_EQ(Expression(Integer(1)).Cast(typeid(Boolean)), True);
}


TEST(Boolean, Infer)
{

	Knowledge k;
	EXPECT_EQ(k.Infer(True), True);
	EXPECT_EQ(k.Infer(False), False);
}


}
