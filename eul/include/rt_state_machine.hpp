#pragma once 

#include "event.hpp"
#include "intrusive_list.hpp"
#include "expectation.hpp"
#include <initializer_list>
#include <functional>

// Run time state machine: 
// 1) states constructed at initialization (can be static) 
// 2) Run time generation 

namespace eul
{

class stateIF
{
public:
    virtual ~stateIF() = default;
    virtual expectation entry();
    virtual expectation exit();
};

class state;

class transition : public intrusive_list::node
{
public:
    using BEHAVIOUR = std::function<expectation(const class state&, const event& evt)>;
    transition(state& from, const event& _event, state& to, const BEHAVIOUR& bhv = BEHAVIOUR());
    transition(state& internal, const event& _event, const BEHAVIOUR& bhv);
protected:
    expectation behave(const state& _in, const event& _event) const;
private:
    transition(const event& _event, state& to, const BEHAVIOUR& bhv);
    friend state;
    event _event;
    state& _to;
    BEHAVIOUR _behaviour;
};

class state : public stateIF, public intrusive_list::node
{
public:
    state();
    state(std::initializer_list<std::reference_wrapper<state>> children);
    bool in(const state& child) const;
protected:
    friend class machine;
    std::expected<state*, errno_t> find_transition(const event& _event) const;
    std::expected<state*, errno_t> change(state& state);
    expectation enter();
    expectation confirm_substate_change();
    expectation leave();
    state* default_state();
private:
    intrusive_list children;
    state* _state = nullptr;

    friend transition;
    void transit(transition& transition);
    intrusive_list transitions;
};

class machine : public state
{
public:
    explicit machine(std::initializer_list<std::reference_wrapper<state>> children);
    expectation signal(const event& _event);
};


}
