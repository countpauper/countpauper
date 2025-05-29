#include "Logic/Knowledge.h"
#include "Logic/Function.h"
#include "Logic/Expression.h"
#include <gtest/gtest.h>
#include <gmock/gmock.h>

namespace Angel::Logic::Test
{
using namespace ::testing;

TEST(Function, Construction)
{
    MockFunction<Function::CallbackT> callback;
    EXPECT_TRUE(bool(Function(callback.AsStdFunction(), "test")));
    EXPECT_EQ(Function(callback.AsStdFunction(), "test"), Function(callback.AsStdFunction(), "another test"));
    EXPECT_EQ(Function(callback.AsStdFunction(), "test"), Function(callback.AsStdFunction(), "another test"));
    Function::Callback anotherCallback = [](const Knowledge&, const Substitutions&) -> Expression
    {
        return Boolean(false);
    };
    EXPECT_FALSE(Function(callback.AsStdFunction(), "test") == Function(anotherCallback, "test"));
    EXPECT_THROW(Function(nullptr, "unhappy test"), std::invalid_argument);
    EXPECT_THROW(Function(callback.AsStdFunction(), ""), std::invalid_argument);
}

TEST(Function, Callback)
{
    MockFunction<Function::CallbackT> callback;
    Knowledge k;
    EXPECT_CALL(callback, Call(Ref(k),_)).WillRepeatedly(Return(Integer(3)));
    EXPECT_EQ(k.Infer(Function(callback.AsStdFunction(), "call me")), Integer(3));
}

TEST(Function, Documentation)
{
    MockFunction<Function::CallbackT> callback;
    EXPECT_EQ(Function(callback.AsStdFunction(), "short\nlong").ShortDescription(), "short");
    EXPECT_EQ(Function(callback.AsStdFunction(), "full\ndocumentation").Documentation(), "full\ndocumentation");
}

TEST(Function, to_string)
{
    MockFunction<Function::CallbackT> callback;
    EXPECT_EQ(to_string(Function(callback.AsStdFunction(), "Test\nThis is a test")), "Test");
}

}

