#include <gtest/gtest.h>
#include "Game/Stat.h"
#include "Game/StatDefinition.h"
#include "Game/Mock/MockStatted.h"

namespace Game::Test
{
using namespace ::testing;

TEST(Stat, define_primary_stat)
{
    StatDefinition def;
    def.Parse(R"""(
{
        "Strength":{ "description":"It's how much you're stronk"}
}
    )""");
    EXPECT_EQ(def[Stat::str].Name(), "Strength");
    EXPECT_TRUE(def[Stat::str].Description().find("stronk")!=std::string::npos);
}

TEST(Stat, define_secondary_stat)
{
    StatDefinition def;
    def.Parse(R"""(
    {
            "Strength":{},
            "Damage":{ "depends":"Strength", "table":[1,1,2,2,3,3] }
    }
    )""");
    EXPECT_EQ(def[Stat::damage].Name(), "Damage");

    MockStatted mock;
    EXPECT_CALL(mock, Get(Stat::str)).WillOnce(Return(StatDescriptor(4)));
    EXPECT_CALL(mock, Definition()).WillRepeatedly(ReturnRef(def));
    EXPECT_EQ(def[Stat::damage].Compute(mock).Total(), 3);
}

TEST(Stat, define_multiplied_stat)
{
    StatDefinition def;
    def.Parse(R"""(
    {
            "Agility":{ },
            "Dodge":{ "depends":"Agility", "table":[10,11,12,12,13,13], "*":5 }
    }
    )""");
    MockStatted mock;
    EXPECT_CALL(mock, Get(Stat::agi)).WillOnce(Return(StatDescriptor(3)));
    EXPECT_CALL(mock, Definition()).WillRepeatedly(ReturnRef(def));
    EXPECT_EQ(def[Stat::dodge].Compute(mock).Total(), 60);
}



TEST(Stat, define_stat_with_limit)
{
    StatDefinition def;
    def.Parse(R"""(
    {
            "Reach":{"min": 1}}
    )""");

    MockStatted mock;
    EXPECT_EQ(def[Stat::reach].Compute(mock).Total(), 1);
}

TEST(Stat, define_stat_multiplied_by_stat)
{
    StatDefinition def;
    def.Parse(R"""(
    {
            "Level":{ },
            "Constitution":{ },
            "Hitpoints":{ "depends":"Constitution", "table":[5, 5, 5, 6, 6], "*":"Level" }
    }
    )""");
    MockStatted mock;
    EXPECT_CALL(mock, Get(Stat::level)).WillOnce(Return(StatDescriptor(2)));
    EXPECT_CALL(mock, Get(Stat::con)).WillOnce(Return(StatDescriptor(3)));
    EXPECT_CALL(mock, Definition()).WillRepeatedly(ReturnRef(def));
    EXPECT_EQ(def[Stat::hp].Compute(mock).Total(), 12);
}

TEST(Stat, define_unknown_stat_throws)
{
    StatDefinition def;
    EXPECT_THROW(def.Parse(R"""(
{
        "Fubar":{ "description":"It's not a real stat"}
}
    )"""), std::runtime_error);;
}

TEST(Stat, define_counter)
{
    StatDefinition def;
    def.Parse(R"""(
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
