#include "stdafx.h"
#include "Data.h"

namespace Net
{
	namespace Data
	{

		Base::Base(Layer::Id layer, const Eigen::VectorXd& activation) :
			layer(layer),
			activation(activation)
		{
		}
		Input::Input(Layer::Id layer, const Eigen::VectorXd& activation) :
			Base(layer, activation)
		{
		}
		Output::Output(Layer::Id layer, const Eigen::VectorXd& activation) :
			Base(layer, activation)
		{
		}
		Sample::Sample(const Inputs& inputs, const Outputs& outputs) :
			inputs(inputs),
			outputs(outputs)
		{
		}
	}
}
