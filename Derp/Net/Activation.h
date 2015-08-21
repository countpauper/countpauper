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
		Eigen::VectorXd GetActivation() const { return activation;  }
		Eigen::VectorXd GetExcitation() const { return excitation;  }
	private:
		friend class State;	// See TODO in State::Propagate
		Generation generation;
		const Layer::Base* layer;
		Eigen::VectorXd activation;
		Eigen::VectorXd excitation;
	};

	class Activity : public std::vector < Activation >
	{
	public:
		Eigen::VectorXd operator[](const Layer::Base& layer) const { return GetExcitation(layer); }
		Eigen::VectorXd operator()(const Layer::Base& layer) const { return GetActivation(layer); }
	private:
		Eigen::VectorXd GetExcitation(const Layer::Base& layer) const;
		Eigen::VectorXd GetActivation(const Layer::Base& layer) const;
	};

	class State
	{
	public:
		State(const Network& network);
		Activity Input(const Data::Inputs& sample);
		Activity Step();
		void Propagate();
		Data::Outputs Output() const;
	private:
		Eigen::VectorXd State::GetActivation(const Layer::Base& layer) const;
		Eigen::VectorXd State::GetHistory(const Layer::Base& layer) const;
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
