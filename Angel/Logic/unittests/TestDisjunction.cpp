#include <gtest/gtest.h>
#include "Logic/Knowledge.h"
#include "Logic/Disjunction.h"
#include "Logic/Boolean.h"
#include "Logic/Predicate.h"

namespace Angel::Logic::Test
{

TEST(Disjunction, Construction)
{
    EXPECT_TRUE(Disjunction{}.empty());
    EXPECT_EQ(Disjunction{Boolean(false)}.size(), 1);    
    EXPECT_EQ(Expression(BinaryOperator(L'|'), {Boolean(true), Boolean(false)}), (Disjunction{Boolean(true), Boolean(false)}));   

	static_assert(Logic::IsOperation<Disjunction>);
}

TEST(Disjunction, Conjunctions)
{
    Knowledge k;
    EXPECT_EQ(Disjunction{}.Infer(k), Boolean(false));
    EXPECT_EQ((Disjunction{Boolean(true)}).Infer(k), Boolean(true));
    EXPECT_EQ((Disjunction{Boolean(true), Boolean(false)}).Infer(k), Boolean(true));
    EXPECT_EQ((Disjunction{Disjunction{Boolean(true)}}).Infer(k), Boolean(true));
    EXPECT_EQ((Disjunction{Disjunction{Boolean(false), Integer(3), Integer(2)}}).Infer(k), Integer(3));
}

TEST(Disjunction, Nest)
{
    Knowledge k;
    EXPECT_EQ((Disjunction{Disjunction{Boolean(false)}, Disjunction{Boolean(true)}}).Infer(k), Boolean(true));
    EXPECT_EQ((Disjunction{Predicate("cat")}).Infer(k), Boolean(false));
}

TEST(Disjunction, Inference)
{
    Knowledge k;
    k.Know(Predicate("cat"));
    EXPECT_EQ((Disjunction{Predicate("cat")}).Infer(k), Boolean(true));
}

TEST(Disjunction, to_string)
{
    EXPECT_EQ(to_string(Disjunction{Boolean(true)}), "true");
    EXPECT_EQ(to_string(Disjunction{Boolean(true), Boolean(false)}), "true|false");
}

}

