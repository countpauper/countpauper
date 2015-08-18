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
	BOOST_CHECK_EQUAL(output.GetActivation(*net.GetLayers()[0].get()), sampleData);
}

BOOST_AUTO_TEST_CASE(Propagate)
{
	Net::Network net;
	net.Add(net.Add(2, nullptr),1,std::make_unique<Net::Linear>());
	net.GetLayers()[0]->GetConnections()[0]->Reset(1, 0);
	Eigen::Vector2d sampleData;
	sampleData << 1, 2;
	Net::State output = net.Activate(Net::Sample(sampleData));
	BOOST_CHECK_EQUAL(output.GetActivation(*net.GetLayers()[1].get()), Eigen::VectorXd::Constant(1,3));
}

BOOST_AUTO_TEST_SUITE_END()