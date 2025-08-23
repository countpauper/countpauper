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
    virtual expectation on(const event&);
};

class state;

class transition : public intrusive_list::node
{
public:
    transition(const event& _event, state& to);
    transition(state& from, const event& _event, state& to);
private:
    friend state;
    event _event;
    state& _to;
};

class state : public stateIF, public intrusive_list::node
{
public:
    state();
    state(std::initializer_list<std::reference_wrapper<state>> children);
    bool in(const state& child) const;
protected:
    friend class machine;
    state* find_transition(const event& _event) const;
    std::expected<state*, errno_t> change(state& state);
    expectation enter();
    expectation confirm_substate_change();
    expectation leave();
    state* default_state();
    expectation signal(const event& _event);
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
