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
        "Strength":{ "Id":2 }
}
        )""");
        EXPECT_EQ(def[Stat::str].Name(), "Strength");
}

TEST(Stat, define_secondary_stat)
{
        StatDefinition def;
        def.Load(R"""(
{
        "Strength":{ "Id":2 },
        "Damage":{ "Id": 4, "Depends":"Strength", "table":[1,1,2,2,3,3] }
}
        )""");
        EXPECT_EQ(def[Stat::damage].Name(), "Damage");

        MockStatted mock;
        EXPECT_CALL(mock, Get(Stat::str)).WillOnce(Return(StatDescriptor(4)));
        EXPECT_CALL(mock, Definition()).WillRepeatedly(ReturnRef(def));
        EXPECT_EQ(def[Stat::damage].Compute(mock).Total(), 3);

}


}
