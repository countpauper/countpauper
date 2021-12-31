#include "stdafx.h"
#include <gl/GL.h>
#include "Skills.h"
#include "Game.h"
#include "VoxelMap.h"
#include "Actor.h"
#include "Plan.h"
#include "Item.h"
#include "Engine/Debug.h"
#include <fstream>

namespace Game
{
    Game::Game() :
        skills("Data/Skills.xml"),
        selectedActor(nullptr),
        selectedSkill(nullptr),
        selectedTarget(nullptr),
        armors(Type::Armor::Load(std::wifstream(L"Data/Armor.csv"))),
        armorMaterials(Type::Armor::Material::Load(std::wifstream(L"Data/ArmorMaterial.csv"))),
        armorModifiers(Type::Armor::Modifier::Load(std::wifstream(L"Data/ArmorBonus.csv"))),
        weapons(Type::Weapon::Load(std::wifstream(L"Data/Weapon.csv"))),
        weaponMaterials(Type::Weapon::Material::Load(std::wifstream(L"Data/WeaponMaterial.csv"))),
        weaponModifiers(Type::Weapon::Modifier::Load(std::wifstream(L"Data/WeaponBonus.csv"))),
        focus(0,0,0)
    {
        // TestDumpAllItems(std::wofstream(L"Items.csv"));
    }
    
    Game::~Game() = default;

    std::vector<Engine::RGBA> Game::teamColor =
    {
        Engine::RGBA(100, 255, 100, 255),
        Engine::RGBA(255, 100, 100, 255),
        Engine::RGBA(100, 100, 255, 255),
        Engine::RGBA(200, 200, 100, 255)
    };


    State Game::Get(const Identity& actor) const
    {
        return State(dynamic_cast<const Actor&>(actor));
    }
	void Game::Adjust(const Identity& actor, const State& state, const std::wstring& description)
    {
        Identity& nonConstId = const_cast<Identity&>(actor);   // faster than reverse look up in objects
        nonConstId.Apply(state);
        Engine::Debug::Log(actor.Description() + L":" + description + L"\n");
    }
    void Game::Apply(IGame& root) const
    {
        assert(&root == this);
    }

    void Game::Tick()
    {
        map->Tick(0.1);
        if (!selectedActor)
            Next();
        else
        {
            AI(); 
        }
    }

    void Game::AI()
    {
        if (selectedActor->GetTeam() > 0)
        {
            for (auto activeActor : ActiveActors())
            {
                if (activeActor->IsAlly(*selectedActor))
                {
                    SelectActor(*activeActor);
                    activeActor->AI(*this);
                }
            }
            Execute();
            Next();
        }
    }

    void Game::Start()
    {
        Activate();
    }

    Game::ActorList Game::NextGroup() const
    {
        auto it = std::find_if(objects.begin(), objects.end(), [](const decltype(objects)::value_type& object)
        {
            auto actor = dynamic_cast<Actor*>(object.get());
            return ((actor) && (!actor->IsAnticipating()));
        });
        if (it == objects.end())
            return ActorList();
        auto first = dynamic_cast<Actor*>(it->get());
        ActorList group;

        for (it; it != objects.end(); ++it)
        {
            auto actor = dynamic_cast<Actor*>(it->get());
            if (actor && actor->IsAlly(*first))
            {
                group.emplace_back(actor);
            }
            else
                break;
        }
        return group;
    }

    void Game::Trigger(const ActorList& triggers)
    {
        assert(selectedActor == nullptr);   // should be open to trigger
        ActorList triggered;
        ActorList group;
        for (auto& object : objects)
        {
            auto actor = dynamic_cast<Actor*>(object.get());
            if ((actor) && 
                ((group.empty()) || (actor->IsAlly(*group.front()))))
            {
                for (auto trigger : triggers)
                {
                    if (actor->Trigger(*trigger, *this))
                    {
                        if (actor->IsActive())
                            triggered.emplace_back(actor);
                    }
                }
            }
        }
    }

    void Game::Activate()
    {
        ActorList group = NextGroup();
        if (group.empty())
            return;
        Turn();
        Activate(group);
        Trigger(group);
    }

    void Game::Turn()
    {
        for (auto& object : objects)
        {
            object->Turn();
        }
    }
    void Game::Activate(const Game::ActorList& group)
    {
        ActorList active;
        ActorList skipped;
        for (auto actor : group)
        {
            actor->Activate(*this);
            if (actor->IsActive())
                active.emplace_back(actor);
            else
                skipped.emplace_back(actor);
        }
        for (auto skip : skipped)
        {
            objects.emplace_back(Extract(*skip));
        }
        if (!active.empty())
        {
            actorsActivated(active);
        }
        else
        {
            assert(objects.front().get() != selectedActor); // skipped should have been moved
            Activate();
        }
    }

