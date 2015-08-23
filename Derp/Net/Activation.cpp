#include "stdafx.h"
#include <Eigen/Dense>
#include "Activation.h"
#include "Net.h"

namespace Net
{
Activation::Activation(Generation generation, const Layer::Base& layer, const Eigen::VectorXd& input) :
	// TODO, when merging, passing the excitation is not correct for input activation, still somehow need to remember probabilities
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

bool Activity::CanGoForward() const
{
	for (const auto& activation : *this)
		if (!activation.GetLayer().GetConnections().empty())
			return true;
	return false;
}

bool Activity::CanGoBackward() const
{
	for (const auto& activation : *this)
		if (!activation.GetLayer().GetReverseConnections().empty())
			return true;
	return false;
}

void State::Propagate()
{
	while (activity.CanGoForward())
	{
		Step();
	}
	while (activity.CanGoBackward())
	{
		Reconstruct(); // TODO rename
	}
}


void State::Step()
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
	activity.assign(future.begin(), future.end());
}

void State::Reconstruct()
{
	Activity future;
	for (const auto& activation : activity)
	{
		// TODO: following block should be in Activation, but it can't declare a list of activations to return
		for (const auto& connection : activation.layer->GetReverseConnections())
		{
			Eigen::VectorXd excitation = connection->GetWeights().transpose() * activation.activation + connection->A().Bias();
			Eigen::VectorXd activationVector = connection->A().GetFunction()(excitation);
			future.emplace_back(Activation(activation.generation + 1, connection->A(), activationVector));
		}
		// TODO future.insert(future.end(), newActivity.begin(), newActivity.end());
	}
	activity.assign(future.begin(), future.end());
}

Eigen::VectorXd State::GetActivation(const Layer::Base& layer) const
{
	for (const auto& activation : activity)
	{
		if (activation.layer == &layer)
			return activation.activation;
	}
	return Eigen::VectorXd::Zero(layer.Size());
}

Data::Outputs State::Output() const
{
	Data::Outputs output;
	for (const auto& outputId : network.GetOutputs())
	{
		output.emplace_back(Data::Output(outputId, GetActivation(network[outputId])));
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

