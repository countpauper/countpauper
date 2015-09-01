#include <Eigen/Dense>
#include <algorithm>
#include "Net.h"
#include "Activation.h"

namespace Net
{
namespace Activation
{
	Activation::Activation(const Eigen::VectorXd& activation) :
		// TODO, when merging, passing the excitation is not correct for input activation, still somehow need to remember probabilities
		activation(activation)
	{
	}

	Activation& Activation::operator=(const Activation& other)
	{
		activation = other.activation;
		return *this;
	}

	void Activity::Activate(const Layer::Base& layer, const Activation& activation)
	{
		if (layer.Size() != activation.GetActivation().size())
			throw std::runtime_error("Layer::Base activated with incompatible state");
		insert(std::make_pair(&layer, activation));

	}

	Eigen::VectorXd Activity::GetActivation(const Layer::Base& layer) const
	{
		const_iterator it = std::find_if(cbegin(), cend(), [&layer](const std::pair<const Layer::Base*,Activation>& activation){ return activation.first == &layer;  });
		if (it == end())
			throw std::out_of_range("No activity for that layer");
		else
			return it->second.GetActivation();
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
			future.Activate(network[input.first], Activation(input.second.activation));
		return future;
	}

	bool Activity::CanGoForward() const
	{
		for (const auto& activation : *this)
			if (!activation.first->connections.empty())
				return true;
		return false;
	}


	Net::Activation::Activity Activity::Step() const
	{
		Activity future;
		for (const auto& activation : *this)
		{
			for (const auto& connection : activation.first->connections)
			{
				Eigen::VectorXd excitation = connection.get().weights * activation.second.activation + connection.get().b.bias;
				Eigen::VectorXd activationVector = connection.get().b.GetFunction()(excitation);
				future.Activate(connection.get().b, Activation(activationVector));
			}
		}
		return future;
	}

	bool Activity::CanGoBackward() const
	{
		for (const auto& activation : *this)
			if (!activation.first->reverse_connections.empty())
				return true;
		return false;
	}

	Net::Activation::Activity Activity::Reconstruct() const
	{
		Activity future;
		for (const auto& activation : *this)
		{
			for (const auto& connection : activation.first->reverse_connections)
			{
				Eigen::VectorXd excitation = connection.get().weights.transpose() * activation.second.activation + connection.get().a.bias;
				Eigen::VectorXd activationVector = connection.get().a.GetFunction()(excitation);
				future.Activate(connection.get().a, Activation(activationVector));
			}
		}
		return future;
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
		return activity.Step();
	}

	Net::Activation::Activity State::Reconstruct() const
	{
		return activity.Reconstruct();
	}


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

