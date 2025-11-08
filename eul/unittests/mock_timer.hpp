#include "timer_itf.hpp"
#include <gmock/gmock.h>

namespace eul::Test
{

struct timer_mock : public timer_itf
{
    MOCK_METHOD(expectation, Start, (), (override));
    MOCK_METHOD(float, Time, (), (const override));
};

};