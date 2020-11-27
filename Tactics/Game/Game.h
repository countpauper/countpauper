#pragma once

#include <memory>
#include <istream>
#include <list>
#include <connection.h>
#include <signal.h>
#include "Engine/Coordinate.h"
#include "Engine/Color.h"
#include "Physics/Direction.h"
#include "Skills.h"
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

    class Game : public IGame
    {
    public:
        enum class Selection
        {
            None = 0,
            Map,
            Object,
            Skill
        };
        Game();
        virtual ~Game();
        void Start();

        //IGame
        State Get(const Identity& actor) const override;
        void Adjust(const Identity& actor, const State& state, const std::wstring& description) override;
        void Apply(IGame& root) const override;
		const Identity* Executor() const override;

        void Tick();
        void Render() const;
        void Key(unsigned code);
        bool CanBe(const Position& position) const override;
        bool CanGo(const Position& from, Direction direction) const override;
        bool Cover(const Position& from, const Position& to) const override;
        void Click(Selection selection, unsigned value);
        std::vector<Actor*> FindTargets(const State& from, const Skill& skill) const;
        typedef std::list<std::unique_ptr<Object>> Objects;
        const Objects& GetObjects() const;
        static std::vector<Engine::RGBA> teamColor;

        const Actor* SelectedActor() const;
        is::signals::signal<void(const Actor*)> actorSelected;
        using ActorList = std::vector<Actor*>;
        is::signals::signal<void(const ActorList&)> actorsActivated;
        const Skill* SelectedSkill() const;
        void SelectSkill(const Skill* skill);
        is::signals::signal<void(const Skill*)> skillSelected;
        void SelectTarget(const Target* target);

        const Type::Armor& FindArmor(const std::wstring& name) const;
        const Type::Armor::Material&  FindArmorMaterial(const std::wstring& name, const Type::Armor& armor) const;
        const Type::Armor::Modifier& FindArmorModifier(const std::wstring& name, const Type::Armor& armor) const;
        const Type::Weapon& FindWeapon(const std::wstring& name) const;
        const Type::Weapon::Material&  FindWeaponMaterial(const std::wstring& name, const Type::Weapon& armor) const;
        const Type::Weapon::Modifier& FindWeaponModifier(const std::wstring& name, const Type::Weapon& armor) const;

        Engine::Coordinate focus;
        Skills skills;
    protected:
        void Next();
        void Turn();
        void Activate();
        void Activate(const Game::ActorList& group);
        void Trigger(const ActorList& triggers);
        void AI();
        void Execute();
        void SetSelectedActor(const Actor* selected);

        std::unique_ptr<Object> Extract(const Object& object);
        void SelectActor(const Actor& actor);
        ActorList ActiveActors();
        ActorList Game::NextGroup() const;
        void Focus(const Object& object);
        void SelectPlan();
        std::wstring Description() const override;
        void TestDumpAllItems(std::wostream& out) const;

        friend std::wistream& operator>>(std::wistream& s, Game& game);
        
        // State
        std::unique_ptr<Map> map;
        Objects objects;
        Actor* selectedActor;

        // Plan
        const Skill* selectedSkill;
        const Target* selectedTarget;

        // Definitiion
        std::vector<Type::Armor> armors;
        std::vector<Type::Armor::Material> armorMaterials;
        std::vector<Type::Armor::Modifier> armorModifiers;
        std::vector<Type::Weapon> weapons;
        std::vector<Type::Weapon::Material> weaponMaterials;
        std::vector<Type::Weapon::Modifier> weaponModifiers;
    };
    std::wistream& operator>>(std::wistream& s, Game& game);

}   // ::Game

