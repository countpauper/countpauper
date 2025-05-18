#include <gtest/gtest.h>
#include "Logic/Knowledge.h"
#include "Logic/Boolean.h"

namespace Angel::Logic::Test
{

TEST(Boolean, Construction)
{
	EXPECT_EQ(Boolean(false), Boolean(false));
	EXPECT_EQ(Boolean(true), Boolean(true));
	EXPECT_EQ(std::string(Boolean(true)), "true");
	EXPECT_EQ(std::string(Boolean(false)), "false");
	EXPECT_TRUE(Boolean("true"));
	EXPECT_FALSE(Boolean(false));
	EXPECT_THROW(Boolean("False"), std::invalid_argument);	
}


TEST(Boolean, Cast)
{
	Expression b = Boolean(true);
	EXPECT_EQ(b, Boolean(true));

	EXPECT_EQ(Boolean(Integer(0)), Boolean(false));
	EXPECT_EQ(Boolean(Integer(-1)), Boolean(true));
	EXPECT_EQ(Boolean(List{}), Boolean(false));
	EXPECT_EQ(Boolean(List{Boolean(false)}), Boolean(true));
}


TEST(Boolean, Infer)
{

	Knowledge k;
	EXPECT_EQ(k.Infer(Boolean(true)), Boolean(true));
	EXPECT_EQ(k.Infer(Boolean(false)), Boolean(false));
}


}
