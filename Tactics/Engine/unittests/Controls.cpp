#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "UI/Control.h"
#include "UI/Mock/MockControl.h"

using namespace ::testing;

namespace Engine::Test
{

TEST(Controls, FindSelf)
{
    MockControl ctrl("foo");
    EXPECT_EQ(ctrl.FindControl("foo"), &ctrl);
    EXPECT_EQ(ctrl.Find<Controls>("foo"), nullptr);
    EXPECT_EQ(ctrl.FindControl("bar"), nullptr);

}


TEST(Controls, FindSubControl)
{
    MockControl ctrl("bar");
    Controls controls("foo");
    controls.push_back(&ctrl);
    EXPECT_EQ(controls.FindControl("foo.bar"), &ctrl);
    EXPECT_EQ(controls.FindControl("foo.baz"), nullptr);
    EXPECT_EQ(controls.FindControl("baz.bar"), nullptr);
}

TEST(Controls, FindAnonymousSubControl)
{
    MockControl ctrl("bar");
    Controls controls("");
    controls.push_back(&ctrl);
    EXPECT_EQ(controls.FindControl("bar"), &ctrl);
}

}
