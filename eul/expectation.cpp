#include "expectation.hpp"

errno_t expectation::error() const
{
    if (has_value())
        return 0;
    else 
        return std::expected<void, errno_t>::error();
}

expectation expectation::ignore(std::initializer_list<errno_t> ignored) const
{
    if (has_value())
        return *this;
    for(auto err : ignored) 
    {
        if (error() == err)
            return as_expected;
    }
    return *this;
}    


