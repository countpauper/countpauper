#pragma once 
#include <expected> 
#include <errno.h>
#include <functional>

class expectation : public std::expected<void, errno_t>
{
public:
    using base_expected = std::expected<void, errno_t>; 
    using base_expected::expected;
    using error_type = base_expected::error_type;
    using value_type = void;
    using unexpected_type = base_expected::unexpected_type;

    constexpr expectation(errno_t err) : base_expected(std::unexpect, err) {} 
    constexpr expectation ignore(errno_t err) const
    {
        if (!has_value() && error()==err)
            return expectation();
        return *this;
    }

    expectation ignore(std::initializer_list<errno_t> ignored) const;
    errno_t error() const;
};

constexpr expectation operator&(expectation lhs, expectation rhs)
{
    if (lhs)
        return rhs; 
    else 
        return lhs;
}

constexpr expectation operator|(expectation lhs, expectation rhs)
{
    if (!lhs)
        return rhs;
    else 
        return lhs;
}

template<typename Fn>
requires std::same_as<typename std::remove_cvref_t<std::invoke_result_t<Fn>>::error_type, errno_t>
constexpr auto operator&&(expectation lhs, Fn&& rhs)
{
    using R = std::remove_cvref_t<std::invoke_result_t<Fn>>;
    if (!lhs)
        return R(std::unexpected(lhs.error()));
    return std::invoke(std::forward<Fn>(rhs));
}


template<typename Fn>
requires std::same_as<typename std::remove_cvref_t<std::invoke_result_t<Fn>>, expectation>
constexpr expectation operator||(expectation lhs, Fn&& rhs)
{
    if (!lhs)
        return std::invoke(std::forward<Fn>(rhs));
    else 
        return lhs;
}

// custom operators can't perform lazy evluation
expectation operator&&(expectation lhs, expectation rhs) = delete;
expectation operator||(expectation lhs, expectation rhs) = delete;

// google mock helpers
#define EXPECTED(exp) EXPECT_TRUE(exp)
#define EXPECT_ERROR(query, err) EXPECT_EQ(expectation(query).error(), err)
#define EXPECT_UNEXPECTED(query) EXPECT_FALSE(expectation(query))

static constexpr expectation as_expected;