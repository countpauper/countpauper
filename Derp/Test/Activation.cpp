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

	std::stringstream stream("1 1 0 1 0 2 1 2");
	Net::Data::Sample sample;
	stream >> sample;

	Net::Data::Outputs output = net(sample.inputs);
	BOOST_CHECK_EQUAL(output[0].activation, sample.inputs[0].activation);
}

BOOST_AUTO_TEST_CASE(Compute)
{
	Net::Network net;
	Net::Layer::Base& input = net.Input(2);
	Net::Layer::Base& output = net.Output(1, Net::Linear());
	net.Directed(input, output);
	input[0].Reset(1);
	output.Reset();

	std::stringstream stream("1 1 1 1 0 2 1 2 1 1 1 3");
	Net::Data::Sample sample;
	stream >> sample;

	Net::Data::Outputs result = net(sample.inputs);
	BOOST_CHECK_EQUAL(result[1].activation, sample.outputs[1].activation);
}

BOOST_AUTO_TEST_SUITE_END()