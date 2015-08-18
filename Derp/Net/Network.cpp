#include "stdafx.h"
#include "Network.h"
#include <boost/random.hpp>
#include <boost/random/normal_distribution.hpp>

namespace Net
{
	double normalized_noise(double mean)
	{
		static boost::mt19937 rng;
		static boost::normal_distribution<> nd(mean, 1.0);
		return nd(rng);
	}

	double normal_noise(double mean, double sigma)
	{
		static boost::mt19937 rng;
		static boost::normal_distribution<> nd(mean, sigma);
		return nd(rng);
	}

	Layer::Layer() :
		units(0)
	{
	}

	Layer::Layer(size_t units, std::unique_ptr<Function>&& function) :
		units(units),
		bias(units),
		function(std::move(function))
	{
	}

	Layer::~Layer()
	{
	}

	void Layer::Connect(Connection& connection)
	{
		connections.push_back(&connection);
	}

	size_t Layer::Size() const
	{
		return units;
	}

	void Layer::Reset(double mean, double sigma)
	{
		bias.setConstant(mean);
		if (sigma != 0)
		{
			Eigen::VectorXd sigmaVector(bias.size());
			sigmaVector.setConstant(sigma);
			bias = bias.binaryExpr(sigmaVector, std::ptr_fun(normal_noise));
		}
	}

	InputLayer::InputLayer()
	{
	}

	InputLayer::InputLayer(size_t units, std::unique_ptr<Function>&& function) :
		Layer(units, std::move(function))
	{
	}


	HiddenLayer::HiddenLayer()
	{

	}

	HiddenLayer::HiddenLayer(size_t units, std::unique_ptr<Function>&& function) :
		Layer(units, std::move(function))
	{
	}


	Connection::Connection(Layer& a, const Layer& b) :
		a(a),
		b(b),
		weights(a.Size(), b.Size())
	{
		a.Connect(*this);
	}

	Connection::~Connection()
	{
	}

	void Connection::Reset(double mean, double sigma)
	{
		weights.setConstant(mean);
		if (sigma != 0)
		{
			Eigen::MatrixXd sigMatrix(weights.rows(), weights.cols());
			sigMatrix.setConstant(sigma);
			weights = weights.binaryExpr(sigMatrix, std::ptr_fun(normal_noise));
		}
	}

	InputLayer& Network::Add(size_t units, std::unique_ptr<Function>&& function)
	{
		layers.emplace_back(std::make_unique<InputLayer>(units, std::move(function)));
		return *static_cast<InputLayer*>(layers.back().get());
	}

	HiddenLayer& Network::Add(Layer& connected, size_t units, std::unique_ptr<Function>&& function)
	{
		layers.emplace_back(std::make_unique<HiddenLayer>(units, std::move(function)));
		connections.emplace_back(std::make_unique<Connection>(connected, *layers.back().get()));
		connected.Connect(*connections.back().get());
		return static_cast<HiddenLayer&>(*layers.back().get());
	}

	const Network::Layers& Network::GetLayers() const
	{
		return layers;
	}

	void Network::Reset(double mean, double sigma)
	{
		for (auto& layer : layers)
			layer->Reset(mean, sigma);
		for (auto& connection : connections)
			connection->Reset(mean, sigma);
	}

	State Network::Activate(const Sample& sample)
	{
		return State(*this, sample);
	}

}
