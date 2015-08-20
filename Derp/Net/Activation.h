#pragma once
#include <Eigen/Dense>
#include <vector>
#include "Data.h"

namespace Net
{
	class Network;
	namespace Layer { class Base; }

	class Activation
	{
	public:
		typedef unsigned Generation;
		Activation(Generation generation, const Layer::Base& layer, const Eigen::VectorXd& activation);
		Activation& operator=(const Activation& other);
	private:
		friend class State;	// See TODO in State::Propagate
		Generation generation;
		const Layer::Base* layer;
		Eigen::VectorXd activation;
	};


	class State
	{
	public:
		State(const Network& network);
	protected:
		void Input(const Data::Inputs& sample);
		void Propagate();
		Data::Outputs Output() const;
	private:
		Eigen::VectorXd State::GetActivation(const Layer::Base& layer) const;
		Eigen::VectorXd State::GetHistory(const Layer::Base& layer) const;
		typedef std::vector<Activation> Activity;
		Activity activity;
		Activity history;
		const Network& network;
	};

	class Computation : protected State
	{
	public:
		Computation(const Network& network, const Data::Inputs& sample);
		Data::Outputs operator()() const { return Output(); }
	};
}