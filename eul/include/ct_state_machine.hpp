#pragma once 

#include "event.hpp"
#include "expectation.hpp"
#include "variant.hpp"
#include <initializer_list>
#include <vector> // TODO linked list or something 
#include <cstdint> 
#include <type_traits>
#include <assert.h>
#include <limits>

// Compile time state machine: 
// 1) RAII states constructed when activated. 
// 2) Compile time generation of state transition table

namespace eul
{

using state_hash = std::size_t; 
class stateIF
{
public:
    virtual expectation on(const event& _event);
};

template<typename T> 
concept is_state  = std::is_base_of_v<stateIF, T>;

template<is_state... C> class state; 
template<> class state<>; 


template<typename T>
concept is_leaf_state = std::is_base_of_v<state<>, T>;

// Branch (parent) state.
template<typename T>
concept is_branch_state = requires(T t) {
    { t.template in<state<>>() } -> std::convertible_to<bool>; 
    { t.template get() } -> std::convertible_to<stateIF>; 
//    { constexpr T::template find<state<>>() } -> std::convertible_to<state_hash>; 
};


class transitioning : public stateIF 
{
};

template<typename FindT, typename Variant> 
constexpr state_hash find_child_transition();

static constexpr state_hash nostate(-1);
static constexpr state_hash anystate(-2);

template<typename FindT, typename Variant, std::size_t Idx=0>
constexpr state_hash find_child_transition_impl() 
{
    using ChildState = std::variant_alternative_t<Idx, Variant>;
    if constexpr (is_branch_state<ChildState>)
    {
        constexpr state_hash result = ChildState::template find<FindT>();
        if constexpr (result!=nostate)   // TODO can this be constexpr? 
        {
            static_assert(result < anystate / std::variant_size_v<Variant>); // prevent overflow    
            return Idx + result * std::variant_size_v<Variant>;
        }
    }
    if constexpr (Idx<std::variant_size_v<Variant>-1)
        return find_child_transition_impl<FindT, Variant, Idx+1>();
    else 
        return nostate;
}

template<typename FindT, typename Variant> 
constexpr state_hash find_child_transition() 
{
    state_hash idx = variant_index_of<FindT, Variant>();
    if (idx == nostate)
    {   
        return find_child_transition_impl<FindT, Variant>();
    }
    else 
    {
        return idx;
    }
}

template<is_state... C>
class state : public stateIF
{
public:
    state() = default;

    template<is_state... FC>
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
        return std::visit(overloaded_visit{
            [](auto& obj) -> stateIF&
            requires is_branch_state<decltype(obj)>  
            {
                return obj.get();
            },
            [](stateIF& leaf) -> stateIF&
            {
                return leaf;
            }
        }, child);
    }    
    expectation change(std::size_t to)
    {
        child = VariantType(transitioning());
        auto index = to % std::variant_size<VariantType>();
        auto sub = to / std::variant_size<VariantType>();
        if (index  != child.index())
        { 
            auto result = construct_variant_by_index<VariantType>(index);
            if (!result)
                return std::unexpected(result.error());
            
            child = result.value();
        }
        return std::visit(overloaded_visit
        {
            [sub](auto& _branch) -> expectation
            requires is_branch_state<decltype(_branch)>
            { 
                return _branch.change(sub);
            },
            [sub](auto&) -> expectation
            {
                if (sub==0)
                    return as_expected;
                else 
                    return std::unexpected(ENOENT);
            }
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
    friend constexpr state_hash find_child_transition_impl();

    template<typename CT>
    static constexpr state_hash find()
    {
        return find_child_transition<CT, VariantType>();
    }


    VariantType child;
};

// Child/Leaf state 
template<>
class state<> : public stateIF
{
    template<is_state... FC>
    friend class state;
public:
    state() = default;
};

template<typename... C>
class machine : public state<C...> 
{
public:
    using StateType = state<C...>;

    machine() = default;
    struct TransStruct
    {
        event _event;
        state_hash _from;
        state_hash _to;
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
    
    template<typename To>
    void transition(const event& _event)
    {  
        transitions.push_back(
            TransStruct{
                ._event = _event, 
                ._from = anystate,
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
                (t._from == anystate || StateType::child.index() == t._from))
            {
                return StateType::change(t._to);
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
