#include <gtest/gtest.h>
#include "Logic/Knowledge.h"
#include "Logic/Conjunction.h"
#include "Logic/Boolean.h"
#include "Logic/Predicate.h"

namespace Angel::Logic::Test
{

TEST(TestComputation, Disjunctions)
{
    Knowledge k;
    // TODO: not all expression should be knowable. Elements and operators are not the same as predicates, clauses and ... formulas? 
    EXPECT_TRUE(conjunction().Compute(k));
    EXPECT_TRUE(conjunction(boolean(true)).Compute(k));
    EXPECT_FALSE(conjunction(boolean(true), boolean(false)).Compute(k));
    EXPECT_TRUE(conjunction(conjunction(boolean(true))).Compute(k));
    EXPECT_FALSE(conjunction(conjunction(boolean(false)), conjunction(boolean(true))).Compute(k));
    EXPECT_FALSE(conjunction(predicate("cat")));
    k.Know(predicate("cat", Sequence()));
    EXPECT_TRUE(conjunction(predicate("cat")).Compute(k));
}

}

