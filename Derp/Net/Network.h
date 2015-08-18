#pragma once

#include <Eigen/Dense>
#include <vector>
#include <memory>

#include "Activation.h"
#include "Function.h"

namespace Net
{
	class Layer;
	class Connection;
	class Network;
	class Sample;

	class Layer
	{
	public:
		typedef std::vector<Connection*> Connections;
		Layer();
		Layer(size_t units, std::unique_ptr<Function>&& function);
		virtual ~Layer();
		void Connect(Connection& connection);
		size_t Size() const;
		const Eigen::VectorXd& Bias() const { return bias; }
		const Connections& GetConnections() const { return connections; }
		const Connection& operator[](unsigned index) const { return *connections[index]; }
		Connection& operator[](unsigned index) { return *connections[index]; }
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

	class Connection
	{
	public:
		Connection(Layer& a, const Layer&  b);
		virtual ~Connection();
		const Layer& A() const { return a; }
		const Layer& B() const { return b; }
		const Eigen::MatrixXd& GetWeights() const { return weights; }
		void Reset(double mean, double sigma);
	private:
		const Layer& a;
		const Layer& b;
		Eigen::MatrixXd weights;

		friend std::ostream& operator<< (std::ostream& stream, const Connection& connection);
		friend std::istream& operator>> (std::istream& stream, Connection& connection);
	};


	class Network
	{
	public:
		typedef std::vector<std::unique_ptr<Layer>> Layers;
		typedef std::vector<std::unique_ptr<Connection>> Connections;
		const Layers& GetLayers() const;
		const Layer& operator[](unsigned index) const { return *layers[index].get(); }
		Layer& operator[](unsigned index) { return *layers[index].get(); }
		State Activate(const Sample& sample);
		InputLayer& Add(size_t units, std::unique_ptr<Function>&& function);
		HiddenLayer& Add(Layer& layer, size_t units, std::unique_ptr<Function>&& function);
		void Reset(double mean = 0, double sigma = 0);
	private:
		Layers layers;
		Connections connections;

		friend std::ostream& operator<< (std::ostream& stream, const Network& network);
		friend std::istream& operator>> (std::istream& stream, Network& network);
	};


}
