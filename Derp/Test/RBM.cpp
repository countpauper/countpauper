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

	Net::Data::Sample sample;
	std::stringstream stream("1 1 0\n0 1 1 1\n");
	stream >> sample;
	rbm.SetProbabilistic();
	// doesnt matter what is put in, with 0 weights nothing comes out of hidden and 0 bias, always p=0.5
	BOOST_CHECK_EQUAL(rbm(sample.inputs)[0].activation.coeff(0), 0.5);
}

BOOST_AUTO_TEST_SUITE_END()

