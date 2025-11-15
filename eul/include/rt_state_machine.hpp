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

class state;

class state : public intrusive_list::node
{
public:
    
    state();
    state(std::initializer_list<std::reference_wrapper<state>> children);
    bool in(const state& child) const;

    using behaviour = std::function<expectation(const class state&, const event& evt)>;
    state& entry(const behaviour& _entry);
    state& exit(const behaviour& _exit);
protected:
    friend class machine;
    std::expected<state*, errno_t> find_transition(const event& _event) const;
    std::expected<state*, errno_t> change(state& state, const event& _event);
    expectation enter(const event& _event);
    expectation confirm_substate_change(const event& _event);
    expectation leave(const event& _event);
    state* default_state();

    expectation on_entry(const event& _event) const;
    expectation on_exit(const event& _event) const;
public:

    // TODO can be any callable that matches the signature (expecation(const state&, const event&))
    // or void, use a concept for it
    // Then also use templated callback/signals for entry/exit      
    class transition : public intrusive_list::node
    {
    public:
        transition(state& from, const event& _event, state& to);
        transition(state& internal, const event& _event);
    protected:
        virtual expectation guard(const state& _in, const event& _event) const;
    private:
        transition(const event& _event, state& to);

        friend state;
        event _event;
        state& _to;
    };

//    template<std::invocable<const state&, const event&> GUARD>     // TODO GUARD must be invocable as a guard
    template<typename GUARD> 
    class guard_transition : public transition 
    {
    public:
        guard_transition(state& from, const event& _event, state& to, const GUARD& grd)
            : transition(from, _event, to)
        {
            _guard = grd;
        }
        guard_transition(state& internal, const event& _event, const GUARD& grd)
            : transition(internal, _event)
        {
            _guard = grd;
        }
    protected:
        expectation guard(const state& _in, const event& _event) const override 
        {
            return _guard(_in, _event);
        }
    private:
        GUARD _guard;
    };

    using guard_transition_fn = guard_transition<state::behaviour>;

private:
    intrusive_list children;
    state* _state = nullptr;
    behaviour _entry; 
    behaviour _exit;

    void transit(transition& transition);
    intrusive_list transitions;
};



class machine : public state
{
public:
    explicit machine(std::initializer_list<std::reference_wrapper<state>> children);
    expectation signal(const event& _event);

    static event construction;
};


}
