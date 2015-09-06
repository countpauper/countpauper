#include <boost/format.hpp>
#include "Network.h"
#include "Math.h"
#include "Connection.h"
#include "Layer.h"
#include "Activation.h"
#include "Function.h"
#include "IO.h"

namespace Net
{
	Layer::Input& Network::Input(size_t units)
	{
		layers.emplace_back(std::make_unique<Layer::Input>(units));
		return *static_cast<Layer::Input*>(layers.back().get());
	}

	Layer::Output& Network::Output(size_t units, const Activation::Function& function)
	{
		layers.emplace_back(std::make_unique<Layer::Output>(units, function));
		return static_cast<Layer::Output&>(*layers.back().get());
	}

	Layer::Visible& Network::Visible(size_t units, const Activation::Function& function)
	{
		layers.emplace_back(std::make_unique<Layer::Visible>(units, function));
		return static_cast<Layer::Visible&>(*layers.back().get());
	}

	Layer::Hidden& Network::Hidden(size_t units, const Activation::Function& function)
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

	size_t Network::size() const
	{
		return layers.size();
	}

	Network::LayerIds Network::GetOutputs() const
	{
		LayerIds outputs;
		Layers::const_iterator it = layers.begin();
		while ((it = std::find_if(it, layers.end(), [](const std::unique_ptr<Layer::Base>& layer){ return layer->IsOutput(); }))!=layers.end())
		{
			outputs.push_back(Layer::Id(it - layers.begin()));
			++it;
		}
		return outputs;
	}

	Network::LayerIds Network::GetInputs() const
	{
		LayerIds outputs;
		Layers::const_iterator it = layers.begin();
		while ((it = std::find_if(it, layers.end(), [](const std::unique_ptr<Layer::Base>& layer){ return layer->IsInput(); })) != layers.end())
		{
			outputs.push_back(Layer::Id(it - layers.begin()));
			++it;
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
		return Activation::Computation(*this, inputs)();
	}

	double Network::MeanSquaredError(const Data::Set& data) const
	{
		double totalE = 0;
		for (const auto& sample : data)
		{
			auto output = Compute(sample.inputs);
			totalE += output.MeanSquaredError(sample.outputs); 
		}
		return totalE;
	}

	const IO::version Network::version = 1;

	std::ostream& operator<< (std::ostream& stream, const Network& network)
	{
		stream << network.version << IO::separator;
		stream << network.layers.size() << IO::separator << network.connections.size() << std::endl;
		for (auto layerIt = network.layers.begin(); layerIt != network.layers.end(); ++layerIt)
		{
			const Layer::Base& layer = *layerIt->get();
			stream << layerIt - network.layers.begin() << IO::separator << IO::classname(layer) << IO::separator;
			stream << layer;
		}
		for (auto connectionIt = network.connections.begin(); connectionIt != network.connections.end(); ++connectionIt)
		{
			const Connection::Base& connection = *connectionIt->get();
			Layer::Id a = std::find_if(network.layers.begin(), network.layers.end(), [connection](const std::unique_ptr<Layer::Base>& ptr){ return IO::CompareUniquePtr(ptr, connection.a); }) - network.layers.begin();
			Layer::Id b = std::find_if(network.layers.begin(), network.layers.end(), [connection](const std::unique_ptr<Layer::Base>& ptr){ return IO::CompareUniquePtr(ptr, connection.b); }) - network.layers.begin();
			stream << connectionIt - network.connections.begin() << IO::separator << IO::classname(connection) << IO::separator << a << IO::separator << b << IO::separator;
			stream << connection;
		}
		return stream;
	}

	std::istream& operator>> (std::istream& stream, Network& network)
	{
		IO::version v;
		stream >> v;
		if (v >= 1)
		{
			size_t layerCount, connectionCount;
			stream >> layerCount >> connectionCount;
			for (size_t layerIt = 0; layerIt < layerCount; ++layerIt)
			{
				Layer::Id id;
				std::string layerType;
				stream >> id >> layerType;
				// todo: layer factory
				if (layerType == "Net::Layer::Input")
					network.layers.emplace_back(std::make_unique<Layer::Input>());
				else if (layerType == "Net::Layer::Output")
					network.layers.emplace_back(std::make_unique<Layer::Output>());
				else if (layerType == "Net::Layer::Visible")
					network.layers.emplace_back(std::make_unique<Layer::Visible>());
				else if (layerType == "Net::Layer::Hidden")
					network.layers.emplace_back(std::make_unique<Layer::Hidden>());
				else
					throw std::domain_error((boost::format("Syntax error,  layer[%d].type = '%s' unknown.") % layerIt % layerType).str());
				Layer::Base& layer = *network.layers.back().get();
				stream >> layer;
			}
			for (size_t connectionIt = 0; connectionIt < connectionCount; ++connectionIt)
			{
				Connection::Id id;
				Layer::Id a, b;
				std::string connectionType;
				stream >> id >> connectionType >> a >> b;
				if (a >= layerCount)
					throw std::out_of_range((boost::format("Connection [%d].A = %d out of range.") % id % a).str());

				if (b >= layerCount)
					throw std::out_of_range((boost::format("Connection [%d].B = %d out of range.") % id % b).str());

				if (connectionType == "Net::Connection::Directed")
					network.connections.emplace_back(std::make_unique<Connection::Directed>(*network.layers[a].get(), *network.layers[b].get()));
				else if (connectionType == "Net::Connection::Undirected")
					network.connections.emplace_back(std::make_unique<Connection::Undirected>(*network.layers[a].get(), *network.layers[b].get()));
				else
					throw std::domain_error((boost::format("Syntax error,  connection[%d].Type = '%s' unknown.") % id % connectionType).str());
				Connection::Base& connection = *network.connections.back().get();
				stream >> connection;
			}
		}
		return stream;
	}

}
