#include <gtest/gtest.h>
#include "Engine/File/Json.h"

namespace Engine::Test
{

TEST(JSon, TryGet)
{
        json parsed = json::parse(R"""(
{
        "Foo": 5
}
        )""");

        EXPECT_EQ(*Engine::try_get<int>(parsed, "Foo"), 5);
        EXPECT_FALSE(Engine::try_get<int>(parsed, "Bar").has_value());
}

TEST(JSon, ValueOr)
{
        json parsed = json::parse(R"""(
{
        "Foo": 4
}
        )""");

        EXPECT_EQ(Engine::get_value_or<int>(parsed, "Foo"), 4);
        EXPECT_EQ(Engine::get_value_or<int>(parsed, "Bar", 3), 3);
}


}
