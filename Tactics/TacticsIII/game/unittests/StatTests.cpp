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
        EXPECT_EQ(def[Stat::Str].Name(), "Strength");
}

TEST(Stat, define_secondary_stat)
{
        StatDefinition def;
        def.Load(R"""(
{
        "Strength":{ "Id":2 },
        "Damage":{ "Id": 4, "Depends":"Strength" }
}
        )""");
        EXPECT_EQ(def[Stat::Damage].Name(), "Damage");

        MockStatted mock;
        EXPECT_CALL(mock, Get(Stat::Str)).WillOnce(Return(StatDescriptor(5)));
        EXPECT_CALL(mock, Definition()).WillRepeatedly(ReturnRef(def));
        EXPECT_EQ(def[Stat::Damage].Compute(mock).Total(), 5);

}


}
