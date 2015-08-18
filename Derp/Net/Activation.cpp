#include "stdafx.h"
#include "Activation.h"
#include "Network.h"
#include "Sample.h"
//TODO: activation RAII: state only exists after activation, only keep reference on creation, start recursively creating activation states on activate

Activation::Activation(const Layer& layer) :
	layer(layer),
	activation(layer.Size())
{

}

void Activation::Activate(const Eigen::VectorXd& input)
{
	if (input.size() != activation.size())
		throw std::runtime_error("Layer activated with incompatible state");
	activation = input;
}


State::State(const Network& network)
{
	for (const auto& layer : network.GetLayers())
	{
		if (InputLayer* l = dynamic_cast<InputLayer*>(layer.get()))
			inputLayers.emplace_back(Activation(*l));
	}
}


void State::Activate(const Sample& sample)
{
	size_t pos = 0;
	for (auto& layerState : inputLayers)
	{
		const InputLayer& input = *static_cast<const InputLayer*>(&layerState.GetLayer());
		layerState.Activate(sample.Activation(pos, input.Size()));
		pos += input.Size();
	}
}

