#pragma once
#include <Eigen/Dense>
#include <map>
#include "Net.h"

namespace Net
{
namespace Activation
{
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