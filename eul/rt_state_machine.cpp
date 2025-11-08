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
        _state = nullptr;
        if (!result)
            return result;
    }
    return exit();
}

expectation state::confirm_substate_change() 
{
    if (!_state)
        _state = default_state();
    if (!_state) 
        return as_expected;
    auto enter_result = _state->enter();
    if (!enter_result)
        _state = nullptr;
    return enter_result;
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
    if (children.empty())
        return nullptr;
    return &static_cast<state&>(children.front());
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

std::expected<state*,errno_t> state::find_transition(const event& _event) const
{
    for(const auto& node : transitions)
    {
        const auto& trans = static_cast<const transition&>(node); 
        if (trans._event == _event) {
            auto result = trans.behave(*this, _event);
            if (result) {
                return &trans._to;
            } else if (result.error() != EACCES) {
                return std::unexpected(result.error());
            }
        }
    }
    if (_state)
        return _state->find_transition(_event);
    return std::unexpected(ECHILD);
}

std::expected<state*, errno_t> state::change(state& to)
{
    if (this == &to)
        return this;
    for(auto& node: children)
    {
        state& child = static_cast<state&>(node);
        if (auto change_result = child.change(to))
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
            else 
            {
                return nullptr;
            }
        } else if (change_result.error()!=ECHILD)
            return std::unexpected(change_result.error());
    }
    return std::unexpected(ECHILD);
}


machine::machine(std::initializer_list<std::reference_wrapper<state>> children)
    : state(children)
{
    confirm_substate_change();
}

expectation machine::signal(const event& _event)
{    
    auto target = find_transition(_event);
    if (!target) {
        return std::unexpected(target.error());
    }
    auto& state_target = **target;
    auto destination = change(state_target);
    if (destination)
    {
        if (*destination)
            return (*destination)->enter();
        return as_expected;
    }
    return std::unexpected(destination.error());
}

transition::transition(const event& _event, state& to, const transition::BEHAVIOUR& bhv) :
    _event(_event),
    _to(to),
    _behaviour(bhv)
{
}

transition::transition(state& from, const event& _event, state& to, const transition::BEHAVIOUR& bhv) :
    transition(_event, to, bhv)
{
    from.transit(*this);
}


transition::transition(state& internal, const event& _event, const transition::BEHAVIOUR& bhv) :
    transition(_event, internal, bhv)
{
    internal.transit(*this);
}


expectation transition::behave(const state& _state, const event& _event) const
{
    if (!_behaviour) {
        return as_expected;
    }
    return _behaviour(_state, _event);
}

}