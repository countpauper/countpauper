#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "UI/Bus.h"
#include "UI/WindowMessages.h"

using namespace ::testing;

namespace Engine::Test
{

class MockPassenger : public Passenger
{
public:
    MOCK_METHOD(void, OnMessage, (const Message&), (override));
};

// There's not enough introspection to match message content without adding a lot of boiler plate to message
// for now just match on type for the tests. The responsibility of the bus is to pass the reference anyway. Not the content
// A bus that defers passing the message and needs to serialize it would need this tested, but that should have similar
// introspection/boiler plate
MATCHER_P(MessageOfType, messageType, std::string("Is not of the type ") + messageType->name() )
{
    return (arg.GetType() == std::type_index(*messageType));
}

TEST(Bus, MessageIsDeliveredOnlyToSubscriber)
{
    MockPassenger passenger;
    MockPassenger other;
    Bus bus;
    bus.Subscribe(passenger, {MessageType<Click>()});
    bus.Subscribe(other, {MessageType<KeyPressed>()});
    EXPECT_CALL(passenger, OnMessage(MessageOfType(&typeid(Click))));
    EXPECT_CALL(other, OnMessage(_));
    std::stringstream ss;
    bus.Post(Click{1,2,3});
}


}
