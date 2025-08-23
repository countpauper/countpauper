#pragma once 
#include <expected> 
#include <errno.h>

using expectation = std::expected<void, errno_t>;
static constexpr expectation as_expected;