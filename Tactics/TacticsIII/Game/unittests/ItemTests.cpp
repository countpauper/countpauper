#include <gtest/gtest.h>
#include "Game/ItemDatabase.h"
#include "Definition.h"
#include <nlohmann/json.hpp>

namespace Game::Test
{
using namespace ::testing;

TEST(Item, Load)
{
    auto json = nlohmann::json::parse(R"""({
        "name":"Test",
        "damage": 3
})""");

    Definition def(Item::definition);
    def.Define(Stat::damage);
    Item item(json);
    EXPECT_EQ(item.Name(), "Test");
    EXPECT_EQ(item.Get(Stat::damage).Total(), 3);
}


}
