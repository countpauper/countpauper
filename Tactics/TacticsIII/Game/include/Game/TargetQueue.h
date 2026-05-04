#pragma once
#include <string>
#include "Game/Requirement.h"
#include "Game/Delta.h"
#include "Game/TargetQueue.h"
#include <tuple>

namespace Game
{


template<typename T, typename... Ts>
concept IsOneOf = (std::same_as<T, Ts> || ...);

template<typename... T> 
class TargetQueue
{
public: 
    using variant_type = std::variant<T...>;
    using tuple_type = std::tuple<T...>;

    TargetQueue(const tuple_type& initial)
    {   // Initialize each variant to the expected type, so push_back can verify it
        std::apply([this](auto... args) 
        {
            size_t i = 0;
            ((elements[i++] = args), ...);
        }, initial);
    }

    TargetQueue() : 
        TargetQueue(tuple_type())
    {
    }

    bool IsComplete() const 
    { 
        return count == elements.size();
    }

    bool IsEmpty() const 
    {
        return count == 0;
    }

    template<typename ET>
    //requires IsOneOf<ET, T...>
    void push_back( ET&& value) 
    {
        if (IsComplete())
            throw std::out_of_range("Target queue is already complete.");        
        if (!std::holds_alternative<ET>(elements[count])) 
            throw std::bad_variant_access();
        elements[count++] = std::forward<ET>(value);
    }

    // Get the value at index idx (as a variant)
    variant_type operator[](size_t idx)
    {
        if (idx >= count) 
            throw std::out_of_range("Index out of range.");
        return elements[idx];
    }

    variant_type pop()
    {
        if (IsEmpty())
            throw std::runtime_error("Target queue is empty.");
        return elements[--count];
    }

    // Convert back to a std::tuple<Ts...>
    tuple_type as_tuple() const 
    {
        if (!IsComplete())
            throw std::runtime_error("Target queue is incomplete.");
        if constexpr(sizeof...(T)==0)
            return std::tuple<>();
        else if constexpr(sizeof...(T)==1)
            return std::make_tuple(std::get<T...>(elements[0]));
        else
        {
            return [this]<std::size_t... Is>(std::index_sequence<Is...>) {
                return std::make_tuple(std::get<std::tuple_element_t<Is, tuple_type>>(elements[Is])...);
            }(std::index_sequence_for<T...>{});
        }
    }
private:
    std::array<variant_type, sizeof...(T)> elements;
    std::size_t count = 0;
};


// Extract tuple elements and unfold them as template parameters
template<typename Tuple>
struct ExtractTargets;

template<typename... Ts>
struct ExtractTargets<std::tuple<Ts...>> {
    using type = TargetQueue<Ts...>;
};

}
