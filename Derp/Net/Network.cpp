#include "stdafx.h"
#include "Network.h"
#include "Math.h"
#include "Connection.h"
#include "Layer.h"

namespace Net
{
	Layer::Input& Network::Input(size_t units)
	{
		layers.emplace_back(std::make_unique<Layer::Input>(units));
		return *static_cast<Layer::Input*>(layers.back().get());
	}

	Layer::Output& Network::Output(size_t units, const Function& function)
	{
		layers.emplace_back(std::make_unique<Layer::Output>(units, function));
		return static_cast<Layer::Output&>(*layers.back().get());
	}

	Layer::Visible& Network::Visible(size_t units, const Function& function)
	{
		layers.emplace_back(std::make_unique<Layer::Visible>(units, function));
		return static_cast<Layer::Visible&>(*layers.back().get());
	}

	Layer::Hidden& Network::Hidden(size_t units, const Function& function)
	{
		layers.emplace_back(std::make_unique<Layer::Hidden>(units, function));
		return static_cast<Layer::Hidden&>(*layers.back().get());
	}

	Connection::Directed& Network::Directed(Layer::Base& a, Layer::Base& b)
	{
		connections.emplace_back(std::make_unique<Connection::Directed>(a, b));
		return *static_cast<Connection::Directed*>(connections.back().get());
	}

	Connection::Undirected& Network::Undirected(Layer::Base& a, Layer::Base& b)
	{
		connections.emplace_back(std::make_unique<Connection::Undirected>(a, b));
		return *static_cast<Connection::Undirected*>(connections.back().get());
	}

	const Network::Layers& Network::GetLayers() const
	{
		return layers;
	}

	Network::LayerIds Network::GetOutputs() const
	{
		LayerIds outputs;
		for (Layers::const_iterator layerIt = layers.begin(); layerIt != layers.end(); ++layerIt)
		{
			if (dynamic_cast<const Layer::Output*>(layerIt->get()) ||
				dynamic_cast<const Layer::Visible*>(layerIt->get()))
				outputs.push_back(layerIt - layers.begin());
		}
		return outputs;
	}

	void Network::Reset(double mean, double sigma)
	{
		for (auto& layer : layers)
			layer->Reset(mean, sigma);
		for (auto& connection : connections)
			connection->Reset(mean, sigma);
	}

	Data::Outputs Network::Compute(const Data::Inputs& inputs) const
	{
		return Computation(*this, inputs)();
	}

}
