#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "UI/Bus.h"

using namespace ::testing;

namespace Engine::Test
{

class MockPassenger : public Passenger
{
public:
    MOCK_METHOD(void, OnMessage, (const Message&), (override));
};

MATCHER_P(MessageOfType, messageType, std::string("Is not of the type ") + messageType->name() )
{
    return (arg.GetType() == std::type_index(*messageType));
}

TEST(Bus, MessageIsDeliveredToSubscriber)
{
    MockPassenger passenger;
    Bus bus;
    bus.Subscribe(passenger, {MessageType<Click>()});
    EXPECT_CALL(passenger, OnMessage(MessageOfType(&typeid(Click))));
    std::stringstream ss;
    bus.Post(Click{1,2,3});
}


}
