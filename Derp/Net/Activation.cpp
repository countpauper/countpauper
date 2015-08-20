#include "stdafx.h"
#include <Eigen/Dense>
#include "Activation.h"
#include "Net.h"

namespace Net
{
Activation::Activation(Generation generation, const Layer::Base& layer, const Eigen::VectorXd& input) :
	generation(generation),
	layer(&layer),
	activation(input)
{
	if (input.size() != activation.size())
		throw std::runtime_error("Layer::Base activated with incompatible state");
}

Activation& Activation::operator=(const Activation& other)
{
	generation = other.generation;
	layer = other.layer;
	activation = other.activation;
	return *this;
}

State::State(const Network& network) :
	network(network)
{
}

void State::Input(const Data::Inputs& inputs)
{
	size_t pos = 0;
	for (const auto& input : inputs)
		activity.emplace_back(Activation(0, network[input.layer], input.activation));
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
				Eigen::VectorXd excitation = connection->GetWeights() * activation.activation + connection->B().Bias();
				Eigen::VectorXd activationVector = connection->B().GetFunction()(excitation);
				future.emplace_back(Activation(activation.generation + 1, connection->B(), activationVector));
			}
			// TODO future.insert(future.end(), newActivity.begin(), newActivity.end());
		}
		history.insert(history.end(), activity.begin(), activity.end());
		activity.assign(future.begin(), future.end());
	}
}

Eigen::VectorXd State::GetActivation(const Layer::Base& layer) const
{
	for (const auto& activation : history)
	{
		if (activation.layer == &layer)
			return activation.activation;
	}
	return Eigen::VectorXd::Zero(layer.Size());
}

Eigen::VectorXd State::GetHistory(const Layer::Base& layer) const
{
	for (Activity::const_reverse_iterator historyIt = history.crbegin(); historyIt != history.crend(); ++historyIt)
	{
		if (historyIt->layer == &layer)
			return historyIt->activation;
	}
	return Eigen::VectorXd::Constant(layer.Size(), 0);
}


Data::Outputs State::Output() const
{
	Data::Outputs output;
	for (const auto& outputId : network.GetOutputs())
	{
		output.emplace_back(Data::Output(outputId, GetHistory(network[outputId])));
	}
	return output;
}

Computation::Computation(const Network& network, const Data::Inputs& inputs) :
	State(network)
{
	Input(inputs);
	Propagate();
}

}

