#include "Game/PlanFactory.h"
#include "Utility/Variant.h"

namespace Game 
{

   
void DefaultPlanFactory::Activate()
{
    Engine::Application::Get().bus.Subscribe(*this, {Engine::MessageType<Engine::ClickOn>()});
}

void DefaultPlanFactory::Deactivate()
{
    Engine::Application::Get().bus.Unsubscribe(*this, {Engine::MessageType<Engine::ClickOn>()});  
    target = std::monostate{};
}

Plan DefaultPlanFactory::operator()(World& world, Actor& actor) const
{
    return std::visit(overloaded_visit
        {
            [&world, &actor](Position destination) -> Plan
            {
                return Plan::Move(world, actor, destination);
            },
            [&world, &actor](Actor* target) -> Plan 
            {
                return Plan::Attack(world, actor, *target);
            },
            [](std::monostate) -> Plan
            {
                throw std::runtime_error("Can't make a default plan without a target");
            }            
        }, target);
}

void DefaultPlanFactory::OnMessage(const Engine::Message& message)
{
    if (auto clickOn = message.Cast<Engine::ClickOn>())
    {    
        if (auto actor = dynamic_cast<const Actor*>(clickOn->object))
        {
            target = const_cast<Actor*>(actor);
            Engine::Application::Get().bus.Post(Complete(*this));
        }
        if (const auto* map = dynamic_cast<const Map*>(clickOn->object))
        {
            Position destination = map->IdToPosition(clickOn->sub);
            target = destination;
            Engine::Application::Get().bus.Post(Complete(*this));      
        }    
    }
}

}