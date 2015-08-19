#include "stdafx.h"
#include <boost/test/unit_test.hpp>
#include <strstream>
#include <Eigen/Dense>
#include "../Net/NetworkIO.h"
#include "../Net/Sample.h"
#include "../Net/Connection.h"

BOOST_AUTO_TEST_SUITE(Activation);

BOOST_AUTO_TEST_CASE(Input)
{
	Net::Network net;
	net.Input(2, nullptr);
	net.Reset();
	Eigen::Vector2d sampleData;
	sampleData << 1, 2;
	Net::State output = net.Activate(Net::Sample(sampleData));
	BOOST_CHECK_EQUAL(output.GetActivation(net[0]), sampleData);
}

BOOST_AUTO_TEST_CASE(Propagate)
{
	Net::Network net;
	Net::Layer& input = net.Input(2, nullptr);
	Net::Layer& hidden = net.Hidden(1, std::make_unique<Net::Linear>());
	net.Directed(input, hidden);
	input[0].Reset(1, 0);
	Eigen::Vector2d sampleData;
	sampleData << 1, 2;
	Net::State output = net.Activate(Net::Sample(sampleData));
	BOOST_CHECK_EQUAL(output.GetActivation(hidden), Eigen::VectorXd::Constant(1,3));
}

BOOST_AUTO_TEST_SUITE_END()