#pragma once
#include <Eigen/Dense>
#include <vector>

namespace Net
{
	class Network;
	class Sample;
	class Layer;


	class Activation
	{
	public:
		typedef unsigned Generation;
		Activation(Generation generation, const Layer& layer, const Eigen::VectorXd& activation);
	private:
		friend class State;	// See TODO in State::Propagate
		Generation generation;
		const Layer& layer;
		Eigen::VectorXd activation;
	};

	typedef std::vector<Activation> Activity;

	class State
	{
	public:
		State(const Network& network, const Sample& sample);
	private:
		void Input(const Sample& sample);
		void Propagate();
	private:
		Activity activity;
		const Network& network;
	};
}