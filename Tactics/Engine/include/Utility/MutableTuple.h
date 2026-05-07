#pragma once
#include <tuple>
#include <variant>
#include <cstdint>
#include <stdexcept>

namespace Engine
{

template<typename... T>
class MutableTuple
{
public:
    using variant_type = std::variant<T...>;
    using tuple_type = std::tuple<T...>;

    MutableTuple(const tuple_type& initial)
    {   // Initialize each variant to the expected type, so push_back can verify it
        std::apply([this](auto... args)
        {
            size_t i = 0;
            ((elements[i++] = args), ...);
        }, initial);
    }

    MutableTuple() :
        MutableTuple(tuple_type())
    {
    }

    variant_type& operator[](size_t idx)
    {
        return elements[idx];
    }

    variant_type operator[](size_t idx) const
    {
        return elements[idx];
    }

    template<typename ET>
    void set(std::size_t index, ET&& value)
    {
        if (!acceptsTypeAt<ET>(index))
            throw std::bad_variant_access();
        elements[index] = std::forward<ET>(value);
    }

    template<typename ET>
    ET get(std::size_t index)
    {
        return std::get<ET>(elements.at(index));
    }

    // Convert back to a std::tuple<Ts...>
    tuple_type as_tuple() const
    {
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
    std::size_t size() const
    {
        return elements.size();
    }

    // Helper to check if ET is convertible to the active variant type of var
    template<typename ET>
    constexpr bool acceptsTypeAt(std::size_t idx) const
    {
        return isConvertibleToType<ET>(elements.at(idx));
    }
protected:
    // Helper to check if ET is convertible to the variant type of var
    template<typename ET>
    constexpr bool isConvertibleToType(const variant_type& var) const
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
};


template<>
class MutableTuple<>
{
public:
    using variant_type = std::variant<std::monostate>;
    using tuple_type = std::tuple<>;

    MutableTuple() = default;

    variant_type operator[](size_t)
    {
        throw std::out_of_range("No elements in an empty mutable tuple");
    }

    template<typename ET>
    void set(std::size_t, ET&&)
    {
        throw std::out_of_range("Can't set any element in an empty mutable tuple");
    }

    template<typename ET>
    ET get(std::size_t)
    {
        throw std::out_of_range("Can't get any element in an empty mutable tuple");
    }

    // Convert back to a std::tuple<Ts...>
    tuple_type as_tuple() const
    {
        return tuple_type();
    }
    std::size_t size() const
    {
        return 0;
    }

    template<typename ET>
    constexpr bool acceptsTypeAt(std::size_t) const
    {
        throw std::out_of_range("Can't accept any type in an empty mutable tuple");
    }
};

}
