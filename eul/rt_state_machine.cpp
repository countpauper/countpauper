#include "rt_state_machine.hpp"
#include "errno.h"

namespace eul 
{
    
void stateIF::entry()
{
}

void stateIF::exit()
{
}

void stateIF::on(const event& _event)
{
}

state::state()
{
}

state::state(std::initializer_list<std::reference_wrapper<state>> children_)
{
    for(auto it = std::rbegin(children_); it!=std::rend(children_); ++it)
    {
        children.push_front(it->get());
    }
}

void state::leave()
{
    if (_state)
        _state->leave();
    _state = nullptr;
    exit();
}

void state::confirm_substate_change() 
{
    if (!_state)
        _state = default_state();
    if (_state)
        _state->enter();
}

void state::enter()
{
    entry();
    confirm_substate_change();
}

state* state::default_state()
{
    if (!children.empty())
    {
        return &static_cast<state&>(children.front());
    }
    return nullptr;
}

bool state::in(const state& query) const
{
    return this == &query || 
        (_state && _state->in(query));
}

void state::transit(transition& transition)
{
    transitions.push_front(transition);
}

state* state::find_transition(const event& _event) const
{
    for(const auto& node : transitions)
    {
        const auto& trans = static_cast<const transition&>(node); 
        if (trans._event == _event)
            return &trans._to;
    }
    if (_state)
        return _state->find_transition(_event);
    return nullptr;
}

std::expected<state*, errno_t> state::change(state& to)
{
    if (this == &to)
    {
        return this;
    }
    for(auto& node: children)
    {
        state& child = static_cast<state&>(node);
        if (auto result = child.change(to))
        {
            if (_state==*result)
            {
                _state->confirm_substate_change();
                return nullptr;
            }
            else if (*result)
            {
                if (_state)
                    _state->exit();
                _state = *result;
                return this;
            }
        } 
    }
    return std::unexpected(ENOENT);
}

void state::signal(const event& _event)
{
    on(_event);
    if (_state)
        _state->signal(_event);
}


machine::machine(std::initializer_list<std::reference_wrapper<state>> children)
    : state(children)
{
    if (children.size()>0) 
        (*change(*default_state()))->enter();
}

bool machine::signal(const event& _event)
{    
    if (state* target = find_transition(_event))
    {
        auto result = change(*target);
        if (result)
        {
            if (*result)
                (*result)->enter();
            return true;
        }
    }
    signal(_event);
    return false;
}

transition::transition(const event& _event, state& to) :
    _event(_event),
    _to(to)
{
}

transition::transition(state& from, const event& _event, state& to) :
    transition(_event, to)
{
    from.transit(*this);
}

}