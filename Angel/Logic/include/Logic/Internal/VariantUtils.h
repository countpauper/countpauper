#pragma once

#include <variant>
#include "Logic/Operator.h"

namespace Angel::Logic
{
// Use with std::visit to provide a set of explicitly typed alternatives
// or variants with a subset of alternatives
template<class... Ts> struct overloaded_visit : Ts... { using Ts::operator()...; };
template<class T, typename Var>

// is_alternative<T, Variant> is true if T is one of the variants
concept is_alternative = requires (const Var& var) { 
  []<typename... Args>
    requires (std::same_as<T, Args> || ...) 
      (const std::variant<Args...>&){ } (var);
};


template <typename T>
concept has_size = requires(const T& a) {
    { a.size() } -> std::convertible_to<std::size_t>;
};


template<class T>
concept is_container = has_size<T> && !is_operation<T>;

}
