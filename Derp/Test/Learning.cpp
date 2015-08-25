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
	rbm.OutputProbability();
	rbm.hidden.ChangeFunction(Net::Sigmoid());
	Net::Learning::ContrastiveDivergence cd1(rbm);
	Net::Data::Set data;
	std::stringstream("1 1\n1 1 0\n1 0 1 1") >> data;
	for (unsigned i = 0; i < 1000; ++i)
	{
		double p0 = rbm(data[0].inputs)[0].activation.coeff(0);
		cd1.Train(data);
		double p1 = rbm(data[0].inputs)[0].activation.coeff(0);
		BOOST_REQUIRE_GT(p1, p0);
		std::stringstream msg;
		msg << "p1 = " << p1;
//		BOOST_MESSAGE(msg.str());
	}
}

BOOST_AUTO_TEST_CASE(CD1_2)
{
	Net::RBM rbm(2, 1);
	rbm.Reset();
	rbm.OutputProbability();
	rbm.hidden.ChangeFunction(Net::Sigmoid());
	Net::Learning::ContrastiveDivergence cd1(rbm);
	Net::Data::Set data;
	std::stringstream("1 1\n1 1 0\n1 0 2 0 1") >> data;
	for (unsigned i = 0; i < 1000; ++i)
	{
		double p0_0 = rbm(data[0].inputs)[0].activation.coeff(0);
		double p0_1 = rbm(data[0].inputs)[0].activation.coeff(1);
		cd1.Train(data);
		double p1_0 = rbm(data[0].inputs)[0].activation.coeff(0);
		double p1_1 = rbm(data[0].inputs)[0].activation.coeff(1);
		BOOST_REQUIRE_GT(p1_1, p0_1);
		BOOST_REQUIRE_LT(p1_0, p0_0);
		std::stringstream msg;
		msg << "p1(0) = " << p1_0 << " p1(1) "<< p1_1;
		BOOST_MESSAGE(msg.str());
	}
}

BOOST_AUTO_TEST_SUITE_END()