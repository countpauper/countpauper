#include "stdafx.h"
#include <boost/test/unit_test.hpp>
#include "../Net/MatrixIO.h"

BOOST_AUTO_TEST_SUITE(MatrixIO);

BOOST_AUTO_TEST_CASE(one_by_one_double)
{
	Eigen::MatrixXd matrix(1, 1);
	std::stringstream stream;
	stream << 1.5  << std::endl;
	stream >> matrix;
	BOOST_REQUIRE_EQUAL(matrix.rows(), 1);
	BOOST_REQUIRE_EQUAL(matrix.cols(), 1);
	BOOST_REQUIRE_EQUAL(matrix.coeff(0, 0), 1.5);
}

BOOST_AUTO_TEST_CASE(two_by_four_int)
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


BOOST_AUTO_TEST_CASE(vector_float)
{
	Eigen::VectorXf vector(2);
	std::stringstream stream;
	stream << -0.3f << " " << 0.9f  << std::endl;
	stream >> vector;
	BOOST_REQUIRE_EQUAL(vector.size(), 2);
	BOOST_REQUIRE_EQUAL(vector.coeff(0), -0.3f);
	BOOST_REQUIRE_EQUAL(vector.coeff(1), 0.9f);
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