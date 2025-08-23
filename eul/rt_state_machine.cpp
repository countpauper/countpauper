#include "rt_state_machine.hpp"
#include "errno.h"

namespace eul 
{
    
expectation stateIF::entry()
{
    return as_expected;
}

expectation stateIF::exit()
{
    return as_expected;
}

expectation stateIF::on(const event& _event)
{
    return as_expected;
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

expectation state::leave()
{
    if (_state)
    {
        auto result = _state->leave();
        if (!result)
        {
            _state = nullptr;
            return result;
        }
    }
    _state = nullptr;
    return exit();
}

expectation state::confirm_substate_change() 
{
    if (!_state)
        _state = default_state();
    if (_state) 
    {
        auto enter_result = _state->enter();
        if (!enter_result)
        {
            _state = nullptr;
            return enter_result;
        }
        else
            return enter_result;
    }
    else
        return as_expected;
}

expectation state::enter()
{
    auto result = entry();
    if (!result)
        return result;
    return confirm_substate_change();
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
        auto change_result = child.change(to);
        if (change_result)
        {
            if (_state==*change_result)
            {
                auto entry_result = _state->confirm_substate_change();
                if (!entry_result)
                    return std::unexpected(entry_result.error());
                return nullptr;
            }
            else if (*change_result)
            {
                if (_state)
                {
                    auto exit_result = _state->exit();
                    if (!exit_result) 
                    {
                        _state = nullptr;
                        return std::unexpected(exit_result.error());
                    }
                }
                _state = *change_result;
                return this;
            }
        } else if (change_result.error()!=ENOENT)
            return std::unexpected(change_result.error());
    }
    return std::unexpected(ENOENT);
}

expectation state::signal(const event& _event)
{
    auto result = on(_event);
    if (!result)
        return result;
    if (_state)
        return _state->signal(_event);
    return result;
}


machine::machine(std::initializer_list<std::reference_wrapper<state>> children)
    : state(children)
{
    confirm_substate_change();
}

expectation machine::signal(const event& _event)
{    
    if (state* target = find_transition(_event))
    {
        auto destination = change(*target);
        if (destination)
        {
            if (*destination)
                return (*destination)->enter();
            return as_expected;
        } if (destination.error()!=ENOENT) 
            return std::unexpected(destination.error());
    }
    return state::signal(_event);
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