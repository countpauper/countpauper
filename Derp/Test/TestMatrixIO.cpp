#include "stdafx.h"

#define BOOST_TEST_MODULE example
#include <boost/test/included/unit_test.hpp>
#include <strstream>

#include "../Net/MatrixIO.h"
#include <Eigen/Dense>

BOOST_AUTO_TEST_SUITE(MatrixIO);

BOOST_AUTO_TEST_CASE(1by1_double)
{
	Eigen::MatrixXd matrix(1, 1);
	std::stringstream stream;
	stream << 1.5  << std::endl;
	stream >> matrix;
	BOOST_REQUIRE_EQUAL(matrix.rows(), 1);
	BOOST_REQUIRE_EQUAL(matrix.cols(), 1);
	BOOST_REQUIRE_EQUAL(matrix.coeff(0, 0), 1.5);
}

BOOST_AUTO_TEST_CASE(2by4_int)
{
	Eigen::MatrixXi matrix(2, 4);
	std::stringstream stream;
	stream << 1 << " " << 2 << " " << 3 << " " << 4 << std::endl << 5 << " " << 6 << " " << 7 << " " << 8 << std::endl;
	stream >> matrix;
	BOOST_REQUIRE_EQUAL(matrix.rows(), 2);
	BOOST_REQUIRE_EQUAL(matrix.cols(), 4);
	BOOST_REQUIRE_EQUAL(matrix.coeff(0, 0), 1);
	BOOST_REQUIRE_EQUAL(matrix.coeff(0, 2), 3);
	BOOST_REQUIRE_EQUAL(matrix.coeff(1, 1), 6);
	BOOST_REQUIRE_EQUAL(matrix.coeff(1, 3), 8);
}


BOOST_AUTO_TEST_CASE(inf)
{
	Eigen::MatrixXd matrix(2, 1);
	std::stringstream stream;
	stream << std::numeric_limits<double>::infinity() << " " << -std::numeric_limits<double>::infinity() << std::endl;
	stream >> matrix;
	BOOST_REQUIRE_EQUAL(matrix.coeff(0, 0), std::numeric_limits<double>::infinity());
	BOOST_REQUIRE_EQUAL(matrix.coeff(1, 0), -std::numeric_limits<double>::infinity());
}

BOOST_AUTO_TEST_CASE(nan)
{
	Eigen::MatrixXd matrix(1, 2);
	std::stringstream stream;
	stream << std::numeric_limits<double>::quiet_NaN() << " " << std::numeric_limits<double>::signaling_NaN() << std::endl;
	stream >> matrix;
	BOOST_REQUIRE(std::isnan(matrix.coeff(0, 0)));
	BOOST_REQUIRE(std::isnan(matrix.coeff(0, 1)));
}


BOOST_AUTO_TEST_SUITE_END()