#include "stdafx.h"
#include "Network.h"
#include "Math.h"
#include "Connection.h"

namespace Net
{
	Layer::Layer() :
		units(0)
	{
	}

	Layer::Layer(size_t units, const Function& function) :
		units(units),
		bias(units),
		function(function.Copy())
	{
	}

	Layer::~Layer()
	{
	}

	void Layer::Connect(Connection::Base& connection)
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
			bias = bias.binaryExpr(Eigen::VectorXd::Constant(bias.size(), sigma), std::ptr_fun(normal_noise));
		}
	}

	InputLayer::InputLayer()
	{
	}

	InputLayer::InputLayer(size_t units, const Function& function) :
		Layer(units, function)
	{
	}


	HiddenLayer::HiddenLayer()
	{

	}

	HiddenLayer::HiddenLayer(size_t units, const Function& function) :
		Layer(units, function)
	{
	}

	InputLayer& Network::Input(size_t units, const Function&function)
	{
		layers.emplace_back(std::make_unique<InputLayer>(units, std::move(function)));
		return *static_cast<InputLayer*>(layers.back().get());
	}

	HiddenLayer& Network::Hidden(size_t units, const Function& function)
	{
		layers.emplace_back(std::make_unique<HiddenLayer>(units, std::move(function)));
		return static_cast<HiddenLayer&>(*layers.back().get());
	}

	Connection::Directed& Network::Directed(Layer& a, Layer& b)
	{
		connections.emplace_back(std::make_unique<Connection::Directed>(a, b));
		return *static_cast<Connection::Directed*>(connections.back().get());
	}

	Connection::Undirected& Network::Undirected(Layer& a, Layer& b)
	{
		connections.emplace_back(std::make_unique<Connection::Undirected>(a, b));
		return *static_cast<Connection::Undirected*>(connections.back().get());
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