    std::unique_ptr<Object> Game::Extract(const Object& object)
    {
        auto objI= std::find_if(objects.begin(), objects.end(), [&object](const decltype(objects)::value_type& obj)
        {
            return obj.get() == &object;
        });
        if (objI == objects.end())
            return nullptr;
        auto result = std::move(*objI);
        objects.erase(objI);
        return result;
    }
    void Game::Next()
    {
        auto& active = ActiveActors();
        if (active.empty())
            Activate();
        else
            SelectActor(*active.front());
    }

    void Game::SelectActor(const Actor& actor)
    {
        auto ptr = Extract(actor);
        auto it = std::find_if(objects.begin(), objects.end(), [](const decltype(objects)::value_type& object)
        {
            const auto a = dynamic_cast<const Actor*>(object.get());
            return (a) && (!a->IsAnticipating());
        });
        objects.insert(it, std::move(ptr));
        Focus(actor);
        SetSelectedActor(&actor);
        selectedTarget = nullptr;
        selectedSkill = actor.DefaultAttack();
        skillSelected(selectedSkill);
    }

    Game::ActorList Game::ActiveActors()
    {
        ActorList result;
        for (auto& object : objects)
        {
            if (auto actor = dynamic_cast<Actor*>(object.get()))
                if (actor->IsActive())
                    result.emplace_back(actor);
        }
        return result;
    }

    void Game::Focus(const Object& object)
    {
        focus = map->Coordinate(object.GetPosition());
    }
    const Actor* Game::SelectedActor() const
    {
        return selectedActor;
    }

    const Identity* Game::Executor() const
    {
        return selectedActor;
    }

    const Skill* Game::SelectedSkill() const
    {
        return selectedSkill;
    }

    void Game::SelectTarget(const Target* target)
    {
        selectedTarget = target;
        const auto actor = dynamic_cast<const Actor*>(target);
        if ((actor) && (actor->IsActive()))
        {
            SelectActor(*actor);
        }
        else
        {
            SelectPlan();
        }
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
        std::unique_ptr<Plan> plan;
        if ((selectedActor) && (selectedSkill))
        {
            if (selectedTarget)
            {
                if (selectedSkill->IsAttack())
                {
                    plan = std::make_unique<AttackPlan>(*selectedActor, *selectedTarget, *this, *selectedSkill);
                }
                else if (selectedSkill->IsMove())
                {
                    plan = std::make_unique<PathPlan>(*selectedActor, selectedTarget->GetPosition(), *this);
                }
                else if (selectedSkill->IsWait())
                {
                    plan = std::make_unique<WaitPlan>(*selectedActor, *selectedTarget, *this);
                }
            }
            else
            {
                if (selectedSkill->IsWait())
                {
                    plan = std::make_unique<SkipPlan>(*selectedActor, *this);
                }
            }
        }
        if (plan)
        {
            if (plan->Valid())
            {
                Engine::Debug::Log(L"Plan: " + plan->Description() + L" \r\n");
                selectedActor->plan = std::move(plan);
                for (auto actor : ActiveActors())
                {
                    if (actor->IsIdle())
                    {
                        SelectActor(*actor);
                        break;
                    }
                }
            }
            else
            {
                Engine::Debug::Log(L"Plan: " + plan->Description() + L" Failed\r\n");
            }
        }
    }
    void Game::Render() const
    {
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
        glPushName(GLuint(Selection::Map));
        map->Render();
        glPopName();
        glPushName(GLuint(Selection::Object));
        unsigned index = 0;
        for (const auto& object : objects)
        {
            glPushName(object->Id());
            object->Render();
            glPopName();
        }
    }

    void Game::Execute()
    {
        for (auto actor : ActiveActors())
        {
            selectedActor = actor;
            actor->Execute(*this);
            if ((!actor->IsActive()) && (!actor->IsAnticipating()))
            {
                auto ptr = Extract(*objects.front());
                objects.emplace_back(std::move(ptr));
            }
        }
        SetSelectedActor(nullptr);
    }

    void Game::SetSelectedActor(const Actor* selected)
    {
        selectedActor = const_cast<Actor*>(selected);   // faster than looking up in objects
        actorSelected(selectedActor);
    }


