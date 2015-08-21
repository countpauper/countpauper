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
	Net::RBM rbm(1,1);
	Net::Learning::ContrastiveDivergence cd1(rbm);
	
}

BOOST_AUTO_TEST_SUITE_END()