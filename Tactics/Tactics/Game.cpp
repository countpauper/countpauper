#include "stdafx.h"
#include <gl/GL.h>
#include "Game.h"
#include "Map.h"
#include "Actor.h"
#include "Plan.h"
#include "Skills.h"
#include "Item.h"
#include <fstream>

namespace Game
{
    Game::Game() :
        skills("Data/Skills.xml"),
        selectedSkill(nullptr),
        selectedTarget(nullptr),
        armors(Type::Armor::Load(std::wifstream(L"Data/Armor.csv"))),
        armorMaterials(Type::Armor::Material::Load(std::wifstream(L"Data/ArmorMaterial.csv"))),
        armorBoni(Type::Armor::Bonus::Load(std::wifstream(L"Data/ArmorBonus.csv"))),
        weapons(Type::Weapon::Load(std::wifstream(L"Data/Weapon.csv"))),
        weaponMaterials(Type::Weapon::Material::Load(std::wifstream(L"Data/WeaponMaterial.csv"))),
        weaponBoni(Type::Weapon::Bonus::Load(std::wifstream(L"Data/WeaponBonus.csv"))),
        focus(0,0,0)
    {
        // TestDumpAllItems(std::wofstream(L"Items.csv"));
    }
    
    Game::~Game() = default;

    State Game::Get(const Actor& actor) const
    {
        return State(actor);
    }
    void Game::Adjust(const Actor& actor, const State& state)
    {
        Actor& nonConstActor = const_cast<Actor&>(actor);   // faster than looking up in objects
        nonConstActor.Apply(state);
    }
    void Game::Apply(IGame& root) const
    {
        assert(&root == this);
    }

    void Game::Tick()
    {
        Actor* actor;
        while((actor= ActiveActor()) &&
            (!actor->CanAct()))
            Next();
        if (actor->GetTeam() > 0)
            AI(actor);
    }

    void Game::AI(Actor* actor)
    {
        std::vector<std::unique_ptr<Plan>> plans;
        for (auto skill : actor->GetSkills())
        {
            if ((skill.skill->IsAttack()) &&
                (actor->IsPossible(*skill.skill)))
            {
                auto targets = FindTargets(*actor, *skill.skill);
                for (auto target : targets)
                {
                    auto plan = std::make_unique<AttackPlan>(*actor, *target, *this, *skill.skill);
                    if (plan->Valid())
                        plans.emplace_back(std::move(plan));
                }
            }
        }
        if (plans.size())
            plans.front()->Execute(*this);
        Next();
    }

    void Game::Start()
    {
        turn = objects.begin();
        if (turn == objects.end())
            return;
        Activate(**turn);
    }

    void Game::Activate(Object& object)
    {
        Actor* actor = dynamic_cast<Actor*>(&object);
        if (actor)
        {
            Focus(*actor);
            actorActivated(actor);
            SelectSkill(actor->DefaultAttack());
        }
        else
        {
            actorActivated(nullptr);
            SelectSkill(nullptr);
        }
        object.Turn();
    }

    void Game::Next()
    {
        if (turn == objects.end())
            return;
        turn++;
        if (turn == objects.end())
            turn = objects.begin();
        Activate(**turn);
    }

    void Game::Focus(Object& object)
    {
        focus = map.Coordinate(object.GetPosition());
    }
    Actor* Game::ActiveActor() const
    {
        if (turn == objects.end())
            return nullptr;
        return dynamic_cast<Actor*>(turn->get());
    }

    const Skill* Game::SelectedSkill() const
    {
        return selectedSkill;
    }

    std::vector<Actor*> Game::FindTargets(const State& from, const Skill& skill) const
    {
        std::vector<Actor*> result;
        for (auto& object : objects)
        {
            auto actor = dynamic_cast<Actor*>(object.get());
            if (!actor)
                continue;
            if (actor->GetTeam() == from.loyalty)
                continue;
            if (actor->Dead())
                continue;
            result.push_back(actor);
        }
        return result;
    }

