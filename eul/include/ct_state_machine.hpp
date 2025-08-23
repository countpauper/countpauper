#pragma once 

#include "event.hpp"
#include "expectation.hpp"
#include "variant.hpp"
#include <initializer_list>
#include <vector> // TODO linked list or something 
#include <cstdint> 
#include <type_traits>
#include <assert.h>


// Compile time state machine: 
// 1) RAII states constructed when activated. 
// 2) Compile time generation of state transition table
// Status: hierarchical transitions still needs to be generated properly (variable depth represented how)?
// And after that state transitions to non-default child states of parent states 

namespace eul
{


template<typename... C> class state; 
template<> class state<>; 

template<typename T>
concept is_leaf_state = std::is_base_of_v<state<>, T>;

// Branch (parent) state.
template<typename T>
concept is_branch_state = requires(T t) {
    { t.template in<state<>>() } -> std::convertible_to<bool>; 
//    { T::template find<state<>>() } -> std::convertible_to<std::size_t>; 
};

/* this works if T is the State<T...> itself, not classes derived from it, which is not even good enough for the visit
// Primary template: false for any type
template<typename T>
struct is_branch_state_instantiation : std::false_type {};

// Specialization for Wrapper with any args
template<typename First, typename... Args>
struct is_branch_state_instantiation<State<First, Args...>> : std::true_type {};

template<typename T>
inline constexpr bool is_branch_state = is_branch_state_instantiation<std::remove_cv_t<T>>::value;
*/

/* // This only works if the user adds  using is_branch_tag = void;  to all states
template<typename T, typename = void>
struct derives_from_branch_state : std::false_type {};

template<typename T>
struct derives_from_branch_state<T, std::void_t<typename T::is_branch_tag>> : std::true_type {};

// Concept form
template<typename T>
concept is_branch_state = derives_from_branch_state<T>::value;
*/

class stateIF
{
public:
    virtual expectation on(const event& _event);
};

class transitioning : public stateIF 
{
public:
    stateIF& get();
};

template<typename FindT, typename Variant> 
constexpr std::size_t find_child_transition();

static constexpr std::size_t nostate(-1);

template<typename FindT, typename Variant, std::size_t Idx=0>
constexpr std::size_t find_child_transition_impl() 
{
    if constexpr (Idx>=std::variant_size_v<Variant>)
        return nostate;
    using ChildState = std::variant_alternative_t<Idx, Variant>;
    if constexpr (is_branch_state<ChildState>)
    {
        constexpr std::size_t result = ChildState::template find<FindT>();
        if constexpr (result==nostate)   // TODO can this be constexpr? 
        {
            return find_child_transition_impl<FindT, Variant, Idx+1>();

        }
        else 
            return Idx + result * std::variant_size_v<Variant>;
    }
    else 
        return nostate;
}

template<typename FindT, typename Variant> 
constexpr std::size_t find_child_transition() 
{
    // TODO: can include variant_index_of in find_child_transition_impl instead 
    std::size_t idx = variant_index_of<FindT, Variant>();
    if (idx == static_cast<std::size_t>(-1))
    {   
        return find_child_transition_impl<FindT, Variant>();
    }
    else 
    {
        return idx;
    }

}

// TODO : require each C to be a concept state 
template<typename... C>
class state : public stateIF
{
public:
    state() = default;
   
    template<typename... FC>
    friend class state;

    template<typename QC>
    bool in() const 
    {
        return std::visit([](auto const& obj) -> bool 
        {
            using T = std::decay_t<decltype(obj)>;
            if constexpr (std::is_same_v<QC, T>) {
                return true;
            } else if constexpr (is_branch_state<T>) {
                return obj.template in<QC>();
            } else {
                return false;
            }
        }, child);
    }
    stateIF& get()
    {
        return std::visit([](auto& obj) -> stateIF&  
        {
            return obj.get();
        }, child);
    }    
protected:
    expectation on(const event& _event)
    {
        stateIF::on(_event);
        return std::visit([&_event](auto& obj) 
        {
           return obj.on(_event); 
        }, child);
    }

    using VariantType = std::variant<C..., transitioning>;

    template<typename FT, typename VT, std::size_t Idx>
    friend constexpr std::size_t find_child_transition_impl();

    template<typename CT>
    static constexpr std::size_t find()
    {
        return find_child_transition<CT, VariantType>();
    }


    VariantType child;
};

// Child Leaf state 
template<>
class state<> : public stateIF
{
    template<typename... FC>
    friend class state;
public:
    state() = default;
    stateIF& get()
    {
        return *this;  
    }    
};

template<typename... C>
class machine : public state<C...> 
{
public:
    using StateType = state<C...>;

    machine()
    {
    }
    struct TransStruct
    {
        event _event;
        std::size_t _from;
        std::size_t _to;
    };

    template<typename From, typename To>
    void transition(const event& _event)
    {  
        transitions.push_back(
            TransStruct{
                ._event = _event, 
                ._from = StateType::template find<From>(),
                ._to = StateType::template find<To>()
            });
    }
    
    expectation on(const event& _event) 
    {
        return state<C...>::on(_event);
    }


    expectation signal(const event& _event)
    {
        for(const auto& t: transitions)
        {
            if (t._event == _event &&
                StateType::child.index() == t._from)
            {
                StateType::child = typename StateType::VariantType(transitioning());
                auto result = construct_variant_by_index<typename StateType::VariantType>(t._to);
                StateType::child = result.value();
                return as_expected;
            }
        }
        return on(_event);
    } 

private:
    std::vector<TransStruct> transitions;    // TODO: avoid heap. Linked list, but return from Transition?
    // OR require it to be the initializer list and keep that. Its elements are created with a templated function to make it 
    // but that function would require the variant/sm already which is being constructed so circular dependency 
};


}
