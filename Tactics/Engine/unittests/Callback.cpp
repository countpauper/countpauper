#include <gtest/gtest.h>
#include <array>
#include <numeric>
#include <iostream>
#include "Utility/Callback.h"

namespace Engine::Test
{

TEST(Callback, CallbackVoid)
{
	int count = 0;
	Engine::Callback<> c;
	c();
	EXPECT_EQ(count, 0);

	auto connection = c.connect([&count]() {count++; });
	c();
	EXPECT_EQ(count, 1);
	c();
	EXPECT_EQ(count, 2);
	connection.disconnect();
	c();
	EXPECT_EQ(count, 2);
}

TEST(Callback, ConnectionScope)
{
	int count = 0;
	Engine::Callback<> c;
	{
		auto connection = c.connect([&count]() {count++; });
		c();
		EXPECT_EQ(count, 1);
	}
	c();
	EXPECT_EQ(count, 1);
}

TEST(Callback, CallbackArguments)
{
	int argument = 0;
	Engine::Callback<int> c;
	c(3);
	EXPECT_EQ(argument, 0);

	auto connection = c.connect([&argument](int value) {argument = value; });
	c(1);
	EXPECT_EQ(argument, 1);
	c(2);
	EXPECT_EQ(argument, 2);
	connection.disconnect();
	c(4);
	EXPECT_EQ(argument, 2);
}

TEST(Callback, CallbackException)
{
	Engine::Callback<> c;
	auto connection = c.connect([]()
	{
		throw std::runtime_error("Test");
	});
	EXPECT_THROW(c(), std::runtime_error);
	connection.disconnect();
	c();
}

}
