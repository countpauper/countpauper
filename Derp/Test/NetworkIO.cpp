#include "stdafx.h"
#include <boost/test/unit_test.hpp>
#include <strstream>
#include <Eigen/Dense>
#include "../Net/NetworkIO.h"

BOOST_AUTO_TEST_SUITE(NetworkIO);

BOOST_AUTO_TEST_CASE(WriteEmpty)
{
	Net::Network net;
	net.Reset();

	std::stringstream stream;
	stream << net;
	BOOST_REQUIRE(!stream.bad());
	BOOST_CHECK_EQUAL(stream.str(), "1 0 0\n");
}

BOOST_AUTO_TEST_CASE(ReadEmpty)
{
	Net::Network net;
	std::stringstream stream("1 0 0\n");
	stream >> net;
	BOOST_REQUIRE(!stream.bad());
	BOOST_REQUIRE(!stream.eof());
	BOOST_CHECK_EQUAL(net.GetLayers().size(), 0);
}

BOOST_AUTO_TEST_CASE(WriteLayer)
{
	Net::Network net;
	net.Add(3, std::make_unique<Net::Linear>());
	net.Reset();

	std::stringstream stream;

	stream << net;
	BOOST_REQUIRE(!stream.bad());
	BOOST_CHECK_EQUAL(stream.str(), "1 1 0\n0 Net::InputLayer 1 3 Net::Linear\n0 0 0\n");
}

BOOST_AUTO_TEST_CASE(ReadLayer)
{
	Net::Network net;
	std::stringstream stream("1 1 0\n0 Net::InputLayer 1 2 Net::Linear\n0.1 -0.2\n");
	stream >> net;
	BOOST_REQUIRE(!stream.bad());
	BOOST_REQUIRE(!stream.eof());
	BOOST_REQUIRE_EQUAL(net.GetLayers().size(), 1);
	BOOST_REQUIRE_EQUAL(net.GetLayers().front()->Size(), 2);
	Eigen::Vector2d refBias;
	refBias << 0.1, -0.2;
	BOOST_CHECK_EQUAL(net.GetLayers().front()->Bias(), refBias);
	BOOST_CHECK(dynamic_cast<Net::InputLayer*>(net.GetLayers()[0].get()));
	BOOST_CHECK(dynamic_cast<const Net::Linear*>(net.GetLayers()[0]->GetFunction()));
}

BOOST_AUTO_TEST_CASE(WriteConnection)
{
	Net::Network net;
	net.Add(net.Add(3, std::make_unique<Net::Boolean>()), 2, std::make_unique<Net::Sigmoid>());
	net.Reset();

	std::stringstream stream;
	stream << net;
	BOOST_REQUIRE(!stream.bad());
	BOOST_CHECK_EQUAL(stream.str(), "1 2 1\n0 Net::InputLayer 1 3 Net::Boolean\n0 0 0\n1 Net::HiddenLayer 1 2 Net::Sigmoid\n0 0\n0 Net::Connection 0 1 1\n0 0\n0 0\n0 0\n");
}

BOOST_AUTO_TEST_CASE(ReadConnection)
{
	Net::Network net;
	std::stringstream stream("1 2 1\n0 Net::InputLayer 1 2 Net::Boolean\n0.2 -0.3\n1 Net::HiddenLayer 1 2 Net::Sigmoid\n-0.5 0.9\n0 Net::Connection 0 1 1\n0 0\n0 0\n");
	stream >> net;
	BOOST_REQUIRE(!stream.bad());
	BOOST_REQUIRE(!stream.eof());
	BOOST_CHECK_EQUAL(net.GetLayers().size(), 2);
	Eigen::Vector2d refBias;
	refBias << 0.2, -0.3;
	BOOST_CHECK_EQUAL(net.GetLayers()[0]->Bias(), refBias);
	BOOST_CHECK(dynamic_cast<Net::InputLayer*>(net.GetLayers()[0].get()));
	BOOST_CHECK(dynamic_cast<const Net::Boolean*>(net.GetLayers()[0]->GetFunction()));
	refBias << -0.5, 0.9;
	BOOST_CHECK_EQUAL(net.GetLayers()[1]->Bias(), refBias);
	BOOST_CHECK(dynamic_cast<Net::HiddenLayer*>(net.GetLayers()[1].get()));
	BOOST_CHECK(dynamic_cast<const Net::Sigmoid*>(net.GetLayers()[1]->GetFunction()));
	const auto& weights = net.GetLayers()[0]->GetConnections()[0]->GetWeights();
	BOOST_CHECK_EQUAL(weights, Eigen::Matrix2d().Zero());
}

BOOST_AUTO_TEST_CASE(ConnectionIndexException)
{
	Net::Network net;
	std::stringstream stream1("1 1 1\n0 Net::InputLayer 1 0 Net::Linear\n\n0 Net::Connection 1 0 1\n\n");
	BOOST_CHECK_THROW(stream1 >> net, std::out_of_range);
	std::stringstream stream2("1 1 1\n0 Net::InputLayer 1 0 Net::Linear\n\n0 Net::Connection 0 1 1\n\n");
	BOOST_CHECK_THROW(stream2 >> net, std::out_of_range);
}

BOOST_AUTO_TEST_CASE(LayerTypeException)
{
	Net::Network net;
	std::stringstream stream("1 1 0\n0 Bogus 1 0 Linear\n\n");
	BOOST_CHECK_THROW(stream >> net, std::domain_error);
}

BOOST_AUTO_TEST_CASE(ConnectionTypeException)
{
	Net::Network net;
	std::stringstream stream("1 1 1\n0 Net::InputLayer 1 0 Net::Linear\n\n0 Bogus 0 0 0 Net::Linear\n\n");
	BOOST_CHECK_THROW(stream >> net, std::domain_error);
}

BOOST_AUTO_TEST_CASE(FunctionTypeException)
{
	Net::Network net;
	std::stringstream stream("1 1 0\n0 Net::InputLayer 1 0 Bogus\n\n");
	BOOST_CHECK_THROW(stream >> net, std::domain_error);
}
BOOST_AUTO_TEST_SUITE_END()