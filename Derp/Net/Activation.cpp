#include "stdafx.h"
#include <Eigen/Dense>
#include "Activation.h"
#include "Network.h"
#include "Sample.h"

namespace Net
{
Activation::Activation(Generation generation, const Layer& layer, const Eigen::VectorXd& input) :
	generation(generation),
	layer(&layer),
	activation(input)
{
	if (input.size() != activation.size())
		throw std::runtime_error("Layer activated with incompatible state");
}

Activation& Activation::operator=(const Activation& other)
{
	generation = other.generation;
	layer = other.layer;
	activation = other.activation;
	return *this;
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
		for (const auto& activation : activity)
		{
			// TODO: following block should be in Activation, but it can't declare a list of activations to return
			for (const auto& connection : activation.layer->GetConnections())
			{
				Eigen::VectorXd excitation = connection->GetWeights() * activation.activation;
				Eigen::VectorXd activationVector = connection->B().GetFunction()(excitation);
				future.emplace_back(Activation(activation.generation + 1, connection->B(), activationVector));
			}
			// TODO future.insert(future.end(), newActivity.begin(), newActivity.end());
		}
		history.insert(history.end(), activity.begin(), activity.end());
		activity.assign(future.begin(), future.end());
	}
}

Eigen::VectorXd State::GetActivation(const Layer& layer) const
{
	for (const auto& activation : history)
	{
		if (activation.layer == &layer)
			return activation.activation;
	}
	return Eigen::VectorXd::Zero(layer.Size());
}
}

