#include "Logic/Knowledge.h"
#include "Logic/Expression.h"
#include <gtest/gtest.h>

namespace Angel::Logic::Test
{

TEST(Knowledge, KnowSomething)
{
    Knowledge k;
    auto defaultSize = k.Root().size();
    EXPECT_EQ(k.Knows(Predicate("ginny")), Integer(0));
    EXPECT_EQ(k.Know(Predicate("ginny")), 1);
    EXPECT_EQ(k.Root().size(), defaultSize + 1);
    EXPECT_EQ(k.Knows(Predicate("ginny")), Integer(1));
}

TEST(Knowledge, AlreadyKnows)
{
    Knowledge k;
    auto defaultSize = k.Root().size();
    EXPECT_EQ(k.Know(Predicate("ginny")), 1);
    EXPECT_EQ(k.Know(Predicate("ginny")), 0);
    EXPECT_EQ(k.Root().size(), defaultSize+1);
}


TEST(Knowledge, Forget)
{
    Knowledge k { Predicate("ginny") };
    auto originalSize = k.Root().size();
    EXPECT_EQ(k.Forget(Predicate("ginny")), 1);
    EXPECT_EQ(k.Root().size(), originalSize-1);
}

TEST(Knowledge, Defaults)
{
    Knowledge k;
    EXPECT_EQ(k.Root().size(), 2); 
    EXPECT_EQ(k.Knows(Predicate("true")), Integer(1));
    EXPECT_EQ(k.Infer(Predicate("false")), False);
}

TEST(Knowledge, Matches)
{
    Knowledge k;
    EXPECT_TRUE(k.Matches(Predicate("ginny")).empty());
    k.Know(Predicate("ginny"));
    EXPECT_EQ(k.Matches(Predicate("ginny")).size(), 1);
    k.Know(Association{Predicate("ginny"), Predicate("cat")});
//  If this is desired, then instead of inferring, at least Matches should substitute variables 
//    along the clause. Without (paritally) computing operations this is anyway not possible
//    EXPECT_EQ(k.Matches(Predicate("ginny")), (Disjunction{True, Predicate("cat")}));
    EXPECT_EQ(k.Matches(Predicate("ginny")), (Bag{True, Predicate("cat")}));
}

TEST(Knowledge, MatchVariables)
{
    Knowledge k{
       Association(Predicate("fuzzy",{Variable("X")}), Predicate("cat", {Variable("X")}))};
    EXPECT_EQ(k.Knows(Predicate("fuzzy", {All("args")})),
        (Bag{
            Association(Predicate("cat", {Variable("X")}), 
                        Equal{Variable("args"), List{Variable("X")}})
        }));    
    EXPECT_EQ(k.Matches(Predicate("fuzzy",{Id("ginny")})).front(), 
        (Association(Predicate("cat", {Variable("X")}), Conjunction{Equal{Id("ginny"), Variable("X")}})));
}

TEST(Knowledge, MultipleMatches)
{
    Knowledge k{
       Predicate("cat",{Id("gizmo")}), 
       Predicate("cat",{Id("ginny")}) };
    
    EXPECT_EQ(k.Knows(Predicate("cat", {All("args")})),
        (Bag{
            Association(True, Equal{Variable("args"), List{Id("gizmo")}}),
            Association(True, Equal{Variable("args"), List{Id("ginny")}})
        }));
    EXPECT_EQ(k.Matches(Predicate("cat",{Variable("X")})),
        (Bag{Association(True, Conjunction{Equal{Variable("X"), Id("gizmo")}}),
            Association(True, Conjunction{Equal{Variable("X"), Id("ginny")}})}));

    
}

}