#include "stdafx.h"
#include <boost/test/unit_test.hpp>
#include <strstream>
#include <Eigen/Dense>
#include "../Net/IO.h"
#include "../Net/RBM.h"
#include "../Net/Net.h"

BOOST_AUTO_TEST_SUITE(Learning);

BOOST_AUTO_TEST_CASE(CD1)
{
	Net::RBM rbm(1, 1);
	rbm.Reset();
	Net::Learning::ContrastiveDivergence cd1(rbm);
	Net::Data::Set data;
	std::stringstream("1 1\n1 1 0\n1 0 1 1") >> data;
	for (unsigned i = 0; i < 1000; ++i)
	{
		Eigen::VectorXd p0 = rbm.ComputeProbability(data[0].inputs[0]).activation;
		cd1.Train(data);
		Eigen::VectorXd p1 = rbm.ComputeProbability(data[0].inputs[0]).activation;
		BOOST_REQUIRE_GT(p1[0], p0[0]);
		std::stringstream msg;
		msg << "p1 = " << p1[0];
		BOOST_MESSAGE(msg.str());
	}
}

BOOST_AUTO_TEST_CASE(CD1_2)
{
	Net::RBM rbm(2, 1);
	rbm.Reset();
	Net::Learning::ContrastiveDivergence cd1(rbm);
	Net::Data::Set data;
	std::stringstream("1 1\n1 1 0\n1 0 2 0 1") >> data;
	for (unsigned i = 0; i < 1000; ++i)
	{
		Eigen::VectorXd p0 = rbm.ComputeProbability(data[0].inputs[0]).activation;

		cd1.Train(data);

		Eigen::VectorXd p1 = rbm.ComputeProbability(data[0].inputs[0]).activation;

		BOOST_REQUIRE_LT(p1[0], p0[0]);
		BOOST_REQUIRE_GT(p1[1], p0[1]);
		std::stringstream msg;
		msg << "p1[0] = " << p1[0] << " p1(1) "<< p1[1];
		BOOST_MESSAGE(msg.str());
	}
}

BOOST_AUTO_TEST_SUITE_END()