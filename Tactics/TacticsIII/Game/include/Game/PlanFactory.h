#pragma once
#include "Game/Plan.h"
#include "Game/TargetQueue.h"
#include "Game/Map.h"
#include "UI/WindowMessages.h"
#include "UI/Application.h"
#include <tuple>

namespace Game
{


class Actor;
class World;


// TODO 
// or even better than that, fuse it into its Action class Attack { static PlanFactory<Attack> factory; } (circular dependency?)
// and maybe then fuse it further by using CRTP class Attack : public Fabricatable<Attack> { }  
// or even better than that, make a generic factory utility then that takes the constructor parameters.
// Also split the target queue into a generic modifiable tuple in engine with the queue interface on top. 


class PlanFactoryIF
{
public:

    struct Complete : public Engine::Message
    {
        Complete(PlanFactoryIF& factory) : factory(factory) {}
        PlanFactoryIF& factory;
    };

    virtual void Activate() = 0;
    virtual void Deactivate() = 0;
    virtual Plan operator()(World& world, Actor& actor) const = 0;

private:
};

class DefaultPlanFactory : 
    public PlanFactoryIF,
    public Engine::Passenger
{
public:
    void Activate() override;
    void Deactivate() override;
    Plan operator()(World& world, Actor& actor) const override;
protected:
    void OnMessage(const Engine::Message& message) override;
private: 
    std::variant<std::monostate, Position, Actor*> target;
};

template<class AT>
class PlanFactoryAction : 
    public PlanFactoryIF,
    public Engine::Passenger
{
    using Targets = typename ExtractTargets<typename AT::Targets>::type;
public:
    Plan operator()(World& world, Actor& actor) const override
    {
        return std::apply([](auto&&... args) {
                return Plan::Act<AT>(std::forward<decltype(args)>(args)...);
        }, std::tuple_cat(std::tie(world, actor), targets.as_tuple()));
    }

    void Activate() override
    {
        if (targets.IsComplete())
            Engine::Application::Get().bus.Post(Complete(*this));
        else 
        {
            Engine::Application::Get().bus.Subscribe(*this, {Engine::MessageType<Engine::ClickOn>()});
        }
    }
    void Deactivate()
    {
        Engine::Application::Get().bus.Unsubscribe(*this, {Engine::MessageType<Engine::ClickOn>()});        
        targets.clear();
    }
protected:
    void OnMessage(const Engine::Message& message) override 
    {
        if (auto clickOn = message.Cast<Engine::ClickOn>())
        {
            if (auto target = dynamic_cast<const Actor*>(clickOn->object))
            {
                if (targets.template ExpectNext<Actor*>())
                {
                    targets.push_back(target);
                }
                // else warning? Or if position, position of target?  
            }
            if (const auto* map = dynamic_cast<const Map*>(clickOn->object))
            {
                if (targets.template ExpectNext<Position>())
                {
                    Position destination = map->IdToPosition(clickOn->sub);
                    targets.push_back(destination);
                }
                // else warning? Or if target, target that is standing exactly at that position ?                 
            }
            if (targets.IsComplete())
                Engine::Application::Get().bus.Post(Complete(*this));            
        }
    }
private:
    Targets targets;
};

}
