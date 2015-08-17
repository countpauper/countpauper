#pragma once
#include <Eigen/Dense>
#include <vector>
#include <memory>

class Connection;

class Layer
{
public:
	Layer(size_t units);
	virtual ~Layer();
	Layer& Connect(std::unique_ptr<Layer> layer);
	size_t Size() const;
private:
	size_t units;
	std::vector<std::unique_ptr<Connection>> connections;
	Eigen::VectorXd bias;
};

class InputLayer : public Layer
{
public:
	InputLayer(size_t units);
};

class HiddenLayer : public Layer
{
public:
	HiddenLayer(size_t units);

};

class LayerState
{
public:
	LayerState(const Layer& layer);
	void Activate(const Eigen::VectorXd& activation);
	const Layer& GetLayer() const { return layer;  }
private:
	const Layer& layer;
	Eigen::VectorXd activation;
};

class Connection
{
public:
	Connection(const Layer& a, std::unique_ptr<Layer> b);
	Layer* Target() { return target.get(); }
private:
	const Layer& owner;
	std::unique_ptr<Layer> target;
	Eigen::MatrixXd weights;
};
class NetworkState;
class Sample;

class Network
{
public:
	typedef std::vector<std::unique_ptr<InputLayer>> InputLayers;

	const InputLayers& GetInputs() const;

	NetworkState Activate(const Sample& sample);
protected:
	InputLayer& Add(size_t units);
	HiddenLayer& Add(Layer& layer, size_t units);
private:
	InputLayers inputs;
};

class Sample
{
public:
	Sample(const Eigen::VectorXd& activation);
	Eigen::VectorXd Activation(size_t from, size_t count) const;
protected:
	Eigen::VectorXd activation;
};


class NetworkState
{
public:
	typedef std::vector<LayerState> Layers;

	NetworkState(const Network& network);
	void Activate(const Sample& sample);
private:
	Layers inputLayers;
};

class RBM : public Network
{
public:
	RBM(size_t visible, size_t hidden);
private:
	InputLayer& visible;
};

