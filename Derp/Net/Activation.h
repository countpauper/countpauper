#pragma once
#include <Eigen/Dense>
#include <vector>

class Network;
class Sample;
class Layer;

class Activation
{
public:
	Activation(const Layer& layer);
	void Activate(const Eigen::VectorXd& activation);
	const Layer& GetLayer() const { return layer; }
private:
	const Layer& layer;
	Eigen::VectorXd activation;
};

class State
{
public:
	typedef std::vector<Activation> Layers;

	State(const Network& network);
	void Activate(const Sample& sample);
private:
	Layers inputLayers;
};
