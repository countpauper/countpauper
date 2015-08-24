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
	private:
		friend class State;	// See TODO in State::Propagate
		Generation generation;
		const Layer::Base* layer;
		Eigen::VectorXd activation;
		Eigen::VectorXd excitation;
	};

	class Activity : public  std::vector < Activation >
	{
	public:
		Eigen::VectorXd operator[](const Layer::Base& layer) const { return GetActivation(layer); }
		bool CanGoForward() const;
		bool CanGoBackward() const;
	private:
		Eigen::VectorXd GetActivation(const Layer::Base& layer) const;
	};

	class State
	{
	public:
		State(const Network& network);
		Activity Input(const Data::Inputs& sample) const;
		Activity Step() const;
		Activity Reconstruct() const;
		void Propagate();
		Data::Outputs Output() const;
	protected:
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
