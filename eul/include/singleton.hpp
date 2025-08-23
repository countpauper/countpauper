#pragma once 
#include <optional>
#include <type_traits>

namespace eul
{
template<class T>
class singleton final
{
public:
    template<typename ...Args>
    constexpr singleton(Args&&... args)
    { 
        if (!has_value()) {
            if constexpr (std::is_constructible_v<T, Args...>) {
                _instance = T(std::forward<Args>(args)...);
            }
        }
    }

    static constexpr T& Get()
    {
        return *_instance;
    }

    bool has_value() const 
    {
        return _instance.has_value();
    }
    
    T* operator->() const
    {
        if (has_value())
            return &singleton<T>::Get();
        else 
            return nullptr;
    }

    T& operator*() const
    {
        return singleton<T>::Get();
    }
private:
    static std::optional<T> _instance; 
};

template<class T>
std::optional<T> singleton<T>::_instance; 
}



