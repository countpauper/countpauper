#include "stdafx.h"
#include <boost/test/unit_test.hpp>
#include <strstream>
#include <Eigen/Dense>
#include "../Net/IO.h"
#include "../Net/Net.h"

BOOST_AUTO_TEST_SUITE(Activation);

BOOST_AUTO_TEST_CASE(Input)
{
	Net::Network net;
	net.Visible(2, Net::Linear());
	net.Reset();
	Eigen::Vector2d sampleData;
	sampleData << 1, 2;	// TODO: IO for data
	Net::Data::Inputs input;
	input.push_back(Net::Data::Input(0, sampleData));
	Net::Data::Outputs output = net(input);
	BOOST_CHECK_EQUAL(output[0].activation, sampleData);
}

BOOST_AUTO_TEST_CASE(Compute)
{
	Net::Network net;
	Net::Layer::Base& input = net.Input(2);
	Net::Layer::Base& output = net.Output(1, Net::Linear());
	net.Directed(input, output);
	input[0].Reset(1);
	output.Reset();
	Eigen::Vector2d sampleData;
	sampleData << 1, 2;
	Net::Data::Inputs inputData;
	inputData.push_back(Net::Data::Input(0, sampleData));
	Net::Data::Outputs result = net(inputData);
	BOOST_CHECK_EQUAL(result[0].activation, Eigen::VectorXd::Constant(1, 3));
}

BOOST_AUTO_TEST_SUITE_END()