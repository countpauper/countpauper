#pragma once
#include <memory>
#include <stdexcept>

namespace Engine
{

template<class T>
class Singleton final
{
public:
    Singleton()
    {
        Construct();
    }

    template<typename... Params,
        typename std::enable_if<std::is_constructible<T, Params&&...>::value>::type * = nullptr>
    Singleton(Params&&... params)
    {
        Construct(std::forward<Params>(params)...);
    }

    static T& Get()
    {
        return Construct();
    }

    static void Reset()
    {
        single.reset();
   }

    T* operator->() const
    {
        return &Singleton<T>::Get();
    }

    T& operator*() const
    {
        return Singleton<T>::Get();
    }
private:
    template<typename... Params>
    static T& Construct(Params&&... params)
    {
        if (!single)
            single = std::move(std::make_unique<T>(std::forward<Params>(params)...));
        return *single;
    }
    static std::unique_ptr<T> single;
};

template<class T> std::unique_ptr<T> Singleton<T>::single;
}
