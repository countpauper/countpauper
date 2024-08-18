#include <gtest/gtest.h>
#include "Game/Equipments.h"
#include "Game/ItemBonus.h"
#include "Game/ItemDatabase.h"
#include "Definition.h"

#include "Game/Mock/MockBoni.h"
#include "Game/Mock/MockStatted.h"
#include "Game/Mock/MockEquipped.h"

namespace Game::Test
{
using namespace ::testing;

TEST(Equipment, equip)
{
    Equipments equip;
    Item item("Test", {});
    equip.Equip(Equipment(item));
    EXPECT_EQ(equip.GetEquipped().front()->GetItem(), item);
}

TEST(Equipment, unequip)
{
    Equipments equip;
    Item item("Test", {});
    equip.Equip(Equipment(item));
    EXPECT_TRUE(equip.Unequip(*equip.GetEquipped().front()));
    EXPECT_TRUE(equip.GetEquipped().empty());
}

TEST(Equipment, unequip_by_tag)
{
    Equipments equip;
    Item item("Test", {Restriction::metal});
    equip.Equip(Equipment(item));
    EXPECT_EQ(equip.Unequip({Restriction::wood}), 0);
    EXPECT_EQ(equip.Unequip({Restriction::metal}), 1);
    EXPECT_TRUE(equip.GetEquipped().empty());
}

TEST(Equipment, stats)
{
    Definition def(Item::definition);
    def.Define(Stat::range);
    Equipments equip;
    Item item("Test", {Restriction::metal});
    item.Set(Stat::range, 5);
    equip.Equip(Equipment(item));
    EXPECT_FALSE(bool(equip.Get(Stat::reach)));
    EXPECT_EQ(equip.Get(Stat::range).Total(), 5);
    EXPECT_EQ(equip.Get(Stat::range, nullptr, {Restriction::cloth}).Total(), 0);
    EXPECT_EQ(equip.Get(Stat::range, nullptr, {Restriction::metal}).Total(), 5);
}

TEST(Equipment, stat_contribution)
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

    EXPECT_EQ(equip.Get(Stat::weight).Total(), 5);
    EXPECT_EQ(equip.Get(Stat::weight, nullptr, {Restriction::metal}).Total(), 3);
    EXPECT_EQ(equip.Get(Stat::weight, nullptr, {Restriction::leather}).Total(), 2);
}

TEST(Equipment, weapon_material)
{
    Definition def(Item::definition);
    def.Define(Stat::offense);

    Equipments equip;
    Item item("test", {});
    ItemBonus material(json::parse(    R"""({
                "prefix": "unit",
                "offense": 2
        })"""));
    const auto& equipment = equip.Equip(Equipment(item, {&material}));
    EXPECT_EQ(equip.Get(Stat::offense).Total(), 2);
    EXPECT_EQ(equipment.Name(), "unit test");
}

TEST(Equipment, weapon_bonus)
{
    Definition def(Item::definition);
    def.Define(Stat::offense);

    Equipments equip;
    Item item("test", {});
    ItemBonus bonus(json::parse(    R"""({
                "postfix": "of testability",
                "endurance": 2
        })"""));
    const auto& equipment = equip.Equip(Equipment(item, {&bonus}));
    EXPECT_EQ(equip.Get(Stat::endurance).Total(), 2);
    EXPECT_EQ(equipment.Name(), "test of testability");
}

TEST(Equipment, total)
{
    Definition def(Item::definition);
    def.Define(Stat::weight);

    Item armor("armor", Restrictions({Restriction::armor}), Stat::none, {{Stat::weight, 2}});
    Item weapon("weapon", Restrictions({Restriction::melee}), Stat::damage, {{Stat::weight, 1}});
    Equipments equip;
    equip.Equip(Equipment(armor));
    equip.Equip(Equipment(weapon));
    auto total = equip.Get(Stat::weight);
    EXPECT_EQ(total.Total(), 3);
    EXPECT_EQ(total.Description(), "2[armor] + 1[weapon]");
    EXPECT_EQ(equip.Get(Stat::weight, nullptr, Restrictions({Restriction::armor})).Total(), 2);
}

class MockCreature :
    public MockStatted,
    public MockBoni,
    public MockEquipped
{
};

