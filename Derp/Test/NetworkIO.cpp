#include "stdafx.h"
#include <boost/test/unit_test.hpp>
#include <strstream>

#include "../Net/NetworkIO.h"
#include <Eigen/Dense>

BOOST_AUTO_TEST_SUITE(NetworkIO);

BOOST_AUTO_TEST_CASE(writeEmpty)
{
	Network net;
	std::stringstream stream;
	stream << net;
	BOOST_REQUIRE(!stream.bad());
	BOOST_REQUIRE_EQUAL(stream.str(), "1 0 0\n");
}

BOOST_AUTO_TEST_CASE(readEmpty)
{
	Network net;
	std::stringstream stream("1 0 0\n");
	stream >> net;
	BOOST_REQUIRE(!stream.bad());
	BOOST_REQUIRE_EQUAL(net.GetLayers().size(), 0);
}

BOOST_AUTO_TEST_CASE(writeLayer)
{
	Network net;
	net.Add(3);

	std::stringstream stream;

	stream << net;
	BOOST_REQUIRE(!stream.bad());
	BOOST_REQUIRE_EQUAL(stream.str(), "1 1 0\n0 InputLayer 1 3\n0 0 0\n");
}

BOOST_AUTO_TEST_CASE(readLayer)
{
	Network net;
	std::stringstream stream("1 1 0\n0 InputLayer 1 2\n0.1 -0.2\n");
	stream >> net;
	BOOST_REQUIRE(!stream.bad());
	BOOST_REQUIRE_EQUAL(net.GetLayers().size(), 1);
	BOOST_REQUIRE_EQUAL(net.GetLayers().front()->Size(), 2);
	Eigen::Vector2d refBias;
	refBias << 0.1, -0.2;
	BOOST_REQUIRE_EQUAL(net.GetLayers().front()->Bias(), refBias);
}


BOOST_AUTO_TEST_SUITE_END()