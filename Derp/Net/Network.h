#pragma once

#include <Eigen/Dense>
#include <vector>
#include <memory>

#include "Activation.h"

class Connection;
class Sample;

class Layer
{
public:
	typedef std::vector<Connection*> Connections;
	Layer();
	Layer(size_t units);
	virtual ~Layer();
	void Connect(Connection& connection);
	size_t Size() const;
	const Eigen::VectorXd& Bias() const { return bias; }
	const Connections& GetConnections() const { return connections;  }
	void Reset(double mean, double sigma);
private:
	size_t units;
	Connections connections;
	Eigen::VectorXd bias;

	friend std::ostream& operator<< (std::ostream& stream, const Layer& layer);
	friend std::istream& operator>> (std::istream& stream, Layer& layer);
};

class InputLayer : public Layer
{
public:
	InputLayer();
	InputLayer(size_t units);
};

class HiddenLayer : public Layer
{
public:
	HiddenLayer();
	HiddenLayer(size_t units);

};

class Connection
{
public:
	Connection(Layer& a, const Layer&  b);
	const Layer& A() const { return a; }
	const Layer& B() const { return b; }
	const Eigen::MatrixXd& Weights() const { return weights; }
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
	State Activate(const Sample& sample);
	InputLayer& Add(size_t units);
	HiddenLayer& Add(Layer& layer, size_t units);
	void Reset(double mean=0, double sigma=0);
private:
	Layers layers;
	Connections connections;

	 friend std::ostream& operator<< (std::ostream& stream, const Network& network);
	 friend std::istream& operator>> (std::istream& stream, Network& network);
};


