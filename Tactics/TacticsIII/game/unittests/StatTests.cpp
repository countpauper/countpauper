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
        def.Load(R"""(
{
        "Strength":{ }
}
        )""");
        EXPECT_EQ(def[Stat::str].Name(), "Strength");
}

TEST(Stat, define_secondary_stat)
{
        StatDefinition def;
        def.Load(R"""(
        {
                "Strength":{},
                "Damage":{ "Depends":"Strength", "table":[1,1,2,2,3,3] }
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
        def.Load(R"""(
        {
                "Agility":{ },
                "Dodge":{ "Depends":"Agility", "table":[10,11,12,12,13,13], "*":5 }
        }
        )""");
        MockStatted mock;
        EXPECT_CALL(mock, Get(Stat::agi)).WillOnce(Return(StatDescriptor(3)));
        EXPECT_CALL(mock, Definition()).WillRepeatedly(ReturnRef(def));
        EXPECT_EQ(def[Stat::dodge].Compute(mock).Total(), 60);
}

TEST(Stat, define_stat_multiplied_by_stat)
{
        StatDefinition def;
        def.Load(R"""(
        {
                "Level":{ },
                "Constitution":{ },
                "Hitpoints":{ "Depends":"Constitution", "table":[5, 5, 5, 6, 6], "*":"Level" }
        }
        )""");
        MockStatted mock;
        EXPECT_CALL(mock, Get(Stat::level)).WillOnce(Return(StatDescriptor(2)));
        EXPECT_CALL(mock, Get(Stat::con)).WillOnce(Return(StatDescriptor(3)));
        EXPECT_CALL(mock, Definition()).WillRepeatedly(ReturnRef(def));
        EXPECT_EQ(def[Stat::hp].Compute(mock).Total(), 12);
}


}
