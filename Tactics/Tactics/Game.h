#pragma once

#include <memory>
#include <istream>
#include <list>

#include "Direction.h"
#include "Map.h"
#include "IGame.h"
#include "Item.h"

namespace Game
{
    class Object;
    struct Position;
    class Plan;
    class Actor;
    class State;
    class Skills;
    class Skill;

    class Game : public IGame
    {
    public:
        enum class Selection
        {
            None = 0,
            Map,
            Object
        };
        Game(std::unique_ptr<Skills> skills);
        virtual ~Game();

        //IGame
        State Get(const Actor& actor) const override;
        void Adjust(Actor& actor, const State& state) override;
        void Apply() const override;

        void Tick();
        void Render() const;
        void Key(unsigned short code);
        bool CanBe(const Position& position) const override;
        bool CanGo(const Position& from, Direction direction) const override;
        bool Cover(const Position& from, const Position& to) const override;
        void Click(Selection selection, GLuint value);
        std::vector<Actor*> FindTargets(const State& from, const Skill& skill) const;
        std::vector<Actor*> FindTargetsInRange(const State& from, const Skill& skill) const;
        std::unique_ptr<const Skills> skills;
        Actor* ActiveActor() const override;
        void MakePlan(Actor& actor, const Skill& skill);

        const Type::Armor& FindArmor(const std::wstring& name) const;
        const Type::Armor::Material&  FindArmorMaterial(const std::wstring& name, const Type::Armor& armor) const;
        const Type::Armor::Bonus& FindArmorBonus(const std::wstring& name, const Type::Armor& armor) const;
    protected:
        void AI(Actor* actor);
        void Next();
        std::wstring Description() const override;
        void TestDumpAllItems(std::wostream& out) const;


        friend std::wistream& operator>>(std::wistream& s, Game& game);
        Map map;
        typedef std::list<std::unique_ptr<Object>> Objects;
        Objects objects;
        Objects::iterator turn;
        std::unique_ptr<Plan> plan;

        std::vector<Type::Armor> armors;
        std::vector<Type::Armor::Material> armorMaterials;
        std::vector<Type::Armor::Bonus> armorBoni;
        std::vector<Type::Weapon> weapons;
        std::vector<Type::Weapon::Material> weaponMaterials;
        std::vector<Type::Weapon::Bonus> weaponBoni;
    };
    std::wistream& operator>>(std::wistream& s, Game& game);

}   // ::Game

