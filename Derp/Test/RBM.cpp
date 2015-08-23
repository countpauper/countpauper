#include "stdafx.h"
#include <boost/test/unit_test.hpp>
#include <strstream>
#include <Eigen/Dense>
#include "../Net/IO.h"
#include "../Net/RBM.h"
#include "../Net/Net.h"

BOOST_AUTO_TEST_SUITE(RBM);

BOOST_AUTO_TEST_CASE(Reconstruct)
{
	Net::RBM rbm(1, 1);
	rbm.visible[0].Reset(0);
	rbm.visible.Reset(0);
	rbm.hidden.Reset(0);

	Net::Data::Inputs input;
	input.push_back(Net::Data::Input(0, Eigen::VectorXd::Constant(1, 1)));
	rbm.OutputProbability();
	// doesnt matter what is put in, with 0 weights nothing comes out of hidden and 0 bias, always p=0.5
	BOOST_CHECK_EQUAL(rbm(input)[0].activation.coeff(0), 0.5);
}

BOOST_AUTO_TEST_SUITE_END()

