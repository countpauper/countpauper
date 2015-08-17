#include "stdafx.h"
#include "RBM.h"
#include <boost/random.hpp>
#include <boost/random/normal_distribution.hpp>


double normalized_noise(double mean)
{
  static boost::mt19937 rng;
  static boost::normal_distribution<> nd(mean,1.0);
  return nd(rng);
}

double normal_noise(double mean, double sigma)
{
  static boost::mt19937 rng;
  static boost::normal_distribution<> nd(mean, sigma);
  return nd(rng);
}

Layer::Layer(size_t units) :
	units(units)
{
}

Layer::~Layer()
{
}

Layer& Layer::Connect(std::unique_ptr<Layer> layer)
{
	connections.emplace_back(std::move(std::make_unique<Connection>(*this, std::move(layer))));
	return *connections.back()->Target();

}

size_t Layer::Size() const
{
	return units;
}

InputLayer::InputLayer(size_t units) :
	Layer(units)
{
}


HiddenLayer::HiddenLayer(size_t units) :
	Layer(units)
{
}


Connection::Connection(const Layer& owner, std::unique_ptr<Layer> target) :
	owner(owner),
	target(std::move(target)),
	weights(Eigen::MatrixXd::Zero(owner.Size(), target->Size()).unaryExpr(std::ptr_fun(normalized_noise)))
{
}

InputLayer& Network::Add(size_t units)
{
	inputs.emplace_back(std::make_unique<InputLayer>(units));
	return *static_cast<InputLayer*>(inputs.back().get());
}

HiddenLayer& Network::Add(Layer& connected, size_t units)
{
	return static_cast<HiddenLayer&>(connected.Connect(std::make_unique<HiddenLayer>(units)));
}

const Network::InputLayers& Network::GetInputs() const 
{ 
	return inputs;  
}

NetworkState Network::Activate(const Sample& sample)
{
	NetworkState result(*this);
	result.Activate(sample);
	return result;
}


NetworkState::NetworkState(const Network& network)
{
	for (const auto& layer : network.GetInputs())
	{
		if (InputLayer* l = dynamic_cast<InputLayer*>(layer.get()))
			inputLayers.emplace_back(LayerState(*l));
	}
}

//TODO: activation state only exists after activation, only keep reference on creation, start recursively creating activation states on activate

void NetworkState::Activate(const Sample& sample)
{
	size_t pos = 0;
	for (auto& layerState : inputLayers)
	{
		const InputLayer& input = *static_cast<const InputLayer*>(&layerState.GetLayer());
		layerState.Activate(sample.Activation(pos, input.Size()));
		pos += input.Size();
	}
}

RBM::RBM(size_t visible, size_t hidden) :
	visible(Add(visible))
{
	Add(this->visible, hidden);
}

Eigen::VectorXd Sample::Activation(size_t from, size_t count) const
{
	return activation.segment(from, count);
}
