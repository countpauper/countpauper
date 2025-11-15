#include "rt_state_machine.hpp"
#include "errno.h"

namespace eul 
{
    
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



state& state::entry(const behaviour& _entry)
{
    this->_entry = _entry;
    return *this;
}
state& state::exit(const behaviour& _exit)
{
    this->_exit = _exit;
    return *this;
}

expectation state::on_entry(const event& _event) const
{
    if (_entry)
        return _entry(*this, _event);
    return as_expected;
}

expectation state::on_exit(const event& _event) const
{
    if (_exit)
        return _exit(*this, _event);
    return as_expected;
}

expectation state::leave(const event& _event)
{
    if (_state)
    {
        auto result = _state->leave(_event);
        _state = nullptr;
        if (!result)
            return result;
    }
    return on_exit(_event);
}

expectation state::confirm_substate_change(const event& _event) 
{
    if (!_state)
        _state = default_state();
    if (!_state) 
        return as_expected;
    auto enter_result = _state->enter(_event);
    if (!enter_result)
        _state = nullptr;
    return enter_result;
}

expectation state::enter(const event& _event)
{
    auto result = on_entry(_event);
    if (!result)
        return result;

    return confirm_substate_change(_event);
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

std::expected<state*, errno_t> state::change(state& to, const event& _event)
{
    if (this == &to)
        return this;
    for(auto& node: children)
    {
        state& child = static_cast<state&>(node);
        if (auto change_result = child.change(to, _event))
        {
            if (_state==*change_result)
            {
                auto entry_result = _state->confirm_substate_change(_event);
                if (!entry_result)
                    return std::unexpected(entry_result.error());
                return nullptr;
            }
            else if (*change_result)
            {
                if (_state)
                {
                    auto exit_result = _state->on_exit(_event);
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
    confirm_substate_change(construction);
}

expectation machine::signal(const event& _event)
{    
    auto target = find_transition(_event);
    if (!target) {
        return std::unexpected(target.error());
    }
    auto state_target = *target;
    auto destination = change(*state_target, _event);
    if (destination)
    {
        if (*destination)
            return (*destination)->enter(_event);
        return as_expected;
    }
    return std::unexpected(destination.error());
}

state::transition::transition(const event& _event, state& to, const BEHAVIOUR& bhv) :
    _event(_event),
    _to(to),
    _behaviour(bhv)
{
}

state::transition::transition(state& from, const event& _event, state& to) :
    transition(_event, to, BEHAVIOUR())
{
    from.transit(*this);
}

state::transition::transition(state& from, const event& _event, state& to, const BEHAVIOUR& bhv) :
    transition(_event, to, bhv)
{
    from.transit(*this);
}


state::transition::transition(state& internal, const event& _event, const BEHAVIOUR& bhv) :
    transition(_event, internal, bhv)
{
    internal.transit(*this);
}


expectation state::transition::behave(const state& _state, const event& _event) const
{
    if (!_behaviour) {
        return as_expected;
    }
    return _behaviour(_state, _event);
}


event machine::construction;


}