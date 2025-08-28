#include "signal.hpp"
#include "captor.hpp"
#include <gtest/gtest.h>
#include <gmock/gmock.h>

using namespace ::testing;

namespace eul::Test
{

TEST(signal, connect_disconnect)
{
    slot_fn<> s;
    EXPECT_FALSE(s.connected());
    signal<> sig;
    s = sig.connect([](){});
    EXPECT_TRUE(s.connected());
    EXPECT_EQ(sig.connections(), 1);
    s.disconnect();
    EXPECT_FALSE(s.connected());
    EXPECT_EQ(sig.connections(), 0);
    s.disconnect();
}

TEST(signal, one) 
{
    signal<> sig;
    MockFunction<void(void)> cb;
    EXPECT_CALL(cb, Call).Times(1);     
    {
        auto s = sig.connect(cb.AsStdFunction());

        sig();
    }
    sig();
}

TEST(signal, multiple) 
{
    signal<> sig;
    MockFunction<void(void)> a;
    MockFunction<void(void)> b;
    EXPECT_CALL(a, Call).Times(1);     
    EXPECT_CALL(b, Call).Times(1);     
    slot_fn<> A(sig, a.AsStdFunction());
    auto B = sig.connect(b.AsStdFunction());
    EXPECT_EQ(sig.connections(), 2);
    sig();

    sig.disconnect();
    EXPECT_EQ(sig.connections(), 0);
    sig();
}

TEST(signal, signal_with_args) 
{
    signal<bool, int> sig;
    MockFunction<void(bool, int)> a;
    EXPECT_CALL(a, Call(true, 3)).Times(1);     
    auto s = sig.connect(a.AsStdFunction());
    const int v = 3;
    sig(true, v);
}

TEST(signal, disconnect_during_signal) 
{
    signal<> sig;

    MockFunction<void()> stillCalled;    
    EXPECT_CALL(stillCalled, Call()).Times(2);
    slot_fn<> backslot(sig, stillCalled.AsStdFunction());

    MockFunction<void()> disconnecter;
    slot_fn<> s;
    EXPECT_CALL(disconnecter, Call()).Times(1).WillOnce(Invoke([&s]()
    {
        s.disconnect();
    }));
    s = sig.connect(disconnecter.AsStdFunction());
    slot_fn<> frontslot(sig, stillCalled.AsStdFunction());

    sig();
    EXPECT_FALSE(s.connected());
    EXPECT_TRUE(backslot.connected());
}


TEST(signal, null_callback_cant_be_connected) 
{
    signal<> s;
    auto nullslot = s.connect(nullptr);
    EXPECT_FALSE(nullslot.connected());
    EXPECT_EQ(s.connections(), 0);
    s();
}

void free_function(int& a) { a+=1; } 

TEST(signal, free_function_callback) 
{
    signal<int&> s;
    auto free_slot = s.connect2(free_function);
    int v = 0;
    s(v);
    EXPECT_EQ(v, 1);
}

TEST(signal, two_captors_called) 
{
    MockFunction<void(unsigned, int)> call1;   
    captor<decltype(call1.AsStdFunction()), unsigned> cap1(call1.AsStdFunction(), 0);
    EXPECT_CALL(call1, Call(0, -3));

    MockFunction<void(unsigned, int, int)> call2;   
    captor<decltype(call2.AsStdFunction()), unsigned, int> cap2(call2.AsStdFunction(), 1, 2);
    EXPECT_CALL(call2, Call(1, 2, -3));

    signal<int> sig;
    auto slot1 = sig.connect(cap1);
    auto slot2 = sig.connect(cap2);

    sig(-3);
}


}