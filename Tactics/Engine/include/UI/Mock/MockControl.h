#pragma once
#include <gmock/gmock.h>
#include "UI/Control.h"

namespace Engine::Test
{

class MockControl : public Control
{
public:
    using Control::Control;
    MOCK_METHOD(void, Render, (), (const override));
    MOCK_METHOD(Control*, Click, (Coordinate), (const override));
};

}
