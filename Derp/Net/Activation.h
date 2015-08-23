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
		const Layer::Base& GetLayer() const { return *layer;  }
	private:
		friend class State;	// See TODO in State::Propagate
		Generation generation;
		const Layer::Base* layer;
		Eigen::VectorXd activation;
	};

	class Activity : public std::vector<Activation>
	{
	public:
		bool CanGoForward() const;
		bool CanGoBackward() const;
	};

	class State
	{
	public:
		State(const Network& network);
		void Input(const Data::Inputs& sample);
		void Step();
		void Reconstruct();
		void Propagate();
		Data::Outputs Output() const;
	private:
		Eigen::VectorXd State::GetActivation(const Layer::Base& layer) const;
		Activity activity;
		const Network& network;
	};

	class Computation : protected State
	{
	public:
		Computation(const Network& network, const Data::Inputs& sample);
		Data::Outputs operator()() const { return Output(); }
	};

}
