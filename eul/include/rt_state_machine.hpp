#pragma once 

#include "event.hpp"
#include "intrusive_list.hpp"
#include <initializer_list>
#include <functional>
#include <expected>

// Run time state machine: 
// 1) states constructed at initialization (can be static) 
// 2) Run time generation 

namespace eul
{

class StateIF
{
public:
    virtual ~StateIF() = default;
    virtual void Entry();
    virtual void Exit();
    virtual void OnEvent(const Event& event);
};

class State;

class Transition : public LinkedList::Node
{
public:
    Transition(const Event& event, State& to);
    Transition(State& from, const Event& event, State& to);
private:
    friend State;
    Event event;
    State& to;
};

class State : public StateIF, public LinkedList::Node
{
public:
    State();
    State(std::initializer_list<std::reference_wrapper<State>> children);
    bool InState(const State& child) const;
protected:
    friend class Machine;
    State* FindTransition(const Event& event) const;
    std::expected<State*, errno_t> ChangeState(State& state);
    void Enter();
    void ConfirmeSubStateChange();
    void Leave();
    State* DefaultState();
    void SignalEvent(const Event& event);
private:
    LinkedList children;
    State* state = nullptr;

    friend Transition;
    void Transit(Transition& transition);
    LinkedList transitions;
};

class Machine : public State
{
public:
    explicit Machine(std::initializer_list<std::reference_wrapper<State>> children);
    bool Signal(const Event& event);
};


}
