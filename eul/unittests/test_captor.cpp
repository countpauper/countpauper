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

int free_function(int a )
{
    return a+1;
}

TEST(captor, is_smoll)
{
    captor<int(*)(int), int> cap(free_function, 3);
    int some_int = 3;
    std::function<int()> lambda = [some_int]() { return some_int+1; };
    static_assert(sizeof(cap) < sizeof(lambda));
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

struct CaptorMock 
{
    MOCK_METHOD(void, fn_void, ());
    MOCK_METHOD(unsigned, fn_params, (char, int));
};

TEST(captor, void_method)
{
    CaptorMock mock; 
    method_captor<CaptorMock, decltype(CaptorMock::fn_void)> cap(mock, CaptorMock::fn_void);
    EXPECT_CALL(mock, fn_void());
    cap();
}

TEST(captor, mix_method)
{
    CaptorMock mock; 
    method_captor<CaptorMock, decltype(CaptorMock::fn_params), char> cap(mock, CaptorMock::fn_params, 'a');
    EXPECT_CALL(mock, fn_params('a',-2)).Times(1).WillOnce(Return(3));
    EXPECT_EQ(cap(-2), 3);
}

}

