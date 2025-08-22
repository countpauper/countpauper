#include "signal.hpp"
#include <gtest/gtest.h>
#include <gmock/gmock.h>

using namespace ::testing;

namespace eul::Test
{

TEST(Signal, ConnectDisconnect)
{
    Slot<> slot;
    EXPECT_FALSE(slot.IsConnected());
    Signal<> signal;
    slot = signal.Connect([](){});
    EXPECT_TRUE(slot.IsConnected());
    EXPECT_EQ(signal.connections(), 1);
    slot.Disconnect();
    EXPECT_FALSE(slot.IsConnected());
    EXPECT_EQ(signal.connections(), 0);
    slot.Disconnect();
}

TEST(Signal, One) 
{
    Signal<> signal;
    MockFunction<void(void)> cb;
    EXPECT_CALL(cb, Call).Times(1);     
    {
        auto slot = signal.Connect(cb.AsStdFunction());

        signal();
    }
    signal();
}

TEST(Signal, Multiple) 
{
    Signal<> signal;
    MockFunction<void(void)> a;
    MockFunction<void(void)> b;
    EXPECT_CALL(a, Call).Times(1);     
    EXPECT_CALL(b, Call).Times(1);     
    Slot<> slotA(signal, a.AsStdFunction());
    auto slotB = signal.Connect(b.AsStdFunction());
    EXPECT_EQ(signal.connections(), 2);
    signal();

    signal.DisconnectAll();
    EXPECT_EQ(signal.connections(), 0);
}

TEST(Signal, SignalWithArgs) 
{
    Signal<bool, int> signal;
    MockFunction<void(bool, int)> a;
    EXPECT_CALL(a, Call(true, 3)).Times(1);     
    auto slot = signal.Connect(a.AsStdFunction());
    const int v = 3;
    signal(true, v);
}

TEST(Signal, DisconnectDuringSignal) 
{
    Signal<> signal;

    MockFunction<void()> stillCalled;    
    EXPECT_CALL(stillCalled, Call()).Times(2);
    Slot<> backSlot(signal, stillCalled.AsStdFunction());

    MockFunction<void()> disconnecter;
    Slot<> slot;
    EXPECT_CALL(disconnecter, Call()).Times(1).WillOnce(Invoke([&slot]()
    {
        slot.Disconnect();
    }));
    slot = signal.Connect(disconnecter.AsStdFunction());
    Slot<> frontSlot(signal, stillCalled.AsStdFunction());

    signal();
    EXPECT_FALSE(slot.IsConnected());
    EXPECT_TRUE(backSlot.IsConnected());
}


}