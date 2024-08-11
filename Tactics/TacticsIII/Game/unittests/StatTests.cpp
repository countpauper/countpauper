#include <gtest/gtest.h>
#include "Game/Stat.h"
#include "Game/StatDefinition.h"
#include "Game/Mock/MockStatted.h"

namespace Game::Test
{
using namespace ::testing;

TEST(Stat, define_primary_stat)
{
    StatDefinition def(R"""(
{
        "Strength":{ "description":"It's how much you're stronk"}
}
    )""");
    EXPECT_EQ(def[Stat::str].Name(), "Strength");
    EXPECT_TRUE(def[Stat::str].Description().find("stronk")!=std::string::npos);
}

TEST(Stat, define_secondary_stat)
{
    StatDefinition def(R"""(
    {
            "Strength":{},
            "Damage":{ "depends":"Strength", "table":[1,1,2,2,3,3] }
    }
    )""");
    EXPECT_EQ(def[Stat::damage].Name(), "Damage");

    MockStatted mock;
    EXPECT_CALL(mock, Get(Stat::str, _, _)).WillOnce(Return(Computation(4, "Mock Str")));
    EXPECT_CALL(mock, Definition()).WillRepeatedly(ReturnRef(def));
    EXPECT_EQ(def[Stat::damage].Compute(mock).Total(), 3);
}

TEST(Stat, define_multiplied_stat)
{
    StatDefinition def(R"""(
    {
            "Agility":{ },
            "Dodge":{ "depends":"Agility", "table":[10,11,12,12,13,13], "*":5 }
    }
    )""");
    MockStatted mock;
    EXPECT_CALL(mock, Get(Stat::agi, _, _)).WillOnce(Return(Computation(3,"Mock Agi")));
    EXPECT_CALL(mock, Definition()).WillRepeatedly(ReturnRef(def));
    EXPECT_EQ(def[Stat::dodge].Compute(mock).Total(), 60);
}



TEST(Stat, define_stat_with_limit)
{
    StatDefinition def(R"""(
    {
            "Reach":{"min": 1}}
    )""");

    MockStatted mock;
    EXPECT_EQ(def[Stat::reach].Compute(mock).Total(), 1);
}

TEST(Stat, define_stat_multiplied_by_stat)
{
    StatDefinition def(R"""(
    {
            "Level":{ },
            "Constitution":{ },
            "Hitpoints":{ "depends":"Constitution", "table":[5, 5, 5, 6, 6], "*":"Level" }
    }
    )""");
    MockStatted mock;
    EXPECT_CALL(mock, Get(Stat::level, _, _)).WillOnce(Return(Computation(2, "Mock2")));
    EXPECT_CALL(mock, Get(Stat::con, _, _)).WillOnce(Return(Computation(3, "Mock3")));
    EXPECT_CALL(mock, Definition()).WillRepeatedly(ReturnRef(def));
    EXPECT_EQ(def[Stat::hp].Compute(mock).Total(), 12);
}

TEST(Stat, constant_stat)
{
    int table[] = { 3 };
    Stat constant("const", "", Stat::Id::none, table);
    MockStatted mock;
    EXPECT_CALL(mock, Get(_, _, _)).Times(0);
    EXPECT_EQ(constant.Compute(mock).Total(), 3);
}

TEST(Stat, define_unknown_stat_throws)
{
    EXPECT_THROW(StatDefinition(R"""(
{
        "Fubar":{ "description":"It's not a real stat"}
}
    )"""), std::invalid_argument);;
}

TEST(Stat, define_counter)
{
    StatDefinition def(R"""(
{
    "Hitpoints": {
        "counter": {
        }
    }
}
    )""");
    EXPECT_EQ(def.GetCounters().size(), 1);
    EXPECT_EQ(def.GetCounters().front()->Name(), "Hitpoints");
}


}
