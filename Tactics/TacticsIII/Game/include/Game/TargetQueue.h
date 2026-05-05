#pragma once
#include <string>
#include "Game/Requirement.h"
#include "Game/Delta.h"
#include "Game/TargetQueue.h"
#include <tuple>

namespace Game
{

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
    void push_back( ET&& value) 
    {
        if (IsComplete())
            throw std::out_of_range("Target queue is already complete.");        
        if (!isConvertibleToActiveType<ET>(elements[count])) 
            throw std::bad_variant_access();
        elements[count++] = std::forward<ET>(value);
    }

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
    // Helper to check if ET is convertible to the active variant type of var
    template<typename ET>
    bool isConvertibleToActiveType(const variant_type& var) const 
    {
        return std::visit([](const auto& arg) 
        {
            using ActiveType = std::decay_t<decltype(arg)>;
            return std::is_convertible_v<ET, ActiveType>;
        }, var);
    }

    // TODO: it could be possible to store the tuple directly, but setting a tuple element then requires a complicated fold expression
    // that unpacks all elements of the current version and only replaces the one at count with the new version to reconstruct the new tuple
    // This might be worht it if there are vastly different size elements in the tuple and the size of the array becomes unnecessarily big.
    // also isConvertibleToActiveType would have to be rewritten with a fold expression that only checks std::is_convertible at the matching
    // runtime==compile time element (and the rest true, then conjunction of the results).  
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
