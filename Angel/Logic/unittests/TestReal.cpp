#include <gtest/gtest.h>
#include "Logic/Knowledge.h"
#include "Logic/Real.h"
#include "Logic/Expression.h"
#include <cmath>

namespace Angel::Logic::Test
{

TEST(Real, Construction)
{
	EXPECT_FALSE(Real(0));
	EXPECT_TRUE(Real(1));
	EXPECT_EQ(Real(3), Real(3.0));
	EXPECT_NE(Real(2), Real(-3));
	EXPECT_EQ(Real("0.5"), Real(0.5));
	EXPECT_EQ(Real("-4"), Real(-4));
	EXPECT_EQ(Real("1.0e-3"), Real(0.001));
	EXPECT_EQ(Real("-inf"), Real(-std::numeric_limits<double>::infinity()));
	EXPECT_TRUE(std::isnan(double(Real("NaN"))));
	EXPECT_THROW(Real("3b"), std::invalid_argument);	
	EXPECT_THROW(Real("1.0e20000"), std::out_of_range);	
	static_assert(sizeof(Real)<=16);
}

TEST(Real, Arithmetic)
{
	EXPECT_EQ(Real(2) + Real(0.5), Real(2.5));
	EXPECT_EQ(Real(2.5) - Real(1.25), Real(1.25));
	EXPECT_EQ(Real(1.25) * Real(4), Integer(5));
	EXPECT_EQ(Real(5) / Real(2), Real(2.5));
	EXPECT_EQ(Real(2.5) ^ Real(2), Real(6.25));
	EXPECT_EQ(log(Real(6.25), Real(2.5)), Integer(2));
}

TEST(Real, Cast)
{
	EXPECT_EQ(Real(True), Real(1));
	EXPECT_EQ(Real(Integer(-4)), Real(-4.0));
	EXPECT_EQ(Expression(Integer(2)).Cast(typeid(Real)), Real(2.0));
}

TEST(Real, Infer)
{
	Knowledge k;
	EXPECT_EQ(k.Infer(Real(1)), Real(1));
}


}
