#include <Eigen/Dense>
#include <algorithm>
#include "Net.h"
#include "State.h"

namespace Net
{
namespace Activation
{
	State::State(const Network& network) :
		network(network)
	{
	}

	void State::Input(const Data::Inputs& inputs)
	{	// TODO: this is just activity += activity
		size_t pos = 0;
		for (const auto& input : inputs)
			Activate(network[input.first], Activation(input.second.activation));
	}
	void State::Clamp(const Data::Outputs& outputs)
	{
		size_t pos = 0;
		for (const auto& output : outputs)
			Activate(network[output.first], Activation(output.second.activation));
	}

	Data::Outputs State::Output() const
	{
		Data::Outputs output;
		for (const auto& outputId : network.GetOutputs())
		{
			output.insert(std::make_pair(outputId, Data::Output(GetActivation(network[outputId]))));
		}
		return output;
	}

	void State::Propagate()
	{
		unsigned activationPropagated;
		do
		{
			activationPropagated = 0;
			for (const auto& layer : network)
			{
				if (IsActive(layer))
					continue;

				bool allActive = true;
				Eigen::VectorXd excitation = layer.bias;
				for (const auto& connection : layer.reverse_connections)
				{
					const auto& inputLayer = connection.get().a;
					if (IsActive(inputLayer))
						excitation += connection.get().weights * GetActivation(inputLayer);
					else
					{
						allActive = false;
						break;
					}
				}
				if (allActive)
				{
					Eigen::VectorXd activationVector = layer.GetFunction()(excitation);
					Activate(layer, activationVector);
					++activationPropagated;
				}
			}
		} while (activationPropagated > 0);
	}


	Computation::Computation(const Network& network, const Data::Inputs& inputs) :
		State(network)
	{
		Input(inputs);
		Propagate();
	}

}	
}