    void Game::Key(unsigned code)
    {
        if (code == VK_RETURN)
        {
            Execute();
            Next();
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
        if (!map->CanBe(position))
            return false;
        for (const auto& object : objects)
            if ((object->GetPosition() == position) && (!object->Prone()))
                return false;
        return true;
    }

    bool Game::CanGo(const Position& from, Physics::Direction direction) const
    {
        auto to = from + direction.Vector();
        return map->CanGo(from, direction);
    }

    bool Game::Cover(const Position& from, const Position& to) const
    {
        Position delta = to - from;
        float z = float(from.z);
        float dz = float(delta.z) / float(delta.y);

        if (std::abs(delta.x) < std::abs(delta.y))
        {
            assert(std::abs(delta.z) <= std::abs(delta.y));  // not checking up or down yet
            float x = float(from.x);
            float dx = float(delta.x) / float(delta.y);
            if (to.y > from.y)
            {
                for (int y = from.y; y < to.y; ++y)
                {
                    if (!CanGo(Position(std::lroundf(x), y, std::lroundf(z)), Physics::Direction(0,1, 0)))
                        return true;
                    x += dx;
                    z += dz;
                }
            }
            else
            {
                for (int y = from.y; y < to.y; --y)
                {
                    if (!CanGo(Position(std::lroundf(x), y, std::lroundf(z)), Physics::Direction(0,-1, 0)))
                        return true;
                    x += dx;
                    z += dz;
                }
            }
        }
        else
        {
            assert(std::abs(delta.z) <= std::abs(delta.x));  // not checking up or down yet
            float y = float(from.y);
            float dy = float(delta.y) / float(delta.x);
            if (to.x > from.x)
            {
                for (int x = from.x; x < to.x; ++x)
                {
                    if (!CanGo(Position(x, std::lroundf(y), std::lroundf(z)), Physics::Direction(1, 0, 0)))
                        return true;
                    y += dy;
                    z += dz;
                }
            }
            else
            {
                for (int x = from.x; x < to.x; --x)
                {
                    if (!CanGo(Position(x, std::lroundf(y), std::lroundf(z)), Physics::Direction(-1, 0, 0)))
                        return true;
                    y += dy;
                    z += dz;
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
        std::wstring mapType;
        s >> mapType;
        if (mapType == L"2D")
        {
            auto map = std::make_unique<FlatMap>();
            s >> *map;
            game.map = std::move(map);
        }
        else if (mapType == L"3D")
        {
            auto map = std::make_unique <VoxelMap>();
            s >> *map;
            game.map = std::move(map);
        }
        return s;
    }

    void Game::Click(Selection selection, GLuint value)
    {
        if (selection == Selection::Map)
        {
            if (selectedActor)
            {
                Position target = Map::NamedLocation(value);
                selectedActor->plan = std::make_unique<PathPlan>(*selectedActor, target, *this);
            }
        }
        else if (selection == Selection::Object)
        {
            auto object = dynamic_cast<Object*>(Object::FindId(value));
            SelectTarget(object);
        }
        else if (selection == Selection::Skill)
        {
            SelectSkill(dynamic_cast<Skill*>(Skill::FindId(value)));
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

    const Type::Armor::Modifier& Game::FindArmorModifier(const std::wstring& name, const Type::Armor& armor) const
    {
        for (const auto& mod : armorModifiers)
        {
            if (armor.Match(mod) && mod.prefix == name)
                return mod;
        }
        throw std::invalid_argument("Unknown modifier type");
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

    const Type::Weapon::Modifier& Game::FindWeaponModifier(const std::wstring& name, const Type::Weapon& weapon) const
    {
        for (const auto& mod : weaponModifiers)
        {
            if (weapon.Match(mod) && mod.prefix == name)
                return mod;
        }
        throw std::invalid_argument("Unknown modifier type");
    }

 
    void Game::TestDumpAllItems(std::wostream& out) const
    {
        out << L"Name, Weight, Enchantment, Sharp, Crush, Burn, Disease, Spirit" << std::endl;
        for (auto& armor : armors)
        {
            for (auto& material : armorMaterials)
            {
                if (armor.category != material.category)
                    continue;
                for (auto& mod : armorModifiers)
                {
                    if (armor.category != mod.category)
                        continue;
                    Armor item(armor, material, mod);
                    Damage mitigation(item.Mitigation());
                    Load req(item.GetLoad());
                    out << item.Name();
                    out << L"," << req.weight<< L"," << req.enchantment;
                    out << L"," << mitigation;
                    out << std::endl;
                }
            }
        }
    }


}   // ::Game


