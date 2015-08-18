#include "stdafx.h"
#include <boost/test/unit_test.hpp>
#include <strstream>
#include <Eigen/Dense>
#include "../Net/NetworkIO.h"
#include "../Net/Sample.h"

BOOST_AUTO_TEST_SUITE(Activation);

BOOST_AUTO_TEST_CASE(Input)
{
	Net::Network net;
	net.Add(2, nullptr);
	net.Reset();
	Eigen::Vector2d sampleData;
	sampleData << 1, 2;
	Net::State output = net.Activate(Net::Sample(sampleData));
}

BOOST_AUTO_TEST_SUITE_END()