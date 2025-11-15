#pragma once 
#include <type_traits>
#include <tuple>
#include <functional>

namespace eul 
{

// Captor is an invocable that captures some state a wrapped invocable at creation. 
// It's an alternative for std::function and lambdas for two reasons: 
// 1) can be smaller than std::function with less captured
// 2) can capture more than a std::function without heap allocation
// when the captor is called with arguments it passed the captured state and the arguments to the functor
template<typename Fn, typename... Captures>
class captor 
{
public:
    captor() = default;
    
    template<typename... PCaptures>
    captor(Fn&& fn, PCaptures&&... captives) :
        _fn(std::move(fn)),
        _captives(std::forward<PCaptures>(captives)...)
    {
    }

    template<typename... Args> 
    std::invoke_result<Fn, Captures..., Args...>::type operator()(Args&&... args) const 
    {
        return std::apply(_fn, std::tuple_cat(_captives, std::make_tuple(args...)));
    }
private:
    Fn _fn;
    std::tuple<Captures...> _captives;
};

template<typename Class, typename Fn,  typename... Captures>
class method_captor : public captor<Fn, Class*, Captures...>
{
public:
    template<typename... PCaptures>
    method_captor(Class& cls, Fn&& fn, PCaptures&&... captives) :
        captor<Fn, Class*, Captures...>(std::move(fn), &cls, std::forward<PCaptures>(captives)...)
    {
    }
};

}