    const Game::Objects& Game::GetObjects() const
    {
        return objects;
    }

    std::vector<Actor*> Game::FindTargetsInRange(const State& from, const Skill& skill) const
    {
        auto targets = FindTargets(from, skill);
        auto it = std::remove_if(targets.begin(), targets.end(), [from, skill](const Actor* actor)
        {
            return actor->GetPosition().Distance(from.position) > skill.range;
        });
        targets.erase(it, targets.end());
        return targets;
    }

    void Game::SelectSkill(const Skill* skill)
    {
        if (skill == selectedSkill)
            return;
        selectedSkill = skill;
        skillSelected(selectedSkill);
        SelectPlan();
    }

    void Game::SelectPlan()
    {
        if (selectedSkill && selectedTarget)
        {
            if (selectedSkill->IsAttack())
            {
                plan = std::make_unique<AttackPlan>(*ActiveActor(), *selectedTarget, *this, *selectedSkill);
            }
            else if (selectedSkill->IsMove())
            {
                plan = std::make_unique<PathPlan>(*ActiveActor(), selectedTarget->GetPosition(), *this);
            }
        }
        else
        {
            plan.reset();
        }
    }
    void Game::Render() const
    {
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();

        glPushName(GLuint(Selection::Map));
        map.Render();
        glPopName();
        glPushName(GLuint(Selection::Object));
        unsigned index = 0;
        for (const auto& object : objects)
        {
            glPushMatrix();
            auto position = object->GetPosition();
            auto square = map.At(position);

            glTranslatef(float(position.x)+0.5f, square.Z(), float(position.y)+0.5f);
            glPushName(object->Id());
            object->Render();
            glPopName();
            glPopMatrix();
        }
        glPopName();
        if (plan)
            plan->Render();
    }

    void Game::Key(unsigned short code)
    {
        if (code == VK_ESCAPE)
        {
            plan.reset();
            Focus(*ActiveActor());
            return;
        }
        auto& playerActor = *ActiveActor();
        if (code == VK_RETURN)
        {
            if (plan)
                plan->Execute(*this);
            Next();
            plan.reset();
            return;
        }
        if (Action::keymap.count(code) == 0)
            return;

        /* TODO: reimplemennt manual plan
        if (!plan)
            plan = std::make_unique<ManualPlan>(playerActor);
        std::unique_ptr<Action> action(Action::keymap[code](plan->Final(), *this));
        if (!action)
            return;
        plan->Add(*this, std::move(action));
        */
    }

    bool Game::CanBe(const Position& position) const
    {
        if (!map.CanBe(position))
            return false;
        for (const auto& object : objects)
            if ((object->GetPosition() == position) && (!object->Prone()))
                return false;
        return true;
    }

    bool Game::CanGo(const Position& from, Direction direction) const
    {
        auto to = from + direction.Vector();
        return map.CanGo(from, direction);
    }

    bool Game::Cover(const Position& from, const Position& to) const
    {
        Position delta = to - from;
        if (std::abs(delta.x) < std::abs(delta.y))
        {
            float x = float(from.x);
            float dx = float(delta.x) / float(delta.y);
            if (to.y > from.y)
            {
                for (int y = from.y; y < to.y; ++y)
                {
                    if (!CanGo(Position(std::lroundf(x), y), Direction(Direction::Value::South)))
                        return true;
                    x += dx;
                }
            }
            else
            {
                for (int y = from.y; y < to.y; --y)
                {
                    if (!CanGo(Position(std::lroundf(x), y), Direction(Direction::Value::North)))
                        return true;
                    x += dx;
                }
            }
        }
        else
        {
            float y = float(from.y);
            float dy = float(delta.y) / float(delta.x);
            if (to.x > from.x)
            {
                for (int x = from.x; x < to.x; ++x)
                {
                    if (!CanGo(Position(x, std::lroundf(y)), Direction(Direction::Value::East)))
                        return true;
                    y += dy;
                }
            }
            else
            {
                for (int x = from.x; x < to.x; --x)
                {
                    if (!CanGo(Position(x, std::lroundf(y)), Direction(Direction::Value::West)))
                        return true;
                    y += dy;
                }
            }
        }
        return false;
    }

