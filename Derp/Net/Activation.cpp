#include <Eigen/Dense>
#include <algorithm>
#include "Net.h"
#include "Activation.h"

namespace Net
{
namespace Activation
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

	Net::Activation::Activity State::Input(const Data::Inputs& inputs) const
	{
		Activity future;
		size_t pos = 0;
		for (const auto& input : inputs)
			future.emplace_back(Activation(0, network[input.first], input.second.activation));
		return future;
	}

	bool Activity::CanGoForward() const
	{
		for (const auto& activation : *this)
			if (!activation.GetLayer().connections.empty())
				return true;
		return false;
	}

	bool Activity::CanGoBackward() const
	{
		for (const auto& activation : *this)
			if (!activation.GetLayer().reverse_connections.empty())
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

	Net::Activation::Activity State::Step() const
	{
		Activity future;
		for (const auto& activation : activity)
		{
			// TODO: following block should be in Activation, but it can't declare a list of activations to return
			for (const auto& connection : activation.layer->connections)
			{
				Eigen::VectorXd excitation = connection.get().weights * activation.activation + connection.get().b.bias;
				Eigen::VectorXd activationVector = connection.get().b.GetFunction()(excitation);
				future.emplace_back(Activation(activation.generation + 1, connection.get().b, activationVector));
			}
			// TODO future.insert(future.end(), newActivity.begin(), newActivity.end());
		}
		return future;
	}

	Net::Activation::Activity State::Reconstruct() const
	{
		Activity future;
		for (const auto& activation : activity)
		{
			// TODO: following block should be in Activation, but it can't declare a list of activations to return
			for (const auto& connection : activation.layer->reverse_connections)
			{
				Eigen::VectorXd excitation = connection.get().weights.transpose() * activation.activation + connection.get().a.bias;
				Eigen::VectorXd activationVector = connection.get().a.GetFunction()(excitation);
				future.emplace_back(Activation(activation.generation + 1, connection.get().a, activationVector));
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
			output.insert(std::make_pair(outputId, Data::Output(GetActivation(network[outputId]))));
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

