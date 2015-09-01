#pragma once
#include <Eigen/Dense>
#include <map>
#include "Net.h"

namespace Net
{
namespace Activation
{
	class Activation
	{
	public:
		Activation(const Eigen::VectorXd& activation);
		Activation& operator=(const Activation& other);
		Eigen::VectorXd GetActivation() const { return activation; }
	private:
		friend class Activity;
		Eigen::VectorXd activation;
		Eigen::VectorXd excitation;
	};

	class Activity : private  std::map < const Layer::Base*, Activation >
	{
	public:
		Eigen::VectorXd operator[](const Layer::Base& layer) const { return GetActivation(layer); }
		void Activate(const Layer::Base& layer, const Activation& activation);
		bool CanGoForward() const;
		bool CanGoBackward() const;
		Activity Step() const;
		Activity Reconstruct() const;
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
		void Apply(const Activity& activity);	// TODO: this is getting weird
	protected:
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
}