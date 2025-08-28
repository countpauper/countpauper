#include "captor.hpp"
#include <gtest/gtest.h>
#include <gmock/gmock.h>

namespace eul::Test
{

using namespace ::testing;

TEST(captor, void)
{
    MockFunction<void(void)> cb;
    captor<std::function<void()>> c(cb.AsStdFunction());
    EXPECT_CALL(cb, Call()).Times(1);
    c();
}

TEST(captor, capture_int)
{
    MockFunction<void(int)> cb;
    captor<std::function<void(int)>, int> c(cb.AsStdFunction(), 42);
    EXPECT_CALL(cb, Call(42)).Times(1);
    c();
}

TEST(captor, argument)
{
    MockFunction<void(int)> cb;
    captor<std::function<void(int)>> c(cb.AsStdFunction());
    EXPECT_CALL(cb, Call(13)).Times(1);
    c(13);
}

TEST(captor, retvalue)
{
    MockFunction<int(int)> cb;
    captor<std::function<int(int)>, int> c(cb.AsStdFunction(), -3);
    EXPECT_CALL(cb, Call(-3)).Times(1).WillOnce(Return(2));
    EXPECT_EQ(c(), 2);
}

TEST(captor, mix)
{
    MockFunction<unsigned(int, char)> cb;
    captor<decltype(cb.AsStdFunction()), int> c(cb.AsStdFunction(), -5);
    EXPECT_CALL(cb, Call(-5, 3)).Times(1).WillOnce(Return(9));
    EXPECT_EQ(c(3), 9);
}

}

