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
	net.Visible(2, Net::Linear());
	net.Reset();
	Eigen::Vector2d sampleData;
	sampleData << 1, 2;
	Eigen::VectorXd output = net.Compute(Net::Sample(sampleData));
	BOOST_CHECK_EQUAL(output, sampleData);
}

BOOST_AUTO_TEST_CASE(Compute)
{
	Net::Network net;
	Net::Layer& input = net.Input(2);
	Net::Layer& output = net.Output(1, Net::Linear());
	net.Directed(input, output);
	input[0].Reset(1, 0);
	Eigen::Vector2d sampleData;
	sampleData << 1, 2;
	Eigen::VectorXd result = net.Compute(Net::Sample(sampleData));
	BOOST_CHECK_EQUAL(result, Eigen::VectorXd::Constant(1, 3));
}

BOOST_AUTO_TEST_SUITE_END()