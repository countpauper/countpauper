#pragma once 

#include <variant>
#include <array>
#include <expected> 
#include <errno.h>

namespace eul 
{

/*
template <typename Variant, typename... Args, std::size_t... Is>
constexpr auto _alternative_creators(std::index_sequence<Is...>) 
{
    using Creator = Variant(*)(Args&&...);
    return std::array<Creator, sizeof...(Is)>{
        {
            [](Args&&... args) -> Variant 
            {
                return Variant{std::in_place_index<Is>, std::forward<Args>(args)...};
            }...
        }
    };
}
*/

template <typename Variant, typename... Args, std::size_t... Is>
constexpr auto _alternative_creators(std::index_sequence<Is...>) 
{
    using Creator = std::expected<Variant, errno_t>(*)(Args&&...);
    return std::array<Creator, sizeof...(Is)>{
        {
            [](Args&&... args) -> std::expected<Variant, errno_t> 
            {
                if constexpr (std::is_constructible_v<std::variant_alternative_t<Is, Variant>, Args...>)
                    return Variant{std::in_place_index<Is>, std::forward<Args>(args)...};
                else 
                    return std::unexpected(EBADF);
            }...
        }
    };

}
/*

template <typename Variant, typename... Args, std::size_t... Is>
constexpr auto _alternative_creators(std::index_sequence<Is...>) 
{
    using Creator = Variant(*)(Args&&...);
    return std::array<Creator, sizeof...(Is)>{
        (
            std::is_constructible_v<std::variant_alternative_t<Is, Variant>, Args...> ?
                [](Args&&... args) -> Variant {
                    return Variant{std::in_place_index<Is>, std::forward<Args>(args)...};
                } :
                static_cast<Creator>(nullptr)
        )...
    };
}
*/
// construct a variant by specifying the index of the type to create, optionally with arguments 
// returns EINVAL if there is no alternative with that index 
// returns EBADF if the alternative does not have a constructor for the provided arguments
// construct_variant_by_index<std::variant<A,B>>(1, "foo") will create alterantive B("foo")
template <typename Variant, typename... Args>
constexpr std::expected<Variant, errno_t> construct_variant_by_index(std::size_t idx, Args&&... args) 
{
    constexpr std::size_t N = std::variant_size_v<Variant>;
    if (idx >= N)
        return std::unexpected(EINVAL);
    static constexpr auto creators = _alternative_creators<Variant, Args...>(std::make_index_sequence<N>{});
    return creators[idx](std::forward<Args>(args)...);
}

// Use to match a subset of alternatives when using std::visit 
// std::variant<A,B> var
// std::visit(overload_visit { 
//  [](const A&){},
//  [](const B&){}
// }, var);
template<class... Ts> struct overloaded_visit : Ts... { using Ts::operator()...; };


template <typename T, typename Variant, std::size_t... Is>
constexpr std::size_t variant_index_of_impl(std::index_sequence<Is...>) 
{
    std::size_t result = static_cast<std::size_t>(-1);
    ((std::is_same_v<T, std::variant_alternative_t<Is, Variant>> ? (result = Is, false) : false) || ...);
    
    return result;
}

// index = variant_index_of<Alternative, Variant> 
// Get the index of a specific Alternative or std::numeric_limits<std::size_t>::max() if that type 
// is not an alternative of the Variant
template <typename T, typename Variant>
constexpr std::size_t variant_index_of() 
{
    return variant_index_of_impl<T, Variant>(std::make_index_sequence<std::variant_size_v<Variant>>{});
}
}