#pragma once
#include "Game/Action.h"
#include "Game/TargetQueue.h"
#include "UI/Message.h"
#include "UI/Application.h"
#include <tuple>

namespace Game
{


class Actor;
class World;


// TODO 
// or even better than that, fuse it into its Action class Attack { static ActionFactory<Attack> factory; } (circular dependency?)
// and maybe then fuse it further by using CRTP class Attack : public Fabricatable<Attack> { }  
// or even better than that, make a generic factory utility then that takes the constructor parameters.


class ActionFactoryIF
{
public:

    struct Complete : public Engine::Message
    {
        Complete(ActionFactoryIF& factory) : factory(factory) {}
        ActionFactoryIF& factory;
    };

    virtual std::unique_ptr<Action> operator()(World& world, Actor& actor) const = 0;

    void Activate()
    {
        if (IsComplete())
            Engine::Application::Get().bus.Post(Complete(*this));
    }
private:
    virtual bool IsComplete() const = 0;
};

template<class AT>
class ActionFactory : 
    public ActionFactoryIF
{
    using Targets = typename ExtractTargets<typename AT::Targets>::type;
public:
    std::unique_ptr<Action> operator()(World& world, Actor& actor) const override
    {
        return std::apply([](auto&&... args) {
                return std::make_unique<AT>(std::forward<decltype(args)>(args)...);
        }, std::tuple_cat(std::tie(world, actor), targets.as_tuple()));
    }



    bool IsComplete() const override 
    {
        return targets.IsComplete();
    }
private:
    Targets targets;
};

}
