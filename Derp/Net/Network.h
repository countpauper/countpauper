#pragma once

#include <Eigen/Dense>
#include <vector>
#include <memory>

#include "Activation.h"
#include "Function.h"

namespace Net
{
	class Layer;
	namespace Connection
	{
		class Base;
		class Directed;
		class Undirected;
		class Recurrent;
	}
	class Network;
	class Sample;

	class Layer
	{
	public:
		typedef std::vector<Connection::Base*> Connections;
		Layer();
		Layer(size_t units, std::unique_ptr<Function>&& function);
		virtual ~Layer();
		void Connect(Connection::Base& connection);
		size_t Size() const;
		const Eigen::VectorXd& Bias() const { return bias; }
		const Connections& GetConnections() const { return connections; }
		const Connection::Base& operator[](unsigned index) const { return *connections[index]; }
		Connection::Base& operator[](unsigned index) { return *connections[index]; }
		void Reset(double mean, double sigma);
		const Function* GetFunction() const { return function.get(); }
	private:
		size_t units;
		Connections connections;
		std::unique_ptr<Function> function;
		Eigen::VectorXd bias;

		friend std::ostream& operator<< (std::ostream& stream, const Net::Layer& layer);
		friend std::istream& operator>> (std::istream& stream, Net::Layer& layer);
	};

	class InputLayer : public Layer
	{
	public:
		InputLayer();
		InputLayer(size_t units, std::unique_ptr<Function>&& function);
	};

	class HiddenLayer : public Layer
	{
	public:
		HiddenLayer();
		HiddenLayer(size_t units, std::unique_ptr<Function>&& function);

	};

	class Network
	{
	public:
		typedef std::vector<std::unique_ptr<Layer>> Layers;
		typedef std::vector<std::unique_ptr<Connection::Base>> Connections;

		const Layers& GetLayers() const;
		const Layer& operator[](unsigned index) const { return *layers[index].get(); }
		Layer& operator[](unsigned index) { return *layers[index].get(); }
		State Activate(const Sample& sample);
		InputLayer& Input(size_t units, std::unique_ptr<Function>&& function);
		HiddenLayer& Hidden(size_t units, std::unique_ptr<Function>&& function);
		Connection::Directed& Directed(Layer& a, Layer&b);
		Connection::Undirected& Undirected(Layer& a, Layer&b);
		void Reset(double mean = 0, double sigma = 0);
	private:
		Layers layers;
		Connections connections;

		friend std::ostream& operator<< (std::ostream& stream, const Network& network);
		friend std::istream& operator>> (std::istream& stream, Network& network);
	};


}
