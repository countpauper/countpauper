#pragma once 

#include "event.hpp"
#include "expectation.hpp"
#include <variant> 
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


template<class... Ts> struct overloaded_visit : Ts... { using Ts::operator()...; };

template <typename T, typename Variant, std::size_t... Is>
constexpr std::size_t variant_index_of_impl(std::index_sequence<Is...>) 
{
    std::size_t result = static_cast<std::size_t>(-1);
    ((std::is_same_v<T, std::variant_alternative_t<Is, Variant>> ? (result = Is, false) : false) || ...);
    return result;
}

template <typename T, typename Variant>
constexpr std::size_t variant_index_of() {
    return variant_index_of_impl<T, Variant>(std::make_index_sequence<std::variant_size_v<Variant>>{});
}


template<typename... C> class state; 
template<> class state<>; 

template<typename T>
concept is_leaf_state = std::is_base_of_v<state<>, T>;

// Branch (parent) state.
template<typename T>
concept is_branch_state = requires(T t) {
    { t.template in<state<>>() } -> std::convertible_to<bool>; 
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
    static const std::size_t npos = -1;
protected:
    expectation on(const event& _event)
    {
        stateIF::on(_event);
        return std::visit([&_event](auto& obj) 
        {
           return obj.on(_event); 
        }, child);
    }

    template<typename CT>
    static std::pair<uint8_t, bool> find()
    {
        std::size_t idx = variant_index_of<CT, VariantType>();
        if (idx == npos)
        {   // TODO iterate over all alterantives until found
            return std::make_pair(0, true);
        }
        else 
        {
            assert(idx<256); // too many TODO error
            return std::make_pair(idx, false);
        }
    }

    using VariantType = std::variant<C..., transitioning>;
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
private:
};

template <typename Variant, std::size_t... Is>
constexpr auto state_creators(std::index_sequence<Is...>) 
{
    return std::array<Variant(*)(), sizeof...(Is)>{
        ([]() -> Variant { return Variant{std::in_place_index<Is>}; })...
    };
}

template <typename Variant>
Variant default_construct_by_index(std::size_t idx) 
{
    constexpr std::size_t N = std::variant_size_v<Variant>;
    assert (idx < N -1 );   // trying to create non existing variant or Transitioning state 
    static constexpr auto creators = state_creators<Variant>(std::make_index_sequence<N>{});
    return creators[idx]();
}


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
        std::pair<std::uint8_t, bool> _from;
        std::pair<std::uint8_t, bool> _to;
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
            assert(!t._from.second); // TODO handle in sub child
            if (t._event == _event &&
                StateType::child.index() == t._from.first)
            {
                StateType::child = typename StateType::VariantType(transitioning());
                assert(!t._to.second); // TODO if sub, then pass that sub as initial state AND SO ON (where is that info)
                StateType::child = default_construct_by_index<typename StateType::VariantType>(t._to.first);
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
