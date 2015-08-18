#include "stdafx.h"
#include <Eigen/Dense>
#include "Activation.h"
#include "Network.h"
#include "Sample.h"

namespace Net
{
Activation::Activation(Generation generation, const Layer& layer, const Eigen::VectorXd& input) :
	generation(generation),
	layer(layer),
	activation(input)
{
	if (input.size() != activation.size())
		throw std::runtime_error("Layer activated with incompatible state");
}

State::State(const Network& network, const Sample& sample) :
	network(network)
{
	Input(sample);
	Propagate();
}

void State::Input(const Sample& sample)
{
	size_t pos = 0;
	for (const auto& layer : network.GetLayers())
	{
		if (InputLayer* input = dynamic_cast<InputLayer*>(layer.get()))
		{
			activity.emplace_back(Activation(0, *input, sample.Activation(pos, input->Size())));
			pos += input->Size();
		}
	}
}

void State::Propagate()
{
	while (!activity.empty())
	{
		Activity future;
		for (const auto& layer : activity)
		{
			// TODO: following block should be in Activion, but it can't declare a list of activations to return
			for (const auto& connection : layer.layer.GetConnections())
			{
				Eigen::VectorXd excitation = connection->GetWeights() * layer.activation;
				Eigen::VectorXd activation = connection->B().GetFunction()->Activate(excitation);
				future.emplace_back(Activation(layer.generation + 1, connection->B(), activation));
			}
			// TODO future.insert(future.end(), newActivity.begin(), newActivity.end());
		}
		activity.swap(future);
	}
}


}
