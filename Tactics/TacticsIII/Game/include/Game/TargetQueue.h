#pragma once
#include "Game/TargetQueue.h"
#include "Utility/MutableTuple.h"
#include <stdexcept>

namespace Game
{


template<typename... T> 
class TargetQueue
{
    using ContainerType = Engine::MutableTuple<T...>;
public: 
    using variant_type = ContainerType::variant_type;
    using tuple_type = ContainerType::tuple_type;
    
    bool IsComplete() const 
    { 
        return count == elements.size();
    }

    bool IsEmpty() const 
    {
        return count == 0;
    }

    template<typename ET>
    void push_back(ET&& value) 
    {
        if (IsComplete())
            throw std::out_of_range("Target queue is already complete."); 
        elements.set(count, std::forward<ET>(value));
        ++count;
    }
    
    template<typename ET>
    bool ExpectNext() const 
    {
        if constexpr(sizeof...(T)==0)
            return std::is_void_v<ET>;
        else 
        {
            if (IsComplete())
                return std::is_void_v<ET>;
            return elements.template acceptsTypeAt<ET>(count);
        }
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

    void clear()
    {
        count = 0;
    }

    // Convert back to a std::tuple<Ts...>
    tuple_type as_tuple() const 
    {
        if (!IsComplete())
            throw std::runtime_error("Target queue is incomplete.");
        return elements.as_tuple();
    }

private:
    std::size_t count = 0;
    ContainerType elements;
};


template<> 
class TargetQueue<>
{
public: 
    using variant_type = std::variant<std::monostate>;
    using tuple_type = std::tuple<>;

    TargetQueue()
    {   
    }

    bool IsComplete() const 
    { 
        return true;
    }

    bool IsEmpty() const 
    {
        return true;
    }

    template<typename ET>
    void push_back( ET&& value) 
    {
        throw std::out_of_range("Target queue is always complete.");        
    }

    template<typename ET>
    bool ExpectNext() const 
    {
        return std::is_void_v<ET>;;
    }

    variant_type operator[](size_t)
    {
        throw std::out_of_range("Index out of range.");
    }

    variant_type pop()
    {
        throw std::runtime_error("Target queue is always empty.");
    }

    void clear()
    {
    }

    // Convert back to a std::tuple<Ts...>
    tuple_type as_tuple() const 
    {
        return tuple_type();
    }
};


// Extract tuple elements and unfold them as template parameters
template<typename Tuple>
struct ExtractTargets;

template<typename... Ts>
struct ExtractTargets<std::tuple<Ts...>> {
    using type = TargetQueue<Ts...>;
};

}
