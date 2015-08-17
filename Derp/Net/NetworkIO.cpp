#include "stdafx.h"
#include <algorithm>
#include <boost/format.hpp>
#include "NetworkIO.h"
#include "MatrixIO.h"

typedef unsigned version;
const version layer_version = 1;
const version connection_version = 1;
const version network_version = 1;
const char seperator = ' ';

std::ostream& operator<< (std::ostream& stream, const Layer& layer)
{
	stream << layer_version << seperator;
	stream << layer.units << std::endl;
	stream << layer.bias.transpose() << std::endl;
	return stream;
}

std::istream& operator>> (std::istream& stream, Layer& layer)
{
	version v;
	stream >> v;
	if (v >= 1)
	{
		stream >> layer.units;
		layer.bias = Eigen::VectorXd(layer.Size());
		stream >> layer.bias;
	}

	return stream;
}

std::ostream& operator<< (std::ostream& stream, const Connection& connection)
{
	stream << connection_version << std::endl;
	stream << connection.weights;

	return stream;
}


std::istream& operator>> (std::istream& stream, Connection& connection)
{
	version v;
	stream >> v;
	if (v >= 1)
	{
		connection.weights.resize(connection.a.Size(), connection.b.Size());
		stream >> connection.weights;
	}

	return stream;
}

template<typename T>
bool CompareUniquePtr(const std::unique_ptr<T>& ptr, const T& ref)
{
	return ptr.get() == &ref;
}

template<typename T> std::string classname(const T& obj)
{
	std::string fullName = std::string(typeid(obj).name());
	assert(fullName.substr(0, 6) == "class ");
	return fullName.substr(6, std::string::npos);
}

std::ostream& operator<< (std::ostream& stream, const Network& network)
{
	stream << network_version << seperator;
	stream << network.layers.size() << seperator << network.connections.size() << std::endl;
	for (auto layerIt = network.layers.begin(); layerIt != network.layers.end(); ++layerIt)
	{
		const Layer& layer = *layerIt->get();
		stream << layerIt - network.layers.begin() << seperator << classname(layer) << seperator;
		stream << layer;
	}
	for (auto connectionIt = network.connections.begin(); connectionIt != network.connections.end(); ++connectionIt)
	{
		const Connection& connection = *connectionIt->get();
		unsigned a = std::find_if(network.layers.begin(), network.layers.end(), [connection](const std::unique_ptr<Layer>& ptr){ return CompareUniquePtr(ptr, connection.A()); }) - network.layers.begin();
		unsigned b = std::find_if(network.layers.begin(), network.layers.end(), [connection](const std::unique_ptr<Layer>& ptr){ return CompareUniquePtr(ptr, connection.B()); }) - network.layers.begin();
		stream << classname(connection) << seperator << a << seperator << b << seperator ;
		stream << connection;
	}
	return stream;
}


std::istream& operator>> (std::istream& stream, Network& network)
{
	version v;
	stream >> v;
	if (v >= 1)
	{
		size_t layerCount, connectionCount;
		stream >> layerCount >> connectionCount;
		for (size_t layerIt = 0; layerIt < layerCount; ++layerIt)
		{
			unsigned layerIndex;
			std::string layerType;
			stream >> layerIndex >> layerType;
			// todo: layer factory
			if (layerType == "InputLayer")
			{
				network.layers.emplace_back(std::make_unique<InputLayer>());
			}
			else if (layerType == "HiddenLayer")
			{
				network.layers.emplace_back(std::make_unique<HiddenLayer>());
			}
			else
			{
				throw std::ios_base::failure((boost::format("Syntax error,  layer[%d].type = '%s' unknown.") % layerIt % layerType).str());
			}
			Layer& layer = *network.layers.back().get();
			stream >> layer;
		}
		for (size_t connectionIt = 0; connectionIt < connectionCount; ++connectionIt)
		{
			unsigned connectionIndex, aIndex, bIndex;
			std::string connectionType;
			stream >> connectionIndex >> connectionType >> aIndex >> bIndex;
			if (aIndex >= layerCount)
				throw std::out_of_range((boost::format("Connection [%d].A = %d out of range.") % connectionIndex % aIndex).str());

			if (bIndex >= layerCount)
				throw std::out_of_range((boost::format("Connection [%d].B = %d out of range.") % connectionIndex % bIndex).str());

			if (connectionType == "Connection")
			{
				network.connections.emplace_back(std::make_unique<Connection>(*network.layers[aIndex].get(), *network.layers[bIndex].get()));
			}
			else
			{
				throw std::ios_base::failure((boost::format("Syntax error,  connection[%d].Type = '%s' unknown.") % connectionIndex % connectionType).str());
			}
			Connection& connection = *network.connections.back().get();
			stream >> connection;
		}
	}
	return stream;
}

