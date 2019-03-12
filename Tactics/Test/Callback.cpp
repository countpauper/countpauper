#include <boost/test/unit_test.hpp>
#include <array>
#include <numeric>
#include <iostream>
#include "Callback.h"

BOOST_AUTO_TEST_SUITE(Callback);

BOOST_AUTO_TEST_CASE(CallbackVoid)
{
	int count = 0;
	Engine::Callback c;
	c();
	BOOST_CHECK_EQUAL(count, 0);

	auto connection = c.connect([&count]() {count++; });
	c();
	BOOST_CHECK_EQUAL(count, 1);
	c();
	BOOST_CHECK_EQUAL(count, 2);
	connection.disconnect();
	c();
	BOOST_CHECK_EQUAL(count, 2);
}

BOOST_AUTO_TEST_CASE(ConnectionScope)
{
	int count = 0;
	Engine::Callback c;
	{
		auto connection = c.connect([&count]() {count++; });
		c();
		BOOST_CHECK_EQUAL(count, 1);
	}
	c();
	BOOST_CHECK_EQUAL(count, 1);
}

BOOST_AUTO_TEST_CASE(CallbackArguments)
{
	int argument = 0;
	Engine::Callback<int> c;
	c(3);
	BOOST_CHECK_EQUAL(argument, 0);

	auto connection = c.connect([&argument](int value) {argument = value; });
	c(1);
	BOOST_CHECK_EQUAL(argument, 1);
	c(2);
	BOOST_CHECK_EQUAL(argument, 2);
	connection.disconnect();
	c(4);
	BOOST_CHECK_EQUAL(argument, 2);
}

BOOST_AUTO_TEST_CASE(CallbackException)
{
	Engine::Callback c;
	auto connection = c.connect([]() 
	{ 
		throw std::runtime_error("Test"); 
	});
	BOOST_CHECK_THROW(c(), std::runtime_error);
	connection.disconnect();
	c();
}



BOOST_AUTO_TEST_SUITE_END()