    std::wstring Game::Description() const
    {
        return L"";
    }
    std::wistream& operator>>(std::wistream& s, Game& game)
    {
        const int index = 1;
        s.pword(1) = &game;

        unsigned objects;
        s >> objects;
        for (unsigned oi = 0; oi < objects; ++oi)
        {
            std::wstring type;
            s >> type;
            if (type == L"Actor")
            {
                auto actor = std::make_unique < Actor>();
                s >> *actor;
                game.objects.emplace_back(std::move(actor));

            }
        }
        game.turn = game.objects.begin();

        s >> game.map;
        game.Focus(*game.ActiveActor());
        return s;
    }

    void Game::Click(Selection selection, GLuint value)
    {
        auto& playerActor = *ActiveActor();
        if (selection == Selection::Map)
        {
            Position target(value & 0xFFFF, (value >> 16) & 0xFFFF);
            plan.reset(new PathPlan(playerActor, target,*this));
        }
        else if (selection == Selection::Object)
        {
            auto object = (Object*)value;
            selectedTarget = dynamic_cast<Actor*>(object);
            SelectPlan();
        }
        else if (selection == Selection::Skill)
        {
            SelectSkill((Skill*)value);
        }
    }

    const Type::Armor& Game::FindArmor(const std::wstring& name) const
    {
        for (const auto& armor : armors)
        {
            if (armor.name == name)
                return armor;
        }
        throw std::invalid_argument("Unknown armor type");
    }

    const Type::Armor::Material&  Game::FindArmorMaterial(const std::wstring& name, const Type::Armor& armor) const
    {
        for (const auto& material : armorMaterials)
        {
            if (armor.Match(material) && material.name == name)
                return material;
        }
        throw std::invalid_argument("Unknown material type");
    }

    const Type::Armor::Bonus& Game::FindArmorBonus(const std::wstring& name, const Type::Armor& armor) const
    {
        for (const auto& bonus : armorBoni)
        {
            if (armor.Match(bonus) && bonus.prefix == name)
                return bonus;
        }
        throw std::invalid_argument("Unknown bonus type");
    }

    const Type::Weapon& Game::FindWeapon(const std::wstring& name) const
    {
        for (const auto& weapon : weapons)
        {
            if (weapon.name == name)
                return weapon;
        }
        throw std::invalid_argument("Unknown Weapon type");
    }

    const Type::Weapon::Material&  Game::FindWeaponMaterial(const std::wstring& name, const Type::Weapon& weapon) const
    {
        for (const auto& material : weaponMaterials)
        {
            if (weapon.Match(material) && material.name == name)
                return material;
        }
        throw std::invalid_argument("Unknown material type");
    }

    const Type::Weapon::Bonus& Game::FindWeaponBonus(const std::wstring& name, const Type::Weapon& weapon) const
    {
        for (const auto& bonus : weaponBoni)
        {
            if (weapon.Match(bonus) && bonus.prefix == name)
                return bonus;
        }
        throw std::invalid_argument("Unknown bonus type");
    }

 
    void Game::TestDumpAllItems(std::wostream& out) const
    {
        out << L"Name, ReqStr, ReqWis, Sharp, Crush, Burn, Disease, Spirit" << std::endl;
        for (auto& armor : armors)
        {
            for (auto& material : armorMaterials)
            {
                if (armor.category != material.category)
                    continue;
                for (auto& bonus : armorBoni)
                {
                    if (armor.category != bonus.category)
                        continue;
                    Armor item(armor, material, bonus);
                    Damage mitigation(item.Mitigation());
                    Requirement req(item.Required());
                    out << item.Name();
                    out << L"," << req.strength << L"," << req.wisdom;
                    out << L"," << mitigation;
                    out << std::endl;
                }
            }
        }
    }


}   // ::Game


