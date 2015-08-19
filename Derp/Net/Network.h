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
		Layer(size_t units, const Function& function);
		virtual ~Layer() = default;
		void Connect(Connection::Base& connection);
		size_t Size() const;
		const Eigen::VectorXd& Bias() const { return bias; }
		const Connections& GetConnections() const { return connections; }
		const Connection::Base& operator[](unsigned index) const { return *connections[index]; }
		Connection::Base& operator[](unsigned index) { return *connections[index]; }
		void Reset(double mean, double sigma);
		const Function& GetFunction() const { return *function.get(); }
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
		InputLayer() = default;
		InputLayer(size_t units);
	};

	class OutputLayer : public Layer
	{
	public:
		OutputLayer() = default;
		OutputLayer(size_t units, const Function& function);
	};

	class VisibleLayer : public Layer
	{
	public:
		VisibleLayer() = default;
		VisibleLayer(size_t units, const Function& function);
	};

	class HiddenLayer : public Layer
	{
	public:
		HiddenLayer() = default;
		HiddenLayer(size_t units, const Function& function);

	};

	class Network
	{
	public:
		typedef std::vector<std::unique_ptr<Layer>> Layers;
		typedef std::vector<std::unique_ptr<Connection::Base>> Connections;

		const Layers& GetLayers() const;
		const Layer& operator[](unsigned index) const { return *layers[index].get(); }
		Layer& operator[](unsigned index) { return *layers[index].get(); }
		Eigen::VectorXd Compute(const Sample& sample);
		InputLayer& Input(size_t units);
		OutputLayer& Output(size_t units, const Function& function);
		VisibleLayer& Visible(size_t units, const Function& function);
		HiddenLayer& Hidden(size_t units, const Function& function);
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
