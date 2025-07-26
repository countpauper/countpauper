#include <gtest/gtest.h>
#include "Logic/Knowledge.h"
#include "Logic/Integer.h"
#include "Logic/Expression.h"


namespace Angel::Logic::Test
{

TEST(Integer, Construction)
{
	EXPECT_FALSE(Integer(0));
	EXPECT_TRUE(Integer(1));
	EXPECT_EQ(Integer(3), Integer(3));
	EXPECT_NE(Integer(2), Integer(-3));
	EXPECT_EQ(Integer("5"), Integer(5));
	EXPECT_EQ(Integer("-4"), Integer(-4));
	EXPECT_THROW(Integer("3b"), std::invalid_argument);	
	EXPECT_THROW(Integer("9876543210"), std::out_of_range);	
	static_assert(sizeof(Integer)<=16);
}

TEST(Integer, Arithmetic)
{
	EXPECT_EQ(Integer(3) + Integer(2), Number(Integer(5)));
	EXPECT_EQ(Integer(5) - Integer(3), Number(Integer(2)));
	EXPECT_EQ(Integer(2) * Integer(3), Number(Integer(6)));
	EXPECT_EQ(Integer(6) / Integer(2), Number(Integer(3)));
	EXPECT_EQ(Integer(-3) ^ Integer(2), Number(Integer(9)));
	EXPECT_EQ(log(Integer(9), Integer(3)), Number(Integer(2)));
}

TEST(Integer, Cast)
{
	EXPECT_EQ(Integer(False), Integer(0));
	EXPECT_EQ(Integer(True), Integer(1));
	EXPECT_EQ(Expression(False).Cast(typeid(Integer)), Integer(0));
}

TEST(Integer, Infer)
{
	Knowledge k;
	EXPECT_EQ(k.Infer(Integer(1)), Integer(1));
}


}
