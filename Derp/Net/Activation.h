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
		Activation& operator=(const Activation& other);
	private:
		friend class State;	// See TODO in State::Propagate
		Generation generation;
		const Layer* layer;
		Eigen::VectorXd activation;
	};


	class State
	{
	public:
		State(const Network& network, const Sample& sample);
		Eigen::VectorXd State::GetActivation(const Layer& layer) const;
	private:
		void Input(const Sample& sample);
		void Propagate();
	private:
		typedef std::vector<Activation> Activity;
		Activity activity;
		Activity history;
		const Network& network;
	};
}