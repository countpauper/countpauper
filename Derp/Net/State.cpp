#include <Eigen/Dense>
#include <algorithm>
#include "Net.h"
#include "State.h"

namespace Net
{
namespace Activation
{
	Data::Outputs State::Output() const
	{
		Data::Outputs output;
		for (const auto& outputId : network.GetOutputs())
		{
			output.insert(std::make_pair(outputId, Data::Output(activity[network[outputId]])));
		}
		return output;
	}

	Computation::Computation(const Network& network, const Data::Inputs& inputs) :
		State(network)
	{
		activity = Input(inputs);
		Propagate();
	}
}	
}

