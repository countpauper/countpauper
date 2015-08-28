#include <Eigen/Dense>
#include <algorithm>
#include "Activation.h"
#include "Net.h"

namespace Net
{

Activation::Activation(Generation generation, const Layer::Base& layer, const Eigen::VectorXd& activation) :
	// TODO, when merging, passing the excitation is not correct for input activation, still somehow need to remember probabilities
	generation(generation),
	layer(&layer),
	activation(activation)
{	
	if (layer.Size() != activation.size())
		throw std::runtime_error("Layer::Base activated with incompatible state");
}

Activation& Activation::operator=(const Activation& other)
{
	generation = other.generation;
	layer = other.layer;
	activation = other.activation;
	return *this;
}


Eigen::VectorXd Activity::GetActivation(const Layer::Base& layer) const
{
	const_iterator it = std::find_if(cbegin(), cend(), [&layer](const Activation& activation){ return &activation.GetLayer() == &layer;  });
	if (it == end())
		throw std::out_of_range("No activity for that layer");
	else
		return it->GetActivation();
}

State::State(const Network& network) :
	network(network)
{
}

Net::Activity State::Input(const Data::Inputs& inputs) const
{
	Activity future;
	size_t pos = 0;
	for (const auto& input : inputs)
		future.emplace_back(Activation(0, network[input.layer], input.activation));
	return future;
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

void State::Apply(const Activity& newActivity)
{
	this->activity = newActivity;
}

void State::Propagate()
{
	while (activity.CanGoForward())
	{
		activity = Step();
	}
	while (activity.CanGoBackward())
	{
		activity = Reconstruct(); // TODO rename
	}
}

Net::Activity State::Step() const
{
	Activity future;
	for (const auto& activation : activity)
	{
		// TODO: following block should be in Activation, but it can't declare a list of activations to return
		for (const auto& connection : activation.layer->GetConnections())
		{
			Eigen::VectorXd excitation = connection.get().GetWeights() * activation.activation + connection.get().B().Bias();
			Eigen::VectorXd activationVector = connection.get().B().GetFunction()(excitation);
			future.emplace_back(Activation(activation.generation + 1, connection.get().B(), activationVector));
		}
		// TODO future.insert(future.end(), newActivity.begin(), newActivity.end());
	}
	return future;
}

Net::Activity State::Reconstruct() const
{
	Activity future;
	for (const auto& activation : activity)
	{
		// TODO: following block should be in Activation, but it can't declare a list of activations to return
		for (const auto& connection : activation.layer->GetReverseConnections())
		{
			Eigen::VectorXd excitation = connection.get().GetWeights().transpose() * activation.activation + connection.get().A().Bias();
			Eigen::VectorXd activationVector = connection.get().A().GetFunction()(excitation);
			future.emplace_back(Activation(activation.generation + 1, connection.get().A(), activationVector));
		}
		// TODO future.insert(future.end(), newActivity.begin(), newActivity.end());
	}
	return future;
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
	activity = Input(inputs);
	Propagate();
}

}

