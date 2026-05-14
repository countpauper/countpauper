#pragma once
#include <ranges>

template <typename R, typename T>
concept RangeOf = std::ranges::range<R> &&
    std::same_as<std::remove_cvref_t<std::ranges::range_value_t<R>>, T>;
