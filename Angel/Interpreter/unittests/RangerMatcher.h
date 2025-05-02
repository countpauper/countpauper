#include <gtest/gtest.h>
#include <gmock/gmock.h>

MATCHER_P(RangeEqVector, expected, "Range equals initializer list") 
{
    auto arg_it = arg.begin();
    auto exp_it = expected.begin();

    // Check element-wise equality
    while (arg_it != arg.end() && exp_it != expected.end()) {
        if (*arg_it != *exp_it) {
            *result_listener << "Mismatch at position "
                             << std::distance(expected.begin(), exp_it) 
                             << ": expected " << std::string(*exp_it)
                             << ", got " << std::string(*arg_it);
            return false;
        }
        ++arg_it;
        ++exp_it;
    }

    // Check length equality
    if (arg_it != arg.end() || exp_it != expected.end()) {
        *result_listener << "Length mismatch";
        return false;
    }
    return true;
}

template<typename T>
inline auto RangeEq(std::initializer_list<T> ilist) {
    return RangeEqVector(std::vector<T>(ilist));
}