TEST(Equipment, CanEquipNaked)
{
    Definition def(Item::definition);
    def.Define(Stat::weight);
    def.Define(Stat::hold);
    def.Define(Stat::enchantment);
    MockCreature creature;
    EXPECT_CALL(creature, Get(Stat::hold, &creature, Restrictions())).WillOnce(Return(Computation(0)));
    EXPECT_CALL(creature, Get(Stat::weight, &creature, Restrictions::weapon)).WillOnce(Return(Computation(0)));
    EXPECT_CALL(creature, Get(Stat::enchantment, &creature, Restrictions())).WillOnce(Return(Computation(0)));

    EXPECT_CALL(creature, Get(Stat::hands, nullptr, _)).WillOnce(Return(Computation(2)));
    EXPECT_CALL(creature, Get(Stat::lift, nullptr, _)).WillOnce(Return(Computation(2)));
    EXPECT_CALL(creature, Get(Stat::attune, nullptr, _)).WillOnce(Return(Computation(0)));

    EXPECT_CALL(creature, Bonus(_)).WillRepeatedly(Return(Computation()));
    Item weapon("weapon", Restrictions({Restriction::melee}), Stat::damage, {{Stat::weight, 1}, {Stat::hold, 2}});
    Equipments equip;
    EXPECT_TRUE(Equipment(weapon).CanEquip(creature));
}

TEST(Equipment, CanNotEquipHandsFull)
{
    Definition def(Item::definition);
    def.Define(Stat::weight);
    def.Define(Stat::hold);
    def.Define(Stat::enchantment);
    MockCreature creature;
    EXPECT_CALL(creature, Get(Stat::hold, &creature, Restrictions())).WillOnce(Return(Computation(2)));
    EXPECT_CALL(creature, Get(Stat::weight, &creature, Restrictions::weapon)).WillOnce(Return(Computation(1)));
    EXPECT_CALL(creature, Get(Stat::enchantment, &creature, Restrictions())).WillOnce(Return(Computation(0)));

    EXPECT_CALL(creature, Get(Stat::hands, nullptr, _)).WillOnce(Return(Computation(2)));
    EXPECT_CALL(creature, Get(Stat::lift, nullptr, _)).WillOnce(Return(Computation(2)));
    EXPECT_CALL(creature, Get(Stat::attune, nullptr, _)).WillOnce(Return(Computation(0)));

    EXPECT_CALL(creature, Bonus(_)).WillRepeatedly(Return(Computation()));
    Item weapon("weapon", Restrictions({Restriction::melee}), Stat::damage, {{Stat::weight, 1}, {Stat::hold, 1}});
    Equipments equip;
    EXPECT_FALSE(Equipment(weapon).CanEquip(creature));
}

TEST(Equipment, CanNotEquipHeavyArmor)
{
    Definition def(Item::definition);
    def.Define(Stat::weight);
    def.Define(Stat::enchantment);
    MockCreature creature;
    EXPECT_CALL(creature, Get(Stat::hold, &creature, Restrictions())).WillOnce(Return(Computation(0)));
    EXPECT_CALL(creature, Get(Stat::weight, &creature, Restrictions({Restriction::armor}))).WillOnce(Return(Computation(0)));
    EXPECT_CALL(creature, Get(Stat::enchantment, &creature, Restrictions())).WillOnce(Return(Computation(0)));

    EXPECT_CALL(creature, Get(Stat::hands, nullptr, _)).WillOnce(Return(Computation(0)));
    EXPECT_CALL(creature, Get(Stat::endurance, nullptr, _)).WillOnce(Return(Computation(1)));
    EXPECT_CALL(creature, Get(Stat::attune, nullptr, _)).WillOnce(Return(Computation(0)));

    EXPECT_CALL(creature, Bonus(_)).WillRepeatedly(Return(Computation()));
    Item armor("armor", Restrictions({Restriction::armor}), Stat::none, {{Stat::weight, 2}});
    Equipments equip;
    EXPECT_FALSE(Equipment(armor).CanEquip(creature));
}

TEST(Equipment, serialize)
{
    Equipments equip;
    ItemDatabase db;
    auto& item = db.Add(Item("Test", {}));
    equip.Equip(Equipment(item));
    auto serialized = equip.Serialize();
    Equipments deserialized(db, serialized);
    EXPECT_EQ(deserialized.GetEquipped().front()->GetItem(), item);
}

}
