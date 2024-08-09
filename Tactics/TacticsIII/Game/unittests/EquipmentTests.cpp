#include <gtest/gtest.h>
#include "Game/Equipments.h"
#include "Definition.h"

namespace Game::Test
{
using namespace ::testing;

TEST(Equipment, Equip)
{
    Equipments equip;
    Item item("Test", {});
    equip.Equip(Equipment(item));
    EXPECT_EQ(equip.GetEquipped().front()->GetItem(), item);
}

TEST(Equipment, Unequip)
{
    Equipments equip;
    Item item("Test", {});
    equip.Equip(Equipment(item));
    EXPECT_TRUE(equip.Unequip(*equip.GetEquipped().front()));
    EXPECT_TRUE(equip.GetEquipped().empty());
}

TEST(Equipment, UnequipByTag)
{
    Equipments equip;
    Item item("Test", {Restriction::metal});
    equip.Equip(Equipment(item));
    EXPECT_EQ(equip.Unequip({Restriction::wood}), 0);
    EXPECT_EQ(equip.Unequip({Restriction::metal}), 1);
    EXPECT_TRUE(equip.GetEquipped().empty());
}

TEST(Equipment, Stats)
{
    Definition def(Item::definition);
    def.Define(Stat::range);
    Equipments equip;
    Item item("Test", {Restriction::metal});
    item.Set(Stat::range, 5);
    equip.Equip(Equipment(item));
    EXPECT_FALSE(bool(equip.Get(Stat::reach)));
    EXPECT_EQ(equip.Get(Stat::range).Total(), 5);
    EXPECT_EQ(equip.Get(Stat::range, {Restriction::cloth}).Total(), 0);
    EXPECT_EQ(equip.Get(Stat::range, {Restriction::metal}).Total(), 5);
}

TEST(Equipment, StatContributions)
{
    Definition def(Item::definition);
    def.Define(Stat::weight);
    Equipments equip;
    Item item3("Test3", {Restriction::metal});
    item3.Set(Stat::weight, 3);
    equip.Equip(Equipment(item3));

    Item item2("Test2", {Restriction::leather});
    item2.Set(Stat::weight, 2);
    equip.Equip(Equipment(item2));

    EXPECT_EQ(equip.Get(Stat::weight, {}).Total(), 5);
    EXPECT_EQ(equip.Get(Stat::weight, {Restriction::metal}).Total(), 3);
    EXPECT_EQ(equip.Get(Stat::weight, {Restriction::leather}).Total(), 2);
}

}
