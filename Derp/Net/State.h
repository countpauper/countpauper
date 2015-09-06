#pragma once
#include <Eigen/Dense>
#include <map>
#include "Net.h"

namespace Net
{
namespace Activation
{
	class State : public Activity
	{
	public:
		State(const Network& network);
		void Input(const Data::Inputs& sample);
		void Clamp(const Data::Outputs& sample);
		void Propagate();
		Data::Outputs Output() const;
	protected:
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