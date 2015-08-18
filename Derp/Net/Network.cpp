#include "stdafx.h"
#include "Network.h"
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

Layer::Layer() :
	units(0)
{
}

Layer::Layer(size_t units) :
	units(units),
	bias(units)
{
	bias.setZero();
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

InputLayer::InputLayer()
{
}

InputLayer::InputLayer(size_t units) :
	Layer(units)
{
}


HiddenLayer::HiddenLayer()
{

}

HiddenLayer::HiddenLayer(size_t units) :
	Layer(units)
{
}


Connection::Connection(Layer& a, const Layer& b) :
	a(a),
	b(b),
	weights(a.Size(), b.Size())
{
	weights.setZero();
	a.Connect(*this);
}

InputLayer& Network::Add(size_t units)
{
	layers.emplace_back(std::make_unique<InputLayer>(units));
	return *static_cast<InputLayer*>(layers.back().get());
}

HiddenLayer& Network::Add(Layer& connected, size_t units)
{
	layers.emplace_back(std::make_unique<HiddenLayer>(units));
	connections.emplace_back(std::make_unique<Connection>(connected, *layers.back().get()));
	connected.Connect(*connections.back().get());
	return static_cast<HiddenLayer&>(*layers.back().get());
}

const Network::Layers& Network::GetLayers() const 
{ 
	return layers;  
}


State Network::Activate(const Sample& sample)
{
	State result(*this);
	result.Activate(sample);
	return result;
}

