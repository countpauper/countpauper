#pragma once 
#include <type_traits>
#include <tuple>
#include <functional>

namespace eul 
{

// Captor is an invocable that captures some state a wrapped invocable at creation 
// when the captor is called with arguments it passed the captured state and the arguments to the functor
template<typename Fn, typename... Captures>
class captor 
{
public:
    template<typename... PCaptures>
    captor(Fn&& fn, PCaptures&&... captives) :
        _fn(std::move(fn)),
        _captives(std::forward<PCaptures>(captives)...)
    {
    }

    template<typename... Args> 
    std::invoke_result<Fn, Captures..., Args...>::type operator()(Args&&... args) const 
    {
        //return std::invoke(_fn, std::forward<Captures>(_captives)..., std::forward<Args>(args)...);
        return std::apply(_fn, std::tuple_cat(_captives, std::make_tuple(args...)));
    }
private:
    Fn _fn;
    std::tuple<Captures...> _captives;
};

}