#pragma once 

#include <variant>
#include <array>
#include <expected> 

namespace eul 
{

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

template <typename Variant, typename... Args>
constexpr std::expected<Variant, errno_t> construct_variant_by_index(std::size_t idx, Args&&... args) 
{
    constexpr std::size_t N = std::variant_size_v<Variant>;
    if (idx >= N)
        return std::unexpected(EINVAL);
    static constexpr auto creators = _alternative_creators<Variant, Args...>(std::make_index_sequence<N>{});
    return creators[idx](std::forward<Args>(args)...);
}

// Use to match a subset of alternatives 
template<class... Ts> struct overloaded_visit : Ts... { using Ts::operator()...; };


// Get the index of a specific variant or -1 if not found 
template <typename T, typename Variant, std::size_t... Is>
constexpr std::size_t variant_index_of_impl(std::index_sequence<Is...>) 
{
    std::size_t result = static_cast<std::size_t>(-1);
    ((std::is_same_v<T, std::variant_alternative_t<Is, Variant>> ? (result = Is, false) : false) || ...);
    
    return result;
}

template <typename T, typename Variant>
constexpr std::size_t variant_index_of() 
{
    return variant_index_of_impl<T, Variant>(std::make_index_sequence<std::variant_size_v<Variant>>{});
}
}