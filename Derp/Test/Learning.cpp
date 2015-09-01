#include <boost/test/unit_test.hpp>
#include <strstream>
#include <Eigen/Dense>
#include "../Net/IO.h"
#include "../Net/RBM.h"
#include "../Net/Net.h"

BOOST_AUTO_TEST_SUITE(Learning);

BOOST_AUTO_TEST_CASE(CD1)
{
	Net::BinaryRBM rbm(1, 1);
	rbm.Reset();
	Net::Learning::ContrastiveDivergence cd1(rbm);
	Net::Data::Set data;
	std::stringstream("1 1\n1 1 1\n0 1 1 1\n0 1 1 1") >> data;
	for (unsigned i = 0; i < 1000; ++i)
	{
		Eigen::VectorXd p0 = rbm.ComputeProbability(data[0].inputs[0]).activation;
		cd1.Train(data);
		Eigen::VectorXd p1 = rbm.ComputeProbability(data[0].inputs[0]).activation;
		// BOOST_REQUIRE_GT(p1[0], p0[0]);
		std::stringstream msg;
		msg << "p1 = " << p1[0] << " E=" << rbm.MeanSquaredError(data) << " NRG=" << rbm.FreeEnergy(data[0].inputs[0]);
		BOOST_MESSAGE(msg.str());
	}
}

BOOST_AUTO_TEST_CASE(CD1_2)
{
	Net::BinaryRBM rbm(2, 1);
	rbm.Reset();
	Net::Learning::ContrastiveDivergence cd1(rbm);
	Net::Data::Set data;
	std::stringstream("1 1\n1 1 1\n0 1 2 0 1\n0 1 2 0 1") >> data;
	for (unsigned i = 0; i < 1000; ++i)
	{
		Eigen::VectorXd p0 = rbm.ComputeProbability(data[0].inputs[0]).activation;

		cd1.Train(data);

		Eigen::VectorXd p1 = rbm.ComputeProbability(data[0].inputs[0]).activation;

		// BOOST_REQUIRE_LT(p1[0], p0[0]);
		// BOOST_REQUIRE_GT(p1[1], p0[1]);
		std::stringstream msg;
		msg << "p1[0] = " << p1[0] << " p1(1) " << p1[1] << " E=" << rbm.MeanSquaredError(data) << " NRG=" << rbm.FreeEnergy(data[0].inputs[0]);
		BOOST_MESSAGE(msg.str());
	}
}

BOOST_AUTO_TEST_SUITE_END()