#pragma once 
#include <optional>
#include <type_traits>

namespace eul
{
template<class T>
class Singleton final
{
public:
    template<typename ...Args>
    constexpr Singleton(Args&&... args)
    { 
        if (!has_value()) {
            if constexpr (std::is_constructible_v<T, Args...>) {
                instance = T(std::forward<Args>(args)...);
            }
        }
    }

    static constexpr T& Get()
    {
        return *instance;
    }

    bool has_value() const 
    {
        return instance.has_value();
    }
    
    T* operator->() const
    {
        if (has_value())
            return &Singleton<T>::Get();
        else 
            return nullptr;
    }

    T& operator*() const
    {
        return Singleton<T>::Get();
    }
private:
    static std::optional<T> instance; 
};

template<class T>
std::optional<T> Singleton<T>::instance; 
}



