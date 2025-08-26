#include "expectation.hpp"
#include <gtest/gtest.h>
#include <gmock/gmock.h>

namespace Test
{
using namespace ::testing;


TEST(expectation, as_expected)
{
    EXPECT_TRUE(as_expected);
    EXPECT_TRUE(expectation());
    EXPECT_EQ(as_expected.error(), 0);
}

TEST(expectation, unexpected)
{
    EXPECT_FALSE(expectation{std::unexpected(EBADF)});
    EXPECT_EQ(expectation{EBADF}.error(), EBADF);
}

TEST(expectation, ignore)
{
    EXPECT_TRUE(expectation{EBADF}.ignore(EBADF));
    EXPECT_FALSE(expectation{EBADF}.ignore(EINVAL));
    EXPECT_TRUE(expectation{EBADF}.ignore({EINVAL,EBADF}));
}

TEST(expectation, EXPECTED)
{
    EXPECTED(as_expected);
    EXPECT_UNEXPECTED(expectation{EBADF});
    EXPECT_ERROR(expectation{EBADF}, EBADF);
}

TEST(expectation, and)
{
    EXPECTED(as_expected & as_expected);
    EXPECT_ERROR(as_expected & expectation{EBADF}, EBADF);
    EXPECT_ERROR(expectation{EBADF} & as_expected, EBADF);
    EXPECT_ERROR(expectation{EFAULT} & expectation{EBADF}, EFAULT);
}

TEST(expectation, lazy_and)
{
    MockFunction<expectation()> stillCalled;
    EXPECT_CALL(stillCalled, Call()).Times(1);
    EXPECTED(as_expected && stillCalled.AsStdFunction());
    EXPECT_ERROR(expectation{EBADF} && stillCalled.AsStdFunction(), EBADF);
    EXPECT_ERROR(as_expected && []() -> expectation { return std::unexpected(EFAULT); }, EFAULT);
}

TEST(expectation, monadic)
{
    MockFunction<std::expected<void,errno_t>()> stillCalled;
    EXPECT_CALL(stillCalled, Call()).Times(1);
    auto t = as_expected.and_then(stillCalled.AsStdFunction());
    EXPECT_TRUE(t);
}

TEST(expectation, lazy_or)
{
    MockFunction<expectation()> stillCalled;
    EXPECT_CALL(stillCalled, Call()).Times(1);
    EXPECTED(as_expected || stillCalled.AsStdFunction());
    EXPECTED(expectation{EBADF} || stillCalled.AsStdFunction());
    EXPECT_ERROR(expectation{EBADF} || []() { return expectation(EFAULT); }, EFAULT);
}


TEST(expectation, or)
{
    EXPECTED(as_expected | as_expected);
    EXPECTED(as_expected | expectation{EBADF});
    EXPECTED(expectation{EBADF} | as_expected);
    EXPECT_ERROR(expectation{EFAULT} | expectation{EBADF}, EBADF);
}

}