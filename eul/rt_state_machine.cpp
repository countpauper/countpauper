#include "rt_state_machine.hpp"
#include "errno.h"

namespace eul 
{
    
void StateIF::Entry()
{
}

void StateIF::Exit()
{
}

void StateIF::OnEvent(const Event& event)
{
}

State::State()
{
}

State::State(std::initializer_list<std::reference_wrapper<State>> children_)
{
    for(auto it = std::rbegin(children_); it!=std::rend(children_); ++it)
    {
        children.push_front(it->get());
    }
}

void State::Leave()
{
    if (state)
        state->Leave();
    state = nullptr;
    Exit();
}

void State::ConfirmeSubStateChange() 
{
    if (!state)
        state = DefaultState();
    if (state)
        state->Enter();
}

void State::Enter()
{
    Entry();
    ConfirmeSubStateChange();
}

State* State::DefaultState()
{
    if (!children.empty())
    {
        return &static_cast<State&>(children.front());
    }
    return nullptr;
}

bool State::InState(const State& query) const
{
    return this == &query || 
        (state && state->InState(query));
}

void State::Transit(Transition& transition)
{
    transitions.push_front(transition);
}

State* State::FindTransition(const Event& event) const
{
    for(const auto& node : transitions)
    {
        const auto& transition = static_cast<const Transition&>(node); 
        if (transition.event == event)
            return &transition.to;
    }
    if (state)
        return state->FindTransition(event);
    return nullptr;
}

std::expected<State*, errno_t> State::ChangeState(State& to)
{
    if (this == &to)
    {
        return this;
    }
    for(auto& node: children)
    {
        State& child = static_cast<State&>(node);
        if (auto result = child.ChangeState(to))
        {
            if (state==*result)
            {
                state->ConfirmeSubStateChange();
                return nullptr;
            }
            else if (*result)
            {
                if (state)
                    state->Exit();
                state = *result;
                return this;
            }
        } 
    }
    return std::unexpected(ENOENT);
}

void State::SignalEvent(const Event& event)
{
    OnEvent(event);
    if (state)
        state->SignalEvent(event);
}


Machine::Machine(std::initializer_list<std::reference_wrapper<State>> children)
    : State(children)
{
    if (children.size()>0) 
        (*ChangeState(*DefaultState()))->Enter();
}

bool Machine::Signal(const Event& event)
{    
    if (State* target = FindTransition(event))
    {
        auto result = ChangeState(*target);
        if (result)
        {
            if (*result)
                (*result)->Enter();
            return true;
        }
    }
    SignalEvent(event);
    return false;
}

Transition::Transition(const Event& event, State& to) :
    event(event),
    to(to)
{
}

Transition::Transition(State& from, const Event& event, State& to) :
    Transition(event, to)
{
    from.Transit(*this);
}

}