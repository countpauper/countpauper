#include "Interpreter/SourceSpan.h"
#include "Interpreter/Source.h"
#include "Interpreter/Utils.h"
#include <gtest/gtest.h>

namespace Interpreter::Test 
{


TEST(SourceSpan, Empty)
{
    EXPECT_TRUE(SourceSpan().empty());
    EXPECT_TRUE(SourceSpan(1,0).empty());
    EXPECT_FALSE(SourceSpan(0,1).empty());
}
    
TEST(SourceSpan, Size)
{
    EXPECT_EQ(SourceSpan().size(), 0);
    EXPECT_EQ(SourceSpan(1,0).size(), 0);
    EXPECT_EQ(SourceSpan(0,1).size(), 1);
}

TEST(SourceSpan, Equal)
{
    EXPECT_EQ(SourceSpan(), SourceSpan(0,0));
    EXPECT_EQ(SourceSpan(0,1), SourceSpan(0,1));
    EXPECT_NE(SourceSpan(0,2), SourceSpan(0,1));
    EXPECT_NE(SourceSpan(1,1), SourceSpan(0,1));
    Source a, b;
    EXPECT_NE(SourceSpan(0,1,&a), SourceSpan(0,1,&b));
}

TEST(SourceSpan, Sub)
{
    EXPECT_EQ(SourceSpan(0, 5).sub(1, 2), SourceSpan(1,2));
    EXPECT_EQ(SourceSpan(3, 4).sub(1, 3), SourceSpan(4,3));
    EXPECT_EQ(SourceSpan(3, 3).sub(1, 3), SourceSpan(4,2));
    EXPECT_EQ(SourceSpan(2, 2).sub(1, 3), SourceSpan(3,1));
    EXPECT_EQ(SourceSpan(3, 2).sub(-2, 3), SourceSpan(1,3));
    EXPECT_EQ(SourceSpan(2, 2).sub(-3, 3), SourceSpan(0, 3));
    EXPECT_EQ(SourceSpan(2, 4).sub(1, -1), SourceSpan(3, 2));
    EXPECT_EQ(SourceSpan(3, 3).sub(1, -3), SourceSpan(4, 0));
    Source cat("cat");
    EXPECT_EQ(cat.span(0,3), SourceSpan(0, 3, &cat));
}

TEST(SourceSpan, Extract)
{
    Source s("Extract");
    EXPECT_EQ(SourceSpan(1,0).extract(), "");
    EXPECT_EQ(SourceSpan(0,5, &s).extract(), "Extra");
    // These aren't clipped. If clipping is needed, call SourceSpan.sub() first
    EXPECT_THROW(SourceSpan(4,4, &s).extract(), std::out_of_range);
    EXPECT_THROW(SourceSpan(10,3, &s).extract(), std::out_of_range);
}

TEST(Utils, Unclose)
{
    EXPECT_EQ(Unclose("\"literal\"", '"'), "literal");
    EXPECT_EQ(Unclose("'regex'", '\''), "regex");
    EXPECT_EQ(Unclose("<symbol>", '<','>'), "symbol");
    EXPECT_THROW(Unclose("\"", '"'), std::invalid_argument);
    EXPECT_THROW(Unclose("\"open", '"'), std::invalid_argument);
    EXPECT_THROW(Unclose("open\"", '"'), std::invalid_argument);
    
}

